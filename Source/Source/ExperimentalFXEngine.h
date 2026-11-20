#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class ExperimentalFXEngine
{
public:
    enum class Mode
    {
        Bitcrusher = 0,
        SampleRateReduction,
        Downsampling,
        NoiseGenerator,
        MetallicResonator,
        FrequencyShifter,
        IndustrialFeedback,
        GlitchProcessor,
        SpectralDestruction,
        MultibandDistortion
    };

    ExperimentalFXEngine() = default;

    void prepare(double sampleRate, int numChannels)
    {
        currentSampleRate = sampleRate;
        const double smoothTime = 0.02;

        intensitySmoothed.reset(sampleRate, smoothTime);
        mixSmoothed.reset(sampleRate, smoothTime);
        param1Smoothed.reset(sampleRate, smoothTime);
        param2Smoothed.reset(sampleRate, smoothTime);
        param3Smoothed.reset(sampleRate, smoothTime);

        // Noise generator state
        noisePhase = 0.0;

        // Metallic resonator: 8 резонансных фильтров
        resonatorFilters.resize(8);
        for (auto& f : resonatorFilters)
        {
            f.prepare(sampleRate);
        }

        // Multiband: 3 полосы (low, mid, high)
        lowBandBuffer.setSize(numChannels, 1024);
        midBandBuffer.setSize(numChannels, 1024);
        highBandBuffer.setSize(numChannels, 1024);
        
        // Crossover filters (2nd order Linkwitz-Riley)
        for (auto& f : crossoverLP) { f.prepare(sampleRate); }
        for (auto& f : crossoverHP) { f.prepare(sampleRate); }

        // Glitch buffer
        glitchBufferSize = static_cast<int>(sampleRate * 0.5); // 500ms
        glitchBuffer.setSize(numChannels, glitchBufferSize);
        glitchBuffer.clear();
        glitchWriteIndex = 0;

        // Industrial feedback buffer
        feedbackBufferSize = static_cast<int>(sampleRate * 2.0); // 2s
        feedbackBuffer.setSize(numChannels, feedbackBufferSize);
        feedbackBuffer.clear();
        feedbackReadIndex = 0;
        feedbackWriteIndex = 0;
    }

    void setTargetValues(float intensity, float mix, float param1, float param2, float param3)
    {
        intensitySmoothed.setTargetValue(intensity);  // 0.0 ... 1.0
        mixSmoothed.setTargetValue(mix);              // 0.0 ... 1.0
        param1Smoothed.setTargetValue(param1);        // Специфичный для режима
        param2Smoothed.setTargetValue(param2);
        param3Smoothed.setTargetValue(param3);
    }

    void process(juce::AudioBuffer<float>& buffer, Mode mode)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float intensity = intensitySmoothed.getNextValue();
            const float mix = mixSmoothed.getNextValue();
            const float p1 = param1Smoothed.getNextValue();
            const float p2 = param2Smoothed.getNextValue();
            const float p3 = param3Smoothed.getNextValue();

            float dryL = buffer.getSample(0, sample);
            float dryR = (numChannels > 1) ? buffer.getSample(1, sample) : dryL;
            float wetL = dryL;
            float wetR = dryR;

            switch (mode)
            {
                case Mode::Bitcrusher:
                {
                    // Квантование амплитуды: от 2 до 256 уровней
                    const float levels = juce::jmap(intensity, 2.0f, 256.0f);
                    wetL = std::round(dryL * levels) / levels;
                    wetR = std::round(dryR * levels) / levels;
                    break;
                }

                case Mode::SampleRateReduction:
                {
                    // Sample & Hold: удержание значения на N сэмплов
                    const float holdSamples = juce::jmap(intensity, 1.0f, 100.0f);
                    
                    if (sampleRateReductionCounter >= holdSamples)
                    {
                        sampleRateReductionHoldL = dryL;
                        sampleRateReductionHoldR = dryR;
                        sampleRateReductionCounter = 0.0f;
                    }
                    sampleRateReductionCounter += 1.0f;
                    
                    wetL = sampleRateReductionHoldL;
                    wetR = sampleRateReductionHoldR;
                    break;
                }

                case Mode::Downsampling:
                {
                    // Децимация с линейной интерполяцией
                    const float decimationRatio = juce::jmap(intensity, 1.0f, 20.0f);
                    
                    if (downsamplingCounter >= decimationRatio)
                    {
                        downsamplingPrevL = downsamplingCurrL;
                        downsamplingPrevR = downsamplingCurrR;
                        downsamplingCurrL = dryL;
                        downsamplingCurrR = dryR;
                        downsamplingCounter = 0.0f;
                    }
                    
                    float t = downsamplingCounter / decimationRatio;
                    wetL = downsamplingPrevL + (downsamplingCurrL - downsamplingPrevL) * t;
                    wetR = downsamplingPrevR + (downsamplingCurrR - downsamplingPrevR) * t;
                    
                    downsamplingCounter += 1.0f;
                    break;
                }

                case Mode::NoiseGenerator:
                {
                    // Генератор шума с фильтрацией
                    const float noiseAmount = intensity;
                    const float filterCutoff = juce::jmap(p1, 200.0f, 10000.0f);
                    
                    // Белый шум
                    float noiseL = static_cast<float>((std::rand() % 2000 - 1000) / 1000.0);
                    float noiseR = static_cast<float>((std::rand() % 2000 - 1000) / 1000.0);
                    
                    // НЧ фильтр
                    const float alpha = juce::jlimit(0.0f, 1.0f, 
                        2.0f * juce::MathConstants<float>::pi * filterCutoff / static_cast<float>(currentSampleRate));
                    noiseFilterL += alpha * (noiseL - noiseFilterL);
                    noiseFilterR += alpha * (noiseR - noiseFilterR);
                    
                    wetL = dryL + noiseFilterL * noiseAmount;
                    wetR = dryR + noiseFilterR * noiseAmount;
                    break;
                }

                case Mode::MetallicResonator:
                {
                    // Банк резонансных фильтров (metallic sound)
                    const float baseFreq = juce::jmap(p1, 100.0f, 2000.0f);
                    const float resonance = juce::jmap(intensity, 0.5f, 0.99f);
                    
                    float resonatorOutL = 0.0f;
                    float resonatorOutR = 0.0f;
                    
                    for (int i = 0; i < 8; ++i)
                    {
                        float freq = baseFreq * (1.0f + i * 0.5f); // Гармоники
                        resonatorFilters[i].setFrequency(freq);
                        resonatorFilters[i].setResonance(resonance);
                        
                        resonatorOutL += resonatorFilters[i].processSample(dryL);
                        resonatorOutR += resonatorFilters[i].processSample(dryR);
                    }
                    
                    wetL = resonatorOutL * 0.2f;
                    wetR = resonatorOutR * 0.2f;
                    break;
                }

                case Mode::FrequencyShifter:
                {
                    // Сдвиг частоты через ring modulation
                    const float shiftHz = juce::jmap(intensity, -1000.0f, 1000.0f);
                    
                    freqShifterPhaseL += shiftHz / currentSampleRate;
                    freqShifterPhaseR += shiftHz / currentSampleRate;
                    
                    if (freqShifterPhaseL > 1.0) freqShifterPhaseL -= 1.0;
                    if (freqShifterPhaseL < 0.0) freqShifterPhaseL += 1.0;
                    if (freqShifterPhaseR > 1.0) freqShifterPhaseR -= 1.0;
                    if (freqShifterPhaseR < 0.0) freqShifterPhaseR += 1.0;
                    
                    float oscL = std::sin(freqShifterPhaseL * 2.0f * juce::MathConstants<float>::pi);
                    float oscR = std::sin(freqShifterPhaseR * 2.0f * juce::MathConstants<float>::pi);
                    
                    wetL = dryL * oscL;
                    wetR = dryR * oscR;
                    break;
                }

                case Mode::IndustrialFeedback:
                {
                    // Агрессивный feedback с дисторшном
                    const float feedbackDelay = juce::jmap(p1, 0.001f, 0.5f); // 1ms - 500ms
                    const float feedbackAmount = intensity * 0.9f; // Макс 0.9 для стабильности
                    
                    int delaySamples = static_cast<int>(feedbackDelay * currentSampleRate);
                    delaySamples = juce::jlimit(1, feedbackBufferSize - 1, delaySamples);
                    
                    feedbackReadIndex = (feedbackWriteIndex - delaySamples + feedbackBufferSize) % feedbackBufferSize;
                    
                    float fbL = feedbackBuffer.getSample(0, feedbackReadIndex);
                    float fbR = feedbackBuffer.getSample(1, feedbackReadIndex);
                    
                    // Дисторшн в feedback path
                    fbL = std::tanh(fbL * 2.0f);
                    fbR = std::tanh(fbR * 2.0f);
                    
                    wetL = dryL + fbL * feedbackAmount;
                    wetR = dryR + fbR * feedbackAmount;
                    
                    feedbackBuffer.setSample(0, feedbackWriteIndex, wetL);
                    feedbackBuffer.setSample(1, feedbackWriteIndex, wetR);
                    feedbackWriteIndex = (feedbackWriteIndex + 1) % feedbackBufferSize;
                    break;
                }

                case Mode::GlitchProcessor:
                {
                    // Стохастическая манипуляция буфером
                    const float glitchProbability = intensity * 0.1f; // Шанс на сэмпл
                    const float glitchLength = juce::jmap(p1, 0.001f, 0.1f); // 1ms - 100ms
                    
                    if (std::rand() / static_cast<float>(RAND_MAX) < glitchProbability)
                    {
                        glitchActive = true;
                        glitchRemainingSamples = static_cast<int>(glitchLength * currentSampleRate);
                    }
                    
                    if (glitchActive)
                    {
                        int readIndex = (glitchWriteIndex - glitchRemainingSamples + glitchBufferSize) % glitchBufferSize;
                        wetL = glitchBuffer.getSample(0, readIndex);
                        wetR = glitchBuffer.getSample(1, readIndex);
                        glitchRemainingSamples--;
                        
                        if (glitchRemainingSamples <= 0)
                            glitchActive = false;
                    }
                    else
                    {
                        wetL = dryL;
                        wetR = dryR;
                    }
                    
                    glitchBuffer.setSample(0, glitchWriteIndex, dryL);
                    glitchBuffer.setSample(1, glitchWriteIndex, dryR);
                    glitchWriteIndex = (glitchWriteIndex + 1) % glitchBufferSize;
                    break;
                }

                case Mode::SpectralDestruction:
                {
                    // Упрощенная спектральная деструкция через formant filtering
                    const float formantFreq = juce::jmap(intensity, 200.0f, 5000.0f);
                    const float q = juce::jmap(p1, 1.0f, 20.0f);
                    
                    formantFilterL.setFrequency(formantFreq);
                    formantFilterL.setResonance(q);
                    formantFilterR.setFrequency(formantFreq);
                    formantFilterR.setResonance(q);
                    
                    wetL = formantFilterL.processSample(dryL);
                    wetR = formantFilterR.processSample(dryR);
                    
                    // Добавляем дисторшн для "разрушения"
                    wetL = std::tanh(wetL * 3.0f);
                    wetR = std::tanh(wetR * 3.0f);
                    break;
                }

                case Mode::MultibandDistortion:
                {
                    // Разделение на 3 полосы + дисторшн в каждой
                    const float lowCutoff = juce::jmap(p1, 100.0f, 500.0f);
                    const float highCutoff = juce::jmap(p2, 2000.0f, 8000.0f);
                    const float distortionAmount = intensity * 10.0f;
                    
                    // Crossover
                    crossoverLP[0].setFrequency(lowCutoff);
                    crossoverLP[1].setFrequency(lowCutoff);
                    crossoverHP[0].setFrequency(highCutoff);
                    crossoverHP[1].setFrequency(highCutoff);
                    
                    float lowL = crossoverLP[0].processSample(dryL);
                    float midL = crossoverHP[0].processSample(crossoverLP[0].processSample(dryL));
                    float highL = crossoverHP[0].processSample(dryL);
                    
                    float lowR = crossoverLP[1].processSample(dryR);
                    float midR = crossoverHP[1].processSample(crossoverLP[1].processSample(dryR));
                    float highR = crossoverHP[1].processSample(dryR);
                    
                    // Distortion per band
                    lowL = std::tanh(lowL * distortionAmount * 0.5f);
                    midL = std::tanh(midL * distortionAmount);
                    highL = std::tanh(highL * distortionAmount * 2.0f);
                    
                    lowR = std::tanh(lowR * distortionAmount * 0.5f);
                    midR = std::tanh(midR * distortionAmount);
                    highR = std::tanh(highR * distortionAmount * 2.0f);
                    
                    wetL = lowL + midL + highL;
                    wetR = lowR + midR + highR;
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

    juce::LinearSmoothedValue<float>& getIntensitySmoothed() { return intensitySmoothed; }
    juce::LinearSmoothedValue<float>& getMixSmoothed() { return mixSmoothed; }
    juce::LinearSmoothedValue<float>& getParam1Smoothed() { return param1Smoothed; }
    juce::LinearSmoothedValue<float>& getParam2Smoothed() { return param2Smoothed; }
    juce::LinearSmoothedValue<float>& getParam3Smoothed() { return param3Smoothed; }

private:
    double currentSampleRate = 44100.0;

    juce::LinearSmoothedValue<float> intensitySmoothed;
    juce::LinearSmoothedValue<float> mixSmoothed;
    juce::LinearSmoothedValue<float> param1Smoothed;
    juce::LinearSmoothedValue<float> param2Smoothed;
    juce::LinearSmoothedValue<float> param3Smoothed;

    // Sample Rate Reduction
    float sampleRateReductionCounter = 0.0f;
    float sampleRateReductionHoldL = 0.0f;
    float sampleRateReductionHoldR = 0.0f;

    // Downsampling
    float downsamplingCounter = 0.0f;
    float downsamplingPrevL = 0.0f;
    float downsamplingPrevR = 0.0f;
    float downsamplingCurrL = 0.0f;
    float downsamplingCurrR = 0.0f;

    // Noise Generator
    double noisePhase = 0.0;
    float noiseFilterL = 0.0f;
    float noiseFilterR = 0.0f;

    // Metallic Resonator
    struct ResonantFilter
    {
        float y1 = 0.0f, y2 = 0.0f;
        float freq = 1000.0f;
        float res = 0.5f;
        double sampleRate = 44100.0;

        void prepare(double sr) { sampleRate = sr; y1 = y2 = 0.0f; }
        void setFrequency(float f) { freq = f; }
        void setResonance(float r) { res = r; }

        float processSample(float in)
        {
            float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sampleRate);
            float q = 1.0f - res;
            float out = y1 * 2.0f * std::cos(w0) - y2 * q + in * (1.0f - q);
            y2 = y1;
            y1 = out;
            return out;
        }
    };
    std::vector<ResonantFilter> resonatorFilters;

    // Frequency Shifter
    double freqShifterPhaseL = 0.0;
    double freqShifterPhaseR = 0.0;

    // Industrial Feedback
    juce::AudioBuffer<float> feedbackBuffer;
    int feedbackBufferSize = 0;
    int feedbackReadIndex = 0;
    int feedbackWriteIndex = 0;

    // Glitch Processor
    juce::AudioBuffer<float> glitchBuffer;
    int glitchBufferSize = 0;
    int glitchWriteIndex = 0;
    bool glitchActive = false;
    int glitchRemainingSamples = 0;

    // Spectral Destruction
    struct StateVariableFilter
    {
        float y1 = 0.0f, y2 = 0.0f;
        float freq = 1000.0f;
        float res = 1.0f;
        double sampleRate = 44100.0;

        void prepare(double sr) { sampleRate = sr; y1 = y2 = 0.0f; }
        void setFrequency(float f) { freq = f; }
        void setResonance(float r) { res = r; }

        float processSample(float in)
        {
            float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sampleRate);
            float q = 1.0f / res;
            float hp = in - y1 * q - y2;
            float bp = hp * w0 + y1;
            float lp = bp * w0 + y2;
            y1 = bp;
            y2 = lp;
            return lp; // Bandpass для спектрального эффекта
        }
    };
    StateVariableFilter formantFilterL;
    StateVariableFilter formantFilterR;

    // Multiband Distortion
    struct SimpleCrossover
    {
        float y1 = 0.0f;
        float freq = 1000.0f;
        double sampleRate = 44100.0;

        void prepare(double sr) { sampleRate = sr; y1 = 0.0f; }
        void setFrequency(float f) { freq = f; }

        float processSample(float in)
        {
            float alpha = juce::jlimit(0.0f, 1.0f, 
                2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(sampleRate));
            y1 += alpha * (in - y1);
            return y1; // LP output
        }
    };
    SimpleCrossover crossoverLP[2];
    SimpleCrossover crossoverHP[2];
};
