#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

class LFOEngine
{
public:
    enum class Waveform
    {
        Sine = 0,
        Triangle,
        SawUp,
        SawDown,
        Square,
        Random,
        SampleAndHold
    };

    LFOEngine() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        phase = 0.0;
        lastRandomValue = 0.0f;
        currentSHValue = 0.0f;
    }

    void setRate(float rateHz)
    {
        currentRate = juce::jlimit(0.01f, 50.0f, rateHz);
    }

    void setWaveform(Waveform wf)
    {
        currentWaveform = wf;
    }

    // Возвращает значение LFO в диапазоне -1.0 ... 1.0
    float processSample()
    {
        float output = 0.0f;

        switch (currentWaveform)
        {
            case Waveform::Sine:
                output = static_cast<float>(std::sin(phase * 2.0 * juce::MathConstants<double>::pi));
                break;

            case Waveform::Triangle:
            {
                double normalizedPhase = phase - std::floor(phase);
                if (normalizedPhase < 0.25)
                    output = static_cast<float>(normalizedPhase * 4.0);
                else if (normalizedPhase < 0.75)
                    output = static_cast<float>(2.0 - normalizedPhase * 4.0);
                else
                    output = static_cast<float>(normalizedPhase * 4.0 - 4.0);
                break;
            }

            case Waveform::SawUp:
            {
                double normalizedPhase = phase - std::floor(phase);
                output = static_cast<float>(normalizedPhase * 2.0 - 1.0);
                break;
            }

            case Waveform::SawDown:
            {
                double normalizedPhase = phase - std::floor(phase);
                output = static_cast<float>(1.0 - normalizedPhase * 2.0);
                break;
            }

            case Waveform::Square:
            {
                double normalizedPhase = phase - std::floor(phase);
                output = (normalizedPhase < 0.5) ? 1.0f : -1.0f;
                break;
            }

            case Waveform::Random:
            {
                // Сглаженный случайный сигнал (interpolated noise)
                double normalizedPhase = phase - std::floor(phase);
                if (normalizedPhase < 0.001)
                {
                    lastRandomValue = currentRandomValue;
                    currentRandomValue = static_cast<float>(random.nextFloat() * 2.0 - 1.0);
                }
                // Линейная интерполяция между двумя случайными значениями
                output = lastRandomValue + (currentRandomValue - lastRandomValue) * static_cast<float>(normalizedPhase);
                break;
            }

            case Waveform::SampleAndHold:
            {
                double normalizedPhase = phase - std::floor(phase);
                if (normalizedPhase < 0.001)
                {
                    currentSHValue = static_cast<float>(random.nextFloat() * 2.0 - 1.0);
                }
                output = currentSHValue;
                break;
            }
        }

        // Обновляем фазу
        phase += currentRate / currentSampleRate;
        if (phase >= 1.0)
            phase -= 1.0;

        return output;
    }

    void reset()
    {
        phase = 0.0;
    }

private:
    double currentSampleRate = 44100.0;
    double phase = 0.0;
    float currentRate = 1.0f;
    Waveform currentWaveform = Waveform::Sine;

    // Для Random и S&H
    juce::Random random;
    float lastRandomValue = 0.0f;
    float currentRandomValue = 0.0f;
    float currentSHValue = 0.0f;
};
