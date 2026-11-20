#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class DynamicsAndFiltersEngine
{
public:
    enum class Mode
    {
        Compressor = 0,
        Limiter,
        Gate,
        TransientShaper,
        Equalizer,
        LowPass,
        HighPass,
        BandPass,
        NotchFilter,
        ResonantFilter
    };

    DynamicsAndFiltersEngine() = default;

    void prepare(double sampleRate, int numChannels)
    {
        currentSampleRate = sampleRate;
        const double smoothTime = 0.02;

        thresholdSmoothed.reset(sampleRate, smoothTime);
        ratioSmoothed.reset(sampleRate, smoothTime);
        attackSmoothed.reset(sampleRate, smoothTime);
        releaseSmoothed.reset(sampleRate, smoothTime);
        makeupSmoothed.reset(sampleRate, smoothTime);
        frequencySmoothed.reset(sampleRate, smoothTime);
        qSmoothed.reset(sampleRate, smoothTime);
        gainSmoothed.reset(sampleRate, smoothTime);
        mixSmoothed.reset(sampleRate, smoothTime);

        // Инициализация JUCE DSP модулей
        compressor.prepare({ sampleRate, static_cast<juce::uint32>(numChannels), 512 });
        limiter.prepare({ sampleRate, static_cast<juce::uint32>(numChannels), 512 });
        gate.prepare({ sampleRate, static_cast<juce::uint32>(numChannels), 512 });

        // Инициализация биквадратных фильтров для EQ и фильтров
        biquadFiltersL.resize(5);
        biquadFiltersR.resize(5);
        for (auto& f : biquadFiltersL) { f.prepare(sampleRate); }
        for (auto& f : biquadFiltersR) { f.prepare(sampleRate); }

        // State Variable Filter для резонансного фильтра
        svfL.prepare({ sampleRate, 1, 512 });
        svfR.prepare({ sampleRate, 1, 512 });

        // Envelope follower для Transient Shaper
        envelopeL = 0.0f;
        envelopeR = 0.0f;
    }

    void setTargetValues(float threshold, float ratio, float attack, float release, 
                         float makeup, float frequency, float q, float gain, float mix)
    {
        thresholdSmoothed.setTargetValue(threshold);  // dB для compressor/gate/limiter, Hz для filters
        ratioSmoothed.setTargetValue(ratio);          // ratio для compressor, Q для filters
        attackSmoothed.setTargetValue(attack);        // ms
        releaseSmoothed.setTargetValue(release);      // ms
        makeupSmoothed.setTargetValue(makeup);        // dB
        frequencySmoothed.setTargetValue(frequency);  // Hz
        qSmoothed.setTargetValue(q);                  // Q factor
        gainSmoothed.setTargetValue(gain);            // dB для EQ
        mixSmoothed.setTargetValue(mix);              // 0.0 ... 1.0
    }

    void process(juce::AudioBuffer<float>& buffer, Mode mode)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float threshold = thresholdSmoothed.getNextValue();
            const float ratio = ratioSmoothed.getNextValue();
            const float attack = attackSmoothed.getNextValue();
            const float release = releaseSmoothed.getNextValue();
            const float makeup = makeupSmoothed.getNextValue();
            const float frequency = frequencySmoothed.getNextValue();
            const float q = qSmoothed.getNextValue();
            const float gain = gainSmoothed.getNextValue();
            const float mix = mixSmoothed.getNextValue();

            float dryL = buffer.getSample(0, sample);
            float dryR = (numChannels > 1) ? buffer.getSample(1, sample) : dryL;
            float wetL = dryL;
            float wetR = dryR;

            switch (mode)
            {
                case Mode::Compressor:
                {
                    // Настройка компрессора
                    compressor.setThreshold(threshold);
                    compressor.setRatio(ratio);
                    compressor.setAttack(attack);
                    compressor.setRelease(release);
                    
                    juce::dsp::AudioBlock<float> block(buffer);
                    compressor.process(juce::dsp::ProcessContextReplacing<float>(block));
                    
                    // Makeup gain
                    float makeupGain = juce::Decibels::decibelsToGain(makeup, -60.0f);
                    wetL = dryL * makeupGain;
                    wetR = dryR * makeupGain;
                    break;
                }

                case Mode::Limiter:
                {
                    // Brickwall limiter
                    limiter.setThreshold(threshold);
                    limiter.setRelease(release);
                    
                    juce::dsp::AudioBlock<float> block(buffer);
                    limiter.process(juce::dsp::ProcessContextReplacing<float>(block));
                    
                    wetL = dryL;
                    wetR = dryR;
                    break;
                }

                case Mode::Gate:
                {
                    // Noise gate
                    gate.setThreshold(threshold);
                    gate.setAttack(attack);
                    gate.setRelease(release);
                    gate.setRatio(ratio); // Ratio для gate = expansion ratio
                    
                    juce::dsp::AudioBlock<float> block(buffer);
                    gate.process(juce::dsp::ProcessContextReplacing<float>(block));
                    
                    wetL = dryL;
                    wetR = dryR;
                    break;
                }

                case Mode::TransientShaper:
                {
                    // Envelope follower для управления атакой и сустейном
                    const float attackCoeff = std::exp(-1.0f / (attack * 0.001f * currentSampleRate));
                    const float releaseCoeff = std::exp(-1.0f / (release * 0.001f * currentSampleRate));
                    
                    float envL = std::abs(dryL);
                    float envR = std::abs(dryR);
                    
                    if (envL > envelopeL)
                        envelopeL = attackCoeff * envelopeL + (1.0f - attackCoeff) * envL;
                    else
                        envelopeL = releaseCoeff * envelopeL + (1.0f - releaseCoeff) * envL;
                    
                    if (envR > envelopeR)
                        envelopeR = attackCoeff * envelopeR + (1.0f - attackCoeff) * envR;
                    else
                        envelopeR = releaseCoeff * envelopeR + (1.0f - releaseCoeff) * envR;
                    
                    // Attack control (threshold): усиление быстрых транзиентов
                    float attackGain = juce::jmap(threshold, -20.0f, 20.0f); // dB
                    float attackGainLinear = juce::Decibels::decibelsToGain(attackGain, -60.0f);
                    
                    // Sustain control (ratio): усиление/ослабление сустейна
                    float sustainGain = juce::jmap(ratio, 0.0f, 2.0f); // множитель
                    
                    // Комбинирование: быстрые транзиенты * attackGain + медленный сустейн * sustainGain
                    float transientL = (envelopeL > 0.01f) ? (dryL / envelopeL) : dryL;
                    float transientR = (envelopeR > 0.01f) ? (dryR / envelopeR) : dryR;
                    
                    wetL = transientL * attackGainLinear + dryL * (1.0f - attackGainLinear) * sustainGain;
                    wetR = transientR * attackGainLinear + dryR * (1.0f - attackGainLinear) * sustainGain;
                    break;
                }

                case Mode::Equalizer:
                {
                    // Параметрический EQ (peaking filter)
                    float freq = juce::jlimit(20.0f, 20000.0f, frequency);
                    float qFactor = juce::jlimit(0.1f, 10.0f, q);
                    float gainDb = juce::jlimit(-24.0f, 24.0f, gain);
                    
                    // Расчет коэффициентов биквадратного фильтра (peaking EQ)
                    float A = std::pow(10.0f, gainDb / 40.0f);
                    float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(currentSampleRate);
                    float alpha = std::sin(w0) / (2.0f * qFactor);
                    
                    float b0 = 1.0f + alpha * A;
                    float b1 = -2.0f * std::cos(w0);
                    float b2 = 1.0f - alpha * A;
                    float a0 = 1.0f + alpha / A;
                    float a1 = -2.0f * std::cos(w0);
                    float a2 = 1.0f - alpha / A;
                    
                    // Нормализация
                    b0 /= a0; b1 /= a0; b2 /= a0;
                    a1 /= a0; a2 /= a0;
                    
                    // Применение фильтра
                    wetL = biquadFiltersL[0].processSample(dryL, b0, b1, b2, a1, a2);
                    wetR = biquadFiltersR[0].processSample(dryR, b0, b1, b2, a1, a2);
                    break;
                }

                case Mode::LowPass:
                {
                    // Биквадратный Low-Pass фильтр
                    float freq = juce::jlimit(20.0f, 20000.0f, frequency);
                    float qFactor = juce::jlimit(0.1f, 10.0f, q);
                    
                    float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(currentSampleRate);
                    float alpha = std::sin(w0) / (2.0f * qFactor);
                    float cosw0 = std::cos(w0);
                    
                    float b0 = (1.0f - cosw0) / 2.0f;
                    float b1 = 1.0f - cosw0;
                    float b2 = (1.0f - cosw0) / 2.0f;
                    float a0 = 1.0f + alpha;
                    float a1 = -2.0f * cosw0;
                    float a2 = 1.0f - alpha;
                    
                    b0 /= a0; b1 /= a0; b2 /= a0;
                    a1 /= a0; a2 /= a0;
                    
                    wetL = biquadFiltersL[1].processSample(dryL, b0, b1, b2, a1, a2);
                    wetR = biquadFiltersR[1].processSample(dryR, b0, b1, b2, a1, a2);
                    break;
                }

                case Mode::HighPass:
                {
                    // Биквадратный High-Pass фильтр
                    float freq = juce::jlimit(20.0f, 20000.0f, frequency);
                    float qFactor = juce::jlimit(0.1f, 10.0f, q);
                    
                    float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(currentSampleRate);
                    float alpha = std::sin(w0) / (2.0f * qFactor);
                    float cosw0 = std::cos(w0);
                    
                    float b0 = (1.0f + cosw0) / 2.0f;
                    float b1 = -(1.0f + cosw0);
                    float b2 = (1.0f + cosw0) / 2.0f;
                    float a0 = 1.0f + alpha;
                    float a1 = -2.0f * cosw0;
                    float a2 = 1.0f - alpha;
                    
                    b0 /= a0; b1 /= a0; b2 /= a0;
                    a1 /= a0; a2 /= a0;
                    
                    wetL = biquadFiltersL[2].processSample(dryL, b0, b1, b2, a1, a2);
                    wetR = biquadFiltersR[2].processSample(dryR, b0, b1, b2, a1, a2);
                    break;
                }

                case Mode::BandPass:
                {
                    // Биквадратный Band-Pass фильтр
                    float freq = juce::jlimit(20.0f, 20000.0f, frequency);
                    float qFactor = juce::jlimit(0.1f, 10.0f, q);
                    
                    float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(currentSampleRate);
                    float alpha = std::sin(w0) / (2.0f * qFactor);
                    float cosw0 = std::cos(w0);
                    
                    float b0 = alpha;
                    float b1 = 0.0f;
                    float b2 = -alpha;
                    float a0 = 1.0f + alpha;
                    float a1 = -2.0f * cosw0;
                    float a2 = 1.0f - alpha;
                    
                    b0 /= a0; b1 /= a0; b2 /= a0;
                    a1 /= a0; a2 /= a0;
                    
                    wetL = biquadFiltersL[3].processSample(dryL, b0, b1, b2, a1, a2);
                    wetR = biquadFiltersR[3].processSample(dryR, b0, b1, b2, a1, a2);
                    break;
                }

                case Mode::NotchFilter:
                {
                    // Биквадратный Notch (band-reject) фильтр
                    float freq = juce::jlimit(20.0f, 20000.0f, frequency);
                    float qFactor = juce::jlimit(0.1f, 10.0f, q);
                    
                    float w0 = 2.0f * juce::MathConstants<float>::pi * freq / static_cast<float>(currentSampleRate);
                    float alpha = std::sin(w0) / (2.0f * qFactor);
                    float cosw0 = std::cos(w0);
                    
                    float b0 = 1.0f;
                    float b1 = -2.0f * cosw0;
                    float b2 = 1.0f;
                    float a0 = 1.0f + alpha;
                    float a1 = -2.0f * cosw0;
                    float a2 = 1.0f - alpha;
                    
                    b0 /= a0; b1 /= a0; b2 /= a0;
                    a1 /= a0; a2 /= a0;
                    
                    wetL = biquadFiltersL[4].processSample(dryL, b0, b1, b2, a1, a2);
                    wetR = biquadFiltersR[4].processSample(dryR, b0, b1, b2, a1, a2);
                    break;
                }

                case Mode::ResonantFilter:
                {
                    // State Variable Filter с резонансом
                    float freq = juce::jlimit(20.0f, 20000.0f, frequency);
                    float resonance = juce::jlimit(0.0f, 1.0f, q);
                    
                    svfL.setFrequency(freq);
                    svfL.setResonance(resonance);
                    svfR.setFrequency(freq);
                    svfR.setResonance(resonance);
                    
                    juce::dsp::AudioBlock<float> block(buffer);
                    svfL.process(juce::dsp::ProcessContextReplacing<float>(block));
                    
                    wetL = dryL;
                    wetR = dryR;
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

    juce::LinearSmoothedValue<float>& getThresholdSmoothed() { return thresholdSmoothed; }
    juce::LinearSmoothedValue<float>& getRatioSmoothed() { return ratioSmoothed; }
    juce::LinearSmoothedValue<float>& getAttackSmoothed() { return attackSmoothed; }
    juce::LinearSmoothedValue<float>& getReleaseSmoothed() { return releaseSmoothed; }
    juce::LinearSmoothedValue<float>& getMakeupSmoothed() { return makeupSmoothed; }
    juce::LinearSmoothedValue<float>& getFrequencySmoothed() { return frequencySmoothed; }
    juce::LinearSmoothedValue<float>& getQSmoothed() { return qSmoothed; }
    juce::LinearSmoothedValue<float>& getGainSmoothed() { return gainSmoothed; }
    juce::LinearSmoothedValue<float>& getMixSmoothed() { return mixSmoothed; }

private:
    double currentSampleRate = 44100.0;

    juce::LinearSmoothedValue<float> thresholdSmoothed;
    juce::LinearSmoothedValue<float> ratioSmoothed;
    juce::LinearSmoothedValue<float> attackSmoothed;
    juce::LinearSmoothedValue<float> releaseSmoothed;
    juce::LinearSmoothedValue<float> makeupSmoothed;
    juce::LinearSmoothedValue<float> frequencySmoothed;
    juce::LinearSmoothedValue<float> qSmoothed;
    juce::LinearSmoothedValue<float> gainSmoothed;
    juce::LinearSmoothedValue<float> mixSmoothed;

    // JUCE DSP модули
    juce::dsp::Compressor<float> compressor;
    juce::dsp::Limiter<float> limiter;
    juce::dsp::Gate<float> gate;

    // Биквадратные фильтры
    struct BiquadFilter
    {
        float x1 = 0.0f, x2 = 0.0f;
        float y1 = 0.0f, y2 = 0.0f;

        void prepare(double) { x1 = x2 = y1 = y2 = 0.0f; }

        float processSample(float in, float b0, float b1, float b2, float a1, float a2)
        {
            float out = b0 * in + b1 * x1 + b2 * x2 - a1 * y1 - a2 * y2;
            x2 = x1;
            x1 = in;
            y2 = y1;
            y1 = out;
            return out;
        }
    };
    std::vector<BiquadFilter> biquadFiltersL;
    std::vector<BiquadFilter> biquadFiltersR;

    // State Variable Filter
    juce::dsp::StateVariableTPTFilter<float> svfL;
    juce::dsp::StateVariableTPTFilter<float> svfR;

    // Transient Shaper envelope
    float envelopeL = 0.0f;
    float envelopeR = 0.0f;
};
