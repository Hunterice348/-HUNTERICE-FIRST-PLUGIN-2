#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class ModulationEngine
{
public:
    enum class Mode
    {
        Chorus = 0, Flanger, Phaser, Tremolo,
        Vibrato, RingMod, FreqMod, StereoMod
    };

    ModulationEngine() = default;

    void prepare(double sampleRate, int numChannels)
    {
        currentSampleRate = sampleRate;
        const double smoothTime = 0.02;

        rateSmoothed.reset(sampleRate, smoothTime);
        depthSmoothed.reset(sampleRate, smoothTime);
        mixSmoothed.reset(sampleRate, smoothTime);
        feedbackSmoothed.reset(sampleRate, smoothTime);
        widthSmoothed.reset(sampleRate, smoothTime);

        // ИСПРАВЛЕНИЕ: Кастомные LFO вместо juce::dsp::Oscillator
        lfoPhaseL = 0.0;
        lfoPhaseR = 0.0;

        // Линия задержки
        const int maxDelaySamples = static_cast<int>(sampleRate * 0.05) + 100;
        delayLine.prepare({ sampleRate, static_cast<juce::uint32>(numChannels), static_cast<juce::uint32>(maxDelaySamples) });
        delayLine.reset();

        // Allpass для Phaser
        allpassFilters.resize(4);
        for (auto& filter : allpassFilters)
            filter.prepare(sampleRate);
    }

    void setTargetValues(float rate, float depth, float mix, float feedback, float width)
    {
        rateSmoothed.setTargetValue(rate);
        depthSmoothed.setTargetValue(depth);
        mixSmoothed.setTargetValue(mix);
        feedbackSmoothed.setTargetValue(feedback);
        widthSmoothed.setTargetValue(width);
    }

    void process(juce::AudioBuffer<float>& buffer, Mode mode)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        const float baseDelayMs = (mode == Mode::Flanger) ? 2.0f :
                                  (mode == Mode::Vibrato || mode == Mode::FreqMod) ? 5.0f : 15.0f;
        const float baseDelaySamples = static_cast<float>(currentSampleRate * baseDelayMs / 1000.0f);
        const float maxDeviationSamples = (mode == Mode::Flanger) ? 3.0f : 10.0f;

        for (int sample = 0; sample < numSamples; ++sample)
        {
            const float rate     = rateSmoothed.getNextValue();
            const float depth    = depthSmoothed.getNextValue();
            const float mix      = mixSmoothed.getNextValue();
            const float feedback = feedbackSmoothed.getNextValue();
            const float width    = widthSmoothed.getNextValue();

            // ИСПРАВЛЕНИЕ: Правильная генерация LFO с фазовым сдвигом
            // Обновляем фазу LFO
            double phaseIncrement = rate / currentSampleRate;
            lfoPhaseL += phaseIncrement;
            lfoPhaseR += phaseIncrement;
            
            if (lfoPhaseL >= 1.0) lfoPhaseL -= 1.0;
            if (lfoPhaseR >= 1.0) lfoPhaseR -= 1.0;
            
            // Width управляет фазовым сдвигом между каналами
            const float phaseOffset = width * static_cast<float>(juce::MathConstants<double>::pi);
            
            // Униполярный LFO (0.0 ... 1.0)
            float lfoL = static_cast<float>(std::sin(lfoPhaseL * 2.0 * juce::MathConstants<double>::pi)) * 0.5f + 0.5f;
            float lfoR = static_cast<float>(std::sin(lfoPhaseR * 2.0 * juce::MathConstants<double>::pi + phaseOffset)) * 0.5f + 0.5f;
            
            // Биполярный LFO (-1.0 ... 1.0)
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
                        float currentDelay = baseDelaySamples + (lfo_bi * depth * maxDeviationSamples);
                        currentDelay = juce::jmax(1.0f, currentDelay);
                        
                        delayLine.setDelay(currentDelay);
                        float delayed = delayLine.processSample(channel, wet);
                        
                        float fb = (mode == Mode::Flanger) ? feedback : 0.0f;
                        delayLine.pushSample(channel, wet + (delayed * fb));
                        
                        wet = (mode == Mode::Vibrato) ? delayed : delayed;
                        break;
                    }

                    case Mode::Phaser:
                    {
                        float cutoff = 200.0f + (lfo * depth * 4800.0f);
                        float g = std::tan(juce::MathConstants<float>::pi * cutoff / static_cast<float>(currentSampleRate));
                        g = (g - 1.0f) / (g + 1.0f);

                        float phaserOut = wet;
                        for (auto& filter : allpassFilters)
                            phaserOut = filter.processSample(phaserOut, g);
                        
                        wet = phaserOut + (wet * feedback);
                        break;
                    }

                    case Mode::Tremolo:
                    {
                        wet = dry * (1.0f - depth + (depth * lfo));
                        break;
                    }

                    case Mode::RingMod:
                    {
                        wet = dry * lfo_bi;
                        break;
                    }

                    case Mode::StereoMod:
                    {
                        float side = (channel == 0) ? 1.0f : -1.0f;
                        float modWidth = 0.5f + (lfo_bi * depth * 0.5f);
                        wet = (dry * (1.0f - modWidth)) + (dry * side * modWidth);
                        break;
                    }
                }

                wet = (dry * (1.0f - mix)) + (wet * mix);
                wet = juce::jlimit(-1.0f, 1.0f, wet);
                buffer.setSample(channel, sample, wet);
            }
        }
    }

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

    // ИСПРАВЛЕНИЕ: Кастомные LFO (вместо juce::dsp::Oscillator)
    double lfoPhaseL = 0.0;
    double lfoPhaseR = 0.0;
    
    juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear> delayLine;

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
