#pragma once

#include <juce_core/juce_core.h>

class EnvelopeEngine
{
public:
    enum class Stage
    {
        Idle,
        Attack,
        Decay,
        Sustain,
        Release
    };

    EnvelopeEngine() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        currentValue = 0.0f;
        currentStage = Stage::Idle;
    }

    void setParameters(float attackSec, float decaySec, float sustainLevel, float releaseSec)
    {
        attackTime = juce::jmax(0.001f, attackSec);
        decayTime = juce::jmax(0.001f, decaySec);
        sustainLevel = juce::jlimit(0.0f, 1.0f, sustainLevel);
        releaseTime = juce::jmax(0.001f, releaseSec);

        // Расчёт коэффициентов для экспоненциальных кривых
        attackCoeff = std::exp(-1.0f / (attackTime * currentSampleRate));
        decayCoeff = std::exp(-1.0f / (decayTime * currentSampleRate));
        releaseCoeff = std::exp(-1.0f / (releaseTime * currentSampleRate));
    }

    void setVelocitySensitivity(float sensitivity)
    {
        velocitySensitivity = juce::jlimit(0.0f, 1.0f, sensitivity);
    }

    void noteOn(float velocity = 1.0f)
    {
        currentStage = Stage::Attack;
        currentVelocity = velocity;
        
        // Velocity влияет на пиковое значение envelope
        float velocityMod = 1.0f - velocitySensitivity * (1.0f - velocity);
        targetPeak = velocityMod;
    }

    void noteOff()
    {
        if (currentStage != Stage::Idle)
            currentStage = Stage::Release;
    }

    float getNextSample()
    {
        switch (currentStage)
        {
            case Stage::Idle:
                currentValue = 0.0f;
                break;

            case Stage::Attack:
                currentValue += (targetPeak - currentValue) * (1.0f - attackCoeff);
                if (currentValue >= targetPeak * 0.999f)
                {
                    currentValue = targetPeak;
                    currentStage = Stage::Decay;
                }
                break;

            case Stage::Decay:
                currentValue += (sustainLevel - currentValue) * (1.0f - decayCoeff);
                if (std::abs(currentValue - sustainLevel) < 0.001f)
                {
                    currentValue = sustainLevel;
                    currentStage = Stage::Sustain;
                }
                break;

            case Stage::Sustain:
                currentValue = sustainLevel;
                break;

            case Stage::Release:
                currentValue *= releaseCoeff;
                if (currentValue < 0.001f)
                {
                    currentValue = 0.0f;
                    currentStage = Stage::Idle;
                }
                break;
        }

        return currentValue;
    }

    bool isActive() const
    {
        return currentStage != Stage::Idle;
    }

    void reset()
    {
        currentValue = 0.0f;
        currentStage = Stage::Idle;
    }

    Stage getCurrentStage() const { return currentStage; }
    float getCurrentValue() const { return currentValue; }

private:
    double currentSampleRate = 44100.0;
    
    float attackTime = 0.01f;
    float decayTime = 0.1f;
    float sustainLevel = 0.7f;
    float releaseTime = 0.5f;
    
    float attackCoeff = 0.0f;
    float decayCoeff = 0.0f;
    float releaseCoeff = 0.0f;
    
    float currentValue = 0.0f;
    float targetPeak = 1.0f;
    float currentVelocity = 1.0f;
    float velocitySensitivity = 0.0f;
    
    Stage currentStage = Stage::Idle;
};
