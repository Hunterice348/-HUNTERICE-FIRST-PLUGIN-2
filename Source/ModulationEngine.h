#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class ModulationEngine
{
public:
    enum class Mode
    {
        Chorus = 0,
        Flanger,
        Phaser,
        Tremolo,
        Vibrato,
        RingMod,
        FreqMod,       // Audio-rate delay modulation (FM-эффект)
        StereoMod      // Динамическое изменение стерео-базы
    };

    ModulationEngine() = default;

    void prepare(double sampleRate, int numChannels)
    {
        currentSampleRate = sampleRate;
        const double smoothTime = 0.02; // 20 мс сглаживание

        rateSmoothed.reset(sampleRate, smoothTime);
        depthSmoothed.reset(sampleRate, smoothTime);
        mixSmoothed.reset(sampleRate, smoothTime);
        feedbackSmoothed.reset(sampleRate, smoothTime);
        widthSmoothed.reset(sampleRate, smoothTime);

        // Инициализация LFO (Left и Right для стерео-ширины)
        lfoLeft.initialise([](float x) { return std::sin(x); }, 128);
        lfoRight.initialise([](float x) { return std::sin(x); }, 128);
        lfoLeft.setFrequency(1.0);
        lfoRight.setFrequency(1.0);

        // Инициализация линии задержки (макс. 50 мс для хоруса/фленжера)
        const int maxDelaySamples = static_cast<int>(sampleRate * 0.05) + 100;
        delayLine.prepare({ sampleRate, static_cast<juce::uint32>(numChannels), static_cast<juce::uint32>(maxDelaySamples) });
        delayLine.reset();

        // Инициализация Allpass-фильтров для Phaser (4 каскада)
        allpassFilters.resize(4);
        for (auto& filter : allpassFilters)
        {
            filter.prepare(sampleRate);
        }
        
        delayBuffer.setSize(numChannels, maxDelaySamples);
        delayBuffer.clear();
    }

    void setTargetValues(float rate, float depth, float mix, float feedback, float width)
    {
        rateSmoothed.setTargetValue(rate);          // 0.1 ... 20.0 Hz
        depthSmoothed.setTargetValue(depth);        // 0.0 ... 1.0
        mixSmoothed.setTargetValue(mix);            // 0.0 ... 1.0
        feedbackSmoothed.setTargetValue(feedback);  // 0.0 ... 0.95
        widthSmoothed.setTargetValue(width);        // 0.0 ... 1.0
    }

    void process(juce::AudioBuffer<float>& buffer, Mode mode)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        // Базовые задержки в сэмплах для разных режимов
        const float baseDelayMs = (mode == Mode::Flanger) ? 2.0f : 
                                  (mode == Mode::Vibrato || mode == Mode::FreqMod) ? 5.0f : 15.0f;
        const float baseDelaySamples = static_cast<float>(currentSampleRate * baseDelayMs / 1000.0);
        
        // Максимальная девиация задержки
        const float maxDeviationSamples = (mode == Mode::Flanger) ? 3.0f : 
                                          (mode == Mode::FreqMod) ? 10.0f : 10.0f;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // 1. Получаем сглаженные параметры
            const float rate     = rateSmoothed.getNextValue();
            const float depth    = depthSmoothed.getNextValue();
            const float mix      = mixSmoothed.getNextValue();
            const float feedback = feedbackSmoothed.getNextValue();
            const float width    = widthSmoothed.getNextValue();

            // 2. Обновляем LFO
            lfoLeft.setFrequency(rate);
            lfoRight.setFrequency(rate);
            // Width управляет фазовым сдвигом между каналами (0 = 0 рад, 1 = PI рад)
            const float phaseOffset = width * juce::MathConstants<float>::pi;
            
            float lfoL = lfoLeft.processSample(0.0f) * 0.5f + 0.5f; // 0.0 ... 1.0
            float lfoR = lfoRight.processSample(phaseOffset) * 0.5f + 0.5f; // 0.0 ... 1.0

            // Для Ring Mod и некоторых других нужен биполярный LFO (-1.0 ... 1.0)
            float lfoL_bipolar = lfoL * 2.0f - 1.0f;
            float lfoR_bipolar = lfoR * 2.0f - 1.0f;

            for (int channel = 0; channel < numChannels; ++channel)
            {
                float dry = buffer.getSample(channel, sample);
                float wet = dry;
                float lfo = (channel == 0) ? lfoL : lfoR;
                float lfo_bi = (channel == 0) ? lfoL_bipolar : lfoR_bipolar;

                switch (mode)
                {
                    case Mode::Chorus:
                    case Mode::Flanger:
                    case Mode::Vibrato:
                    case Mode::FreqMod:
                    {
                        // Модуляция времени задержки
                        float currentDelay = baseDelaySamples + (lfo_bi * depth * maxDeviationSamples);
                        currentDelay = juce::jmax(1.0f, currentDelay);
                        
                        delayLine.setDelay(currentDelay);
                        float delayed = delayLine.processSample(channel, wet);
                        
                        // Feedback актуален в основном для Flanger
                        float fb = (mode == Mode::Flanger) ? feedback : 0.0f;
                        delayLine.pushSample(channel, wet + (delayed * fb));
                        
                        wet = delayed;
                        break;
                    }

                    case Mode::Phaser:
                    {
                        // Модуляция частоты среза Allpass-фильтров
                        // Базовая частота 200 Гц, девиация до 5000 Гц
                        float cutoff = 200.0f + (lfo * depth * 4800.0f);
                        float g = std::tan(juce::MathConstants<float>::pi * cutoff / static_cast<float>(currentSampleRate));
                        g = (g - 1.0f) / (g + 1.0f); // Коэффициент allpass

                        float phaserOut = wet;
                        for (auto& filter : allpassFilters)
                        {
                            phaserOut = filter.processSample(phaserOut, g);
                        }
                        
                        wet = phaserOut + (wet * feedback); // Feedback для фазера
                        break;
                    }

                    case Mode::Tremolo:
                    {
                        // Амплитудная модуляция
                        float tremoloDepth = (mode == Mode::Tremolo) ? depth : 0.0f;
                        wet = dry * (1.0f - tremoloDepth + (tremoloDepth * lfo));
                        break;
                    }

                    case Mode::RingMod:
                    {
                        // Кольцевая модуляция (умножение на биполярный LFO)
                        wet = dry * lfo_bi;
                        break;
                    }

                    case Mode::StereoMod:
                    {
                        // Динамическое изменение стерео-базы (Mid/Side манипуляция)
                        // Используем фиксированную небольшую задержку для одного канала + модуляция ширины
                        float side = (channel == 0) ? 1.0f : -1.0f;
                        float modWidth = 0.5f + (lfo_bi * depth * 0.5f); // Модуляция от 0 до 1
                        wet = (dry * (1.0f - modWidth)) + (dry * side * modWidth);
                        break;
                    }
                }

                // Dry/Wet Mix
                wet = (dry * (1.0f - mix)) + (wet * mix);

                // Защита от клиппинга (особенно актуально для Feedback и RingMod)
                wet = juce::jlimit(-1.0f, 1.0f, wet);

                buffer.setSample(channel, sample, wet);
            }
        }
    }

    // Геттеры для инициализации значений без щелчков
    juce::LinearSmoothedValue<float>& getRateSmoothed() { return rateSmoothed; }
    juce::LinearSmoothedValue<float>& getDepthSmoothed() { return depthSmoothed; }
    juce::LinearSmoothedValue<float>& getMixSmoothed() { return mixSmoothed; }
    juce::LinearSmoothedValue<float>& getFeedbackSmoothed() { return feedbackSmoothed; }
    juce::LinearSmoothedValue<float>& getWidthSmoothed() { return widthSmoothed; }

private:
    double currentSampleRate = 44100.0;

    juce::LinearSmoothedValue<float> rateSmoothed;
    juce::LinearSmoothedValue<float> depthSmoothed;
    juce::LinearSmoothedValue<float> mixSmoothed;
    juce::LinearSmoothedValue<float> feedbackSmoothed;
    juce::LinearSmoothedValue<float> widthSmoothed;

    juce::dsp::Oscillator<float> lfoLeft;
    juce::dsp::Oscillator<float> lfoRight;
    
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;
    juce::AudioBuffer<float> delayBuffer;

    // Простой Allpass-фильтр 1-го порядка для Phaser
    struct AllpassFilter
    {
        float z1 = 0.0f;
        void prepare(double) { z1 = 0.0f; }
        float processSample(float in, float g)
        {
            float out = -g * in + z1;
            z1 = in + g * out;
            return out;
        }
    };
    std::vector<AllpassFilter> allpassFilters;
};
