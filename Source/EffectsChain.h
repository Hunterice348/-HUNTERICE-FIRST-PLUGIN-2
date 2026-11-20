#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class EffectsChain
{
public:
    EffectsChain() = default;

    void prepare(double sampleRate, int numChannels, int samplesPerBlock)
    {
        currentSampleRate = sampleRate;
        currentNumChannels = numChannels;
        
        juce::dsp::ProcessSpec spec { sampleRate, static_cast<juce::uint32>(samplesPerBlock), static_cast<juce::uint32>(numChannels) };
        
        // Инициализация JUCE DSP модулей
        chorus.prepare(spec);
        chorus.setRate(1.0f);
        chorus.setDepth(0.3f);
        chorus.setMix(0.5f);
        chorus.setFeedback(0.2f);
        chorus.setCentreDelay(7.0f);
        
        phaser.prepare(spec);
        phaser.setRate(1.0f);
        phaser.setDepth(0.5f);
        phaser.setCentreFrequency(1000.0f);
        phaser.setFeedback(0.3f);
        phaser.setMix(0.5f);
        
        reverbParams.roomSize = 0.5f;
        reverbParams.damping = 0.5f;
        reverbParams.wetLevel = 0.33f;
        reverbParams.dryLevel = 0.7f;
        reverbParams.width = 1.0f;
        reverbParams.freezeMode = 0.0f;
        reverb.setParameters(reverbParams);
        
        // Delay line
        maxDelaySamples = static_cast<int>(sampleRate * 2.0); // 2 секунды максимум
        delayBuffer.setSize(numChannels, maxDelaySamples);
        delayBuffer.clear();
        delayWriteIndex = 0;
        
        // Сглаживание параметров
        smoothedDistDrive.reset(sampleRate, 0.02);
        smoothedDistMix.reset(sampleRate, 0.02);
        smoothedSatDrive.reset(sampleRate, 0.02);
        smoothedSatMix.reset(sampleRate, 0.02);
        smoothedDelayTime.reset(sampleRate, 0.02);
        smoothedDelayFeedback.reset(sampleRate, 0.02);
        smoothedDelayMix.reset(sampleRate, 0.02);
        smoothedReverbSize.reset(sampleRate, 0.02);
        smoothedReverbDamp.reset(sampleRate, 0.02);
        smoothedReverbMix.reset(sampleRate, 0.02);
        smoothedPhaserRate.reset(sampleRate, 0.02);
        smoothedPhaserDepth.reset(sampleRate, 0.02);
        smoothedPhaserMix.reset(sampleRate, 0.02);
        smoothedChorusRate.reset(sampleRate, 0.02);
        smoothedChorusDepth.reset(sampleRate, 0.02);
        smoothedChorusMix.reset(sampleRate, 0.02);
        smoothedWidth.reset(sampleRate, 0.02);
    }

    // Установка всех параметров эффектов
    void setParameters(
        bool distEnabled, float distDrive, float distMix,
        bool satEnabled, float satDrive, float satMix,
        bool chorusEnabled, float chorusRate, float chorusDepth, float chorusMix,
        bool delayEnabled, float delayTimeMs, float delayFeedback, float delayMix,
        bool reverbEnabled, float reverbSize, float reverbDamp, float reverbMix,
        bool phaserEnabled, float phaserRate, float phaserDepth, float phaserMix,
        float stereoWidth)
    {
        // Distortion
        this->distEnabled = distEnabled;
        smoothedDistDrive.setTargetValue(distDrive);
        smoothedDistMix.setTargetValue(distMix);
        
        // Saturation
        this->satEnabled = satEnabled;
        smoothedSatDrive.setTargetValue(satDrive);
        smoothedSatMix.setTargetValue(satMix);
        
        // Chorus
        this->chorusEnabled = chorusEnabled;
        smoothedChorusRate.setTargetValue(chorusRate);
        smoothedChorusDepth.setTargetValue(chorusDepth);
        smoothedChorusMix.setTargetValue(chorusMix);
        
        // Delay
        this->delayEnabled = delayEnabled;
        smoothedDelayTime.setTargetValue(delayTimeMs);
        smoothedDelayFeedback.setTargetValue(delayFeedback);
        smoothedDelayMix.setTargetValue(delayMix);
        
        // Reverb
        this->reverbEnabled = reverbEnabled;
        smoothedReverbSize.setTargetValue(reverbSize);
        smoothedReverbDamp.setTargetValue(reverbDamp);
        smoothedReverbMix.setTargetValue(reverbMix);
        
        // Phaser
        this->phaserEnabled = phaserEnabled;
        smoothedPhaserRate.setTargetValue(phaserRate);
        smoothedPhaserDepth.setTargetValue(phaserDepth);
        smoothedPhaserMix.setTargetValue(phaserMix);
        
        // Stereo Width
        smoothedWidth.setTargetValue(stereoWidth);
    }

    // Основная обработка — применяется ко всему миксу
    void process(juce::AudioBuffer<float>& buffer)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();
        
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float sL = buffer.getSample(0, sample);
            float sR = (numChannels > 1) ? buffer.getSample(1, sample) : sL;
            
            // 1. Distortion (hard clipping с tanh)
            if (distEnabled)
            {
                float drive = smoothedDistDrive.getNextValue();
                float mix = smoothedDistMix.getNextValue();
                
                float driveGain = 1.0f + drive * 19.0f; // 1x ... 20x
                float dryL = sL, dryR = sR;
                float wetL = std::tanh(sL * driveGain);
                float wetR = std::tanh(sR * driveGain);
                
                sL = dryL * (1.0f - mix) + wetL * mix;
                sR = dryR * (1.0f - mix) + wetR * mix;
            }
            
            // 2. Saturation (мягкое насыщение)
            if (satEnabled)
            {
                float drive = smoothedSatDrive.getNextValue();
                float mix = smoothedSatMix.getNextValue();
                
                float driveGain = 1.0f + drive * 3.0f; // 1x ... 4x (мягче чем distortion)
                float dryL = sL, dryR = sR;
                
                // Мягкое насыщение: tanh с предварительным усилением + DC offset correction
                float wetL = std::tanh(sL * driveGain);
                float wetR = std::tanh(sR * driveGain);
                
                // Добавляем чётные гармоники для "тёплого" звучания
                wetL = wetL * 0.9f + wetL * wetL * std::copysign(0.1f, wetL);
                wetR = wetR * 0.9f + wetR * wetR * std::copysign(0.1f, wetR);
                
                sL = dryL * (1.0f - mix) + wetL * mix;
                sR = dryR * (1.0f - mix) + wetR * mix;
            }
            
            buffer.setSample(0, sample, sL);
            if (numChannels > 1) buffer.setSample(1, sample, sR);
        }
        
        // 3. Phaser (block processing через JUCE DSP)
        if (phaserEnabled)
        {
            float rate = smoothedPhaserRate.getNextValue();
            float depth = smoothedPhaserDepth.getNextValue();
            float mix = smoothedPhaserMix.getNextValue();
            
            phaser.setRate(rate);
            phaser.setDepth(depth);
            phaser.setMix(mix);
            
            juce::dsp::AudioBlock<float> block(buffer);
            phaser.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
        
        // 4. Chorus (block processing через JUCE DSP)
        if (chorusEnabled)
        {
            float rate = smoothedChorusRate.getNextValue();
            float depth = smoothedChorusDepth.getNextValue();
            float mix = smoothedChorusMix.getNextValue();
            
            chorus.setRate(rate);
            chorus.setDepth(depth);
            chorus.setMix(mix);
            
            juce::dsp::AudioBlock<float> block(buffer);
            chorus.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
        
        // 5. Delay (sample-by-sample для точной модуляции)
        if (delayEnabled)
        {
            for (int sample = 0; sample < numSamples; ++sample)
            {
                float sL = buffer.getSample(0, sample);
                float sR = (numChannels > 1) ? buffer.getSample(1, sample) : sL;
                
                float delayTimeMs = smoothedDelayTime.getNextValue();
                float feedback = smoothedDelayFeedback.getNextValue();
                float mix = smoothedDelayMix.getNextValue();
                
                float delaySamples = static_cast<float>(currentSampleRate * delayTimeMs / 1000.0);
                delaySamples = juce::jlimit(1.0f, static_cast<float>(maxDelaySamples - 1), delaySamples);
                
                // Чтение из буфера задержки с линейной интерполяцией
                float readPos = static_cast<float>(delayWriteIndex) - delaySamples;
                if (readPos < 0.0f) readPos += static_cast<float>(maxDelaySamples);
                
                int readIdx0 = static_cast<int>(readPos) % maxDelaySamples;
                int readIdx1 = (readIdx0 + 1) % maxDelaySamples;
                float frac = readPos - std::floor(readPos);
                
                float delayedL = delayBuffer.getSample(0, readIdx0) * (1.0f - frac) + 
                                 delayBuffer.getSample(0, readIdx1) * frac;
                float delayedR = delayBuffer.getSample(1, readIdx0) * (1.0f - frac) + 
                                 delayBuffer.getSample(1, readIdx1) * frac;
                
                // Запись в буфер с feedback
                delayBuffer.setSample(0, delayWriteIndex, sL + delayedL * feedback);
                delayBuffer.setSample(1, delayWriteIndex, sR + delayedR * feedback);
                delayWriteIndex = (delayWriteIndex + 1) % maxDelaySamples;
                
                // Mix
                float outL = sL * (1.0f - mix) + delayedL * mix;
                float outR = sR * (1.0f - mix) + delayedR * mix;
                
                buffer.setSample(0, sample, outL);
                if (numChannels > 1) buffer.setSample(1, sample, outR);
            }
        }
        
        // 6. Reverb (block processing через JUCE DSP)
        if (reverbEnabled)
        {
            float size = smoothedReverbSize.getNextValue();
            float damp = smoothedReverbDamp.getNextValue();
            float mix = smoothedReverbMix.getNextValue();
            
            reverbParams.roomSize = size;
            reverbParams.damping = damp;
            reverbParams.wetLevel = mix;
            reverbParams.dryLevel = 1.0f - mix;
            reverbParams.width = 1.0f;
            reverb.setParameters(reverbParams);
            
            juce::dsp::AudioBlock<float> block(buffer);
            reverb.process(juce::dsp::ProcessContextReplacing<float>(block));
        }
        
        // 7. Stereo Width (Mid/Side processing)
        for (int sample = 0; sample < numSamples; ++sample)
        {
            float width = smoothedWidth.getNextValue();
            
            float sL = buffer.getSample(0, sample);
            float sR = (numChannels > 1) ? buffer.getSample(1, sample) : sL;
            
            // Mid/Side
            float mid = (sL + sR) * 0.5f;
            float side = (sL - sR) * 0.5f;
            
            // Применяем ширину к side компоненту
            side *= width;
            
            // Обратное преобразование
            float outL = mid + side;
            float outR = mid - side;
            
            // Защита от клиппинга
            outL = juce::jlimit(-1.0f, 1.0f, outL);
            outR = juce::jlimit(-1.0f, 1.0f, outR);
            
            buffer.setSample(0, sample, outL);
            if (numChannels > 1) buffer.setSample(1, sample, outR);
        }
    }

    void reset()
    {
        chorus.reset();
        phaser.reset();
        reverb.reset();
        delayBuffer.clear();
        delayWriteIndex = 0;
    }

private:
    double currentSampleRate = 44100.0;
    int currentNumChannels = 2;
    
    // Bypass flags
    bool distEnabled = false;
    bool satEnabled = false;
    bool chorusEnabled = false;
    bool delayEnabled = false;
    bool reverbEnabled = false;
    bool phaserEnabled = false;
    
    // JUCE DSP модули
    juce::dsp::Chorus<float> chorus;
    juce::dsp::Phaser<float> phaser;
    juce::dsp::Reverb reverb;
    juce::dsp::Reverb::Parameters reverbParams;
    
    // Delay (ручная реализация для гибкости)
    juce::AudioBuffer<float> delayBuffer;
    int maxDelaySamples = 0;
    int delayWriteIndex = 0;
    
    // Smoothed parameters
    juce::LinearSmoothedValue<float> smoothedDistDrive, smoothedDistMix;
    juce::LinearSmoothedValue<float> smoothedSatDrive, smoothedSatMix;
    juce::LinearSmoothedValue<float> smoothedDelayTime, smoothedDelayFeedback, smoothedDelayMix;
    juce::LinearSmoothedValue<float> smoothedReverbSize, smoothedReverbDamp, smoothedReverbMix;
    juce::LinearSmoothedValue<float> smoothedPhaserRate, smoothedPhaserDepth, smoothedPhaserMix;
    juce::LinearSmoothedValue<float> smoothedChorusRate, smoothedChorusDepth, smoothedChorusMix;
    juce::LinearSmoothedValue<float> smoothedWidth;
};
