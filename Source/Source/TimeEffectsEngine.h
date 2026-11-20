#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class TimeEffectsEngine
{
public:
    enum class Mode
    {
        DigitalDelay = 0,
        AnalogDelay,
        TapeDelay,
        PingPongDelay,
        MultiTapDelay,
        DubDelay,
        GlitchDelay,
        Reverb,
        Freeze
    };

    TimeEffectsEngine() = default;

    void prepare(double sampleRate, int numChannels)
    {
        currentSampleRate = sampleRate;
        const double smoothTime = 0.02;

        timeSmoothed.reset(sampleRate, smoothTime);
        feedbackSmoothed.reset(sampleRate, smoothTime);
        mixSmoothed.reset(sampleRate, smoothTime);
        toneSmoothed.reset(sampleRate, smoothTime);
        modulationSmoothed.reset(sampleRate, smoothTime);
        spreadSmoothed.reset(sampleRate, smoothTime);
        sizeSmoothed.reset(sampleRate, smoothTime);

        // Максимальное время задержки: 10 секунд
        const int maxDelaySamples = static_cast<int>(sampleRate * 10.0) + 1000;
        
        // Инициализация основных линий задержки
        delayLineLeft.prepare({ sampleRate, 1, static_cast<juce::uint32>(maxDelaySamples) });
        delayLineRight.prepare({ sampleRate, 1, static_cast<juce::uint32>(maxDelaySamples) });
        delayLineLeft.reset();
        delayLineRight.reset();

        // Multi-tap: 4 отвода
        multiTapDelays.resize(4);
        for (auto& delay : multiTapDelays)
        {
            delay.prepare({ sampleRate, 1, static_cast<juce::uint32>(maxDelaySamples) });
            delay.reset();
        }

        // Reverb: 4 comb + 2 allpass фильтра на канал
        combFiltersLeft.resize(4);
        combFiltersRight.resize(4);
        allpassFiltersLeft.resize(2);
        allpassFiltersRight.resize(2);

        for (auto& f : combFiltersLeft)   { f.prepare(sampleRate); f.setSize(static_cast<int>(sampleRate * 0.05)); }
        for (auto& f : combFiltersRight)  { f.prepare(sampleRate); f.setSize(static_cast<int>(sampleRate * 0.05)); }
        for (auto& f : allpassFiltersLeft)  { f.prepare(sampleRate); f.setSize(static_cast<int>(sampleRate * 0.005)); }
        for (auto& f : allpassFiltersRight) { f.prepare(sampleRate); f.setSize(static_cast<int>(sampleRate * 0.005)); }

        // Фильтры для feedback path (Tone control)
        feedbackFilterLeft.prepare(sampleRate);
        feedbackFilterRight.prepare(sampleRate);

        // Glitch: буфер для стохастических изменений
        glitchBuffer.setSize(numChannels, maxDelaySamples);
        glitchBuffer.clear();
    }

    void setTargetValues(float timeMs, float feedback, float mix, float tone, 
                         float modulation, float spread, float size)
    {
        timeSmoothed.setTargetValue(timeMs);              // 1.0 ... 10000.0 ms
        feedbackSmoothed.setTargetValue(feedback);        // 0.0 ... 0.95
        mixSmoothed.setTargetValue(mix);                  // 0.0 ... 1.0
        toneSmoothed.setTargetValue(tone);                // 0.0 ... 1.0
        modulationSmoothed.setTargetValue(modulation);    // 0.0 ... 1.0
        spreadSmoothed.setTargetValue(spread);            // 0.0 ... 1.0
        sizeSmoothed.setTargetValue(size);                // 0.0 ... 1.0
    }

    void process(juce::AudioBuffer<float>& buffer, Mode mode)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Получаем сглаженные параметры
            const float timeMs     = timeSmoothed.getNextValue();
            const float feedback   = feedbackSmoothed.getNextValue();
            const float mix        = mixSmoothed.getNextValue();
            const float tone       = toneSmoothed.getNextValue();
            const float modulation = modulationSmoothed.getNextValue();
            const float spread     = spreadSmoothed.getNextValue();
            const float size       = sizeSmoothed.getNextValue();

            // Преобразуем время в сэмплы
            const float delaySamples = static_cast<float>(currentSampleRate * timeMs / 1000.0);
            
            // Tone: cutoff frequency от 200 Hz до 20 kHz
            const float cutoffFreq = 200.0f + (tone * tone * 19800.0f);
            const float filterCoeff = juce::jlimit(0.0f, 1.0f, 
                2.0f * juce::MathConstants<float>::pi * cutoffFreq / static_cast<float>(currentSampleRate));

            // Обновляем фильтры feedback path
            feedbackFilterLeft.setCoefficient(filterCoeff);
            feedbackFilterRight.setCoefficient(filterCoeff);

            float dryL = buffer.getSample(0, sample);
            float dryR = (numChannels > 1) ? buffer.getSample(1, sample) : dryL;
            float wetL = dryL;
            float wetR = dryR;

            switch (mode)
            {
                case Mode::DigitalDelay:
                {
                    delayLineLeft.setDelay(delaySamples);
                    delayLineRight.setDelay(delaySamples);
                    
                    float delL = delayLineLeft.processSample(dryL);
                    float delR = delayLineRight.processSample(dryR);
                    
                    // Feedback с фильтрацией
                    float fbL = feedbackFilterLeft.process(delL * feedback);
                    float fbR = feedbackFilterRight.process(delR * feedback);
                    
                    delayLineLeft.pushSample(dryL + fbL);
                    delayLineRight.pushSample(dryR + fbR);
                    
                    wetL = delL;
                    wetR = delR;
                    break;
                }

                case Mode::AnalogDelay:
                {
                    // Эмуляция аналоговой задержки: НЧ фильтр + легкая модуляция
                    const float modAmount = modulation * 0.002f * static_cast<float>(currentSampleRate); // до 2мс
                    const float modL = std::sin(sample * 0.001f) * modAmount;
                    const float modR = std::cos(sample * 0.001f) * modAmount;
                    
                    delayLineLeft.setDelay(delaySamples + modL);
                    delayLineRight.setDelay(delaySamples + modR);
                    
                    float delL = delayLineLeft.processSample(dryL);
                    float delR = delayLineRight.processSample(dryR);
                    
                    // Сильная фильтрация в feedback path (аналоговое звучание)
                    float fbL = feedbackFilterLeft.process(delL * feedback * 0.7f);
                    float fbR = feedbackFilterRight.process(delR * feedback * 0.7f);
                    
                    delayLineLeft.pushSample(dryL + fbL);
                    delayLineRight.pushSample(dryR + fbR);
                    
                    wetL = delL;
                    wetR = delR;
                    break;
                }

                case Mode::TapeDelay:
                {
                    // Эмуляция ленты: wow/flutter + насыщение
                    const float wowFreq = 0.5f + modulation * 2.0f; // 0.5 - 2.5 Hz
                    const float wowAmount = modulation * 0.005f * static_cast<float>(currentSampleRate); // до 5мс
                    const float flutterAmount = modulation * 0.001f * static_cast<float>(currentSampleRate);
                    
                    const float wowL = std::sin(sample * wowFreq / static_cast<float>(currentSampleRate) * 2.0f * juce::MathConstants<float>::pi) * wowAmount;
                    const float flutterL = std::sin(sample * 50.0f / static_cast<float>(currentSampleRate) * 2.0f * juce::MathConstants<float>::pi) * flutterAmount;
                    
                    delayLineLeft.setDelay(delaySamples + wowL + flutterL);
                    delayLineRight.setDelay(delaySamples + wowL * 0.8f + flutterL * 0.8f);
                    
                    float delL = delayLineLeft.processSample(dryL);
                    float delR = delayLineRight.processSample(dryR);
                    
                    // Насыщение в feedback path (tape saturation)
                    float fbL = std::tanh(delL * feedback * 1.5f) * 0.8f;
                    float fbR = std::tanh(delR * feedback * 1.5f) * 0.8f;
                    fbL = feedbackFilterLeft.process(fbL);
                    fbR = feedbackFilterRight.process(fbR);
                    
                    delayLineLeft.pushSample(dryL + fbL);
                    delayLineRight.pushSample(dryR + fbR);
                    
                    wetL = delL;
                    wetR = delR;
                    break;
                }

                case Mode::PingPongDelay:
                {
                    delayLineLeft.setDelay(delaySamples);
                    delayLineRight.setDelay(delaySamples);
                    
                    float delL = delayLineLeft.processSample(dryL);
                    float delR = delayLineRight.processSample(dryR);
                    
                    // Ping-pong: чередование каналов с учетом spread
                    float crossL = delR * spread;
                    float crossR = delL * spread;
                    
                    float fbL = feedbackFilterLeft.process((delL * (1.0f - spread) + crossL) * feedback);
                    float fbR = feedbackFilterRight.process((delR * (1.0f - spread) + crossR) * feedback);
                    
                    delayLineLeft.pushSample(dryL + fbL);
                    delayLineRight.pushSample(dryR + fbR);
                    
                    wetL = delL;
                    wetR = delR;
                    break;
                }

                case Mode::MultiTapDelay:
                {
                    // 4 отвода с разным временем (25%, 50%, 75%, 100% от основного)
                    const float tap1 = delaySamples * 0.25f;
                    const float tap2 = delaySamples * 0.50f;
                    const float tap3 = delaySamples * 0.75f;
                    const float tap4 = delaySamples * 1.00f;
                    
                    multiTapDelays[0].setDelay(tap1);
                    multiTapDelays[1].setDelay(tap2);
                    multiTapDelays[2].setDelay(tap3);
                    multiTapDelays[3].setDelay(tap4);
                    
                    float tapL = 0.0f;
                    float tapR = 0.0f;
                    
                    for (int i = 0; i < 4; ++i)
                    {
                        float gain = 1.0f - (i * 0.2f); // Затухание: 1.0, 0.8, 0.6, 0.4
                        tapL += multiTapDelays[i].processSample(dryL) * gain;
                        tapR += multiTapDelays[i].processSample(dryR) * gain;
                        
                        // Push с feedback
                        multiTapDelays[i].pushSample(dryL + tapL * feedback * 0.25f);
                    }
                    
                    wetL = tapL * 0.5f; // Нормализация
                    wetR = tapR * 0.5f;
                    break;
                }

                case Mode::DubDelay:
                {
                    // Экстремальный feedback + сильная фильтрация
                    delayLineLeft.setDelay(delaySamples);
                    delayLineRight.setDelay(delaySamples);
                    
                    float delL = delayLineLeft.processSample(dryL);
                    float delR = delayLineRight.processSample(dryR);
                    
                    // Агрессивная фильтрация (bandpass-эффект)
                    float fbL = feedbackFilterLeft.process(delL * feedback);
                    float fbR = feedbackFilterRight.process(delR * feedback);
                    
                    // Дополнительная модуляция для "dub" звучания
                    const float dubMod = std::sin(sample * 0.0005f) * modulation * 0.003f * static_cast<float>(currentSampleRate);
                    delayLineLeft.setDelay(delaySamples + dubMod);
                    
                    delayLineLeft.pushSample(dryL + fbL);
                    delayLineRight.pushSample(dryR + fbR);
                    
                    wetL = delL;
                    wetR = delR;
                    break;
                }

                case Mode::GlitchDelay:
                {
                    // Стохастические изменения времени задержки
                    const float glitchAmount = modulation * 0.01f * static_cast<float>(currentSampleRate); // до 10мс
                    const float randomOffset = (std::rand() % 1000) / 1000.0f * glitchAmount;
                    
                    delayLineLeft.setDelay(delaySamples + randomOffset);
                    delayLineRight.setDelay(delaySamples + randomOffset * 0.5f);
                    
                    float delL = delayLineLeft.processSample(dryL);
                    float delR = delayLineRight.processSample(dryR);
                    
                    float fbL = feedbackFilterLeft.process(delL * feedback);
                    float fbR = feedbackFilterRight.process(delR * feedback);
                    
                    delayLineLeft.pushSample(dryL + fbL);
                    delayLineRight.pushSample(dryR + fbR);
                    
                    wetL = delL;
                    wetR = delR;
                    break;
                }

                case Mode::Reverb:
                {
                    // Классический алгоритм: 4 comb + 2 allpass на канал
                    // Размеры comb фильтров зависят от size
                    const float baseSize = 0.02f + size * 0.08f; // 20ms ... 100ms
                    
                    combFiltersLeft[0].setSize(static_cast<int>(currentSampleRate * baseSize * 1.0f));
                    combFiltersLeft[1].setSize(static_cast<int>(currentSampleRate * baseSize * 1.1f));
                    combFiltersLeft[2].setSize(static_cast<int>(currentSampleRate * baseSize * 1.2f));
                    combFiltersLeft[3].setSize(static_cast<int>(currentSampleRate * baseSize * 1.3f));
                    
                    combFiltersRight[0].setSize(static_cast<int>(currentSampleRate * baseSize * 1.05f));
                    combFiltersRight[1].setSize(static_cast<int>(currentSampleRate * baseSize * 1.15f));
                    combFiltersRight[2].setSize(static_cast<int>(currentSampleRate * baseSize * 1.25f));
                    combFiltersRight[3].setSize(static_cast<int>(currentSampleRate * baseSize * 1.35f));
                    
                    // Обработка Left
                    float combSumL = 0.0f;
                    for (auto& f : combFiltersLeft)
                    {
                        combSumL += f.processSample(dryL, feedback);
                    }
                    for (auto& f : allpassFiltersLeft)
                    {
                        combSumL = f.processSample(combSumL, 0.7f);
                    }
                    
                    // Обработка Right
                    float combSumR = 0.0f;
                    for (auto& f : combFiltersRight)
                    {
                        combSumR += f.processSample(dryR, feedback);
                    }
                    for (auto& f : allpassFiltersRight)
                    {
                        combSumR = f.processSample(combSumR, 0.7f);
                    }
                    
                    // Damping (tone control)
                    wetL = feedbackFilterLeft.process(combSumL) * 0.5f;
                    wetR = feedbackFilterRight.process(combSumR) * 0.5f;
                    break;
                }

                case Mode::Freeze:
                {
                    // Freeze: reverb с feedback = 0.99 и без input signal
                    const float freezeFeedback = 0.99f;
                    
                    const float baseSize = 0.05f + size * 0.15f; // 50ms ... 200ms
                    
                    for (int i = 0; i < 4; ++i)
                    {
                        combFiltersLeft[i].setSize(static_cast<int>(currentSampleRate * baseSize * (1.0f + i * 0.1f)));
                        combFiltersRight[i].setSize(static_cast<int>(currentSampleRate * baseSize * (1.05f + i * 0.1f)));
                    }
                    
                    // Только feedback, без input
                    float combSumL = 0.0f;
                    for (auto& f : combFiltersLeft)
                    {
                        combSumL += f.processSample(0.0f, freezeFeedback);
                    }
                    for (auto& f : allpassFiltersLeft)
                    {
                        combSumL = f.processSample(combSumL, 0.7f);
                    }
                    
                    float combSumR = 0.0f;
                    for (auto& f : combFiltersRight)
                    {
                        combSumR += f.processSample(0.0f, freezeFeedback);
                    }
                    for (auto& f : allpassFiltersRight)
                    {
                        combSumR = f.processSample(combSumR, 0.7f);
                    }
                    
                    wetL = feedbackFilterLeft.process(combSumL) * 0.5f;
                    wetR = feedbackFilterRight.process(combSumR) * 0.5f;
                    
                    // "Замораживаем" входной сигнал (не добавляем в reverb)
                    wetL += dryL * 0.1f; // Минимальный dry для стабильности
                    wetR += dryR * 0.1f;
                    break;
                }
            }

            // Dry/Wet Mix
            wetL = dryL * (1.0f - mix) + wetL * mix;
            wetR = dryR * (1.0f - mix) + wetR * mix;

            // Защита от клиппинга
            wetL = juce::jlimit(-1.0f, 1.0f, wetL);
            wetR = juce::jlimit(-1.0f, 1.0f, wetR);

            buffer.setSample(0, sample, wetL);
            if (numChannels > 1)
                buffer.setSample(1, sample, wetR);
        }
    }

    juce::LinearSmoothedValue<float>& getTimeSmoothed() { return timeSmoothed; }
    juce::LinearSmoothedValue<float>& getFeedbackSmoothed() { return feedbackSmoothed; }
    juce::LinearSmoothedValue<float>& getMixSmoothed() { return mixSmoothed; }
    juce::LinearSmoothedValue<float>& getToneSmoothed() { return toneSmoothed; }
    juce::LinearSmoothedValue<float>& getModulationSmoothed() { return modulationSmoothed; }
    juce::LinearSmoothedValue<float>& getSpreadSmoothed() { return spreadSmoothed; }
    juce::LinearSmoothedValue<float>& getSizeSmoothed() { return sizeSmoothed; }

private:
    double currentSampleRate = 44100.0;

    juce::LinearSmoothedValue<float> timeSmoothed;
    juce::LinearSmoothedValue<float> feedbackSmoothed;
    juce::LinearSmoothedValue<float> mixSmoothed;
    juce::LinearSmoothedValue<float> toneSmoothed;
    juce::LinearSmoothedValue<float> modulationSmoothed;
    juce::LinearSmoothedValue<float> spreadSmoothed;
    juce::LinearSmoothedValue<float> sizeSmoothed;

    // Основные линии задержки
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineLeft;
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLineRight;

    // Multi-tap
    std::vector<juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>> multiTapDelays;

    // Reverb filters
    struct CombFilter
    {
        float* buffer = nullptr;
        int bufferSize = 0;
        int writeIndex = 0;
        float lastOut = 0.0f;

        void prepare(double) {}
        void setSize(int size)
        {
            if (size != bufferSize)
            {
                delete[] buffer;
                bufferSize = size;
                buffer = new float[bufferSize]();
                writeIndex = 0;
                lastOut = 0.0f;
            }
        }
        ~CombFilter() { delete[] buffer; }

        float processSample(float in, float feedback)
        {
            float out = buffer[writeIndex];
            lastOut = out;
            buffer[writeIndex] = in + out * feedback;
            writeIndex = (writeIndex + 1) % bufferSize;
            return out;
        }
    };

    struct AllpassFilter
    {
        float* buffer = nullptr;
        int bufferSize = 0;
        int writeIndex = 0;

        void prepare(double) {}
        void setSize(int size)
        {
            if (size != bufferSize)
            {
                delete[] buffer;
                bufferSize = size;
                buffer = new float[bufferSize]();
                writeIndex = 0;
            }
        }
        ~AllpassFilter() { delete[] buffer; }

        float processSample(float in, float gain)
        {
            float buffered = buffer[writeIndex];
            float out = -in * gain + buffered;
            buffer[writeIndex] = in + buffered * gain;
            writeIndex = (writeIndex + 1) % bufferSize;
            return out;
        }
    };

    std::vector<CombFilter> combFiltersLeft;
    std::vector<CombFilter> combFiltersRight;
    std::vector<AllpassFilter> allpassFiltersLeft;
    std::vector<AllpassFilter> allpassFiltersRight;

    // Feedback path filters
    struct SimpleLPFilter
    {
        float y1 = 0.0f;
        float coeff = 1.0f;

        void prepare(double) { y1 = 0.0f; }
        void setCoefficient(float c) { coeff = juce::jlimit(0.0f, 1.0f, c); }
        float process(float in)
        {
            y1 += coeff * (in - y1);
            return y1;
        }
    };

    SimpleLPFilter feedbackFilterLeft;
    SimpleLPFilter feedbackFilterRight;

    juce::AudioBuffer<float> glitchBuffer;
};
