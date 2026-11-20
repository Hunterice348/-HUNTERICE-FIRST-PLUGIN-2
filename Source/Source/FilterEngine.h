#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>

class FilterEngine
{
public:
    enum class Type
    {
        LowPass = 0,
        HighPass,
        BandPass,
        Notch,
        Bypass
    };

    FilterEngine() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        x1L = x2L = y1L = y2L = 0.0f;
        x1R = x2R = y1R = y2R = 0.0f;
        
        // Сглаживание частоты и резонанса
        smoothedCutoff.reset(sampleRate, 0.02);
        smoothedResonance.reset(sampleRate, 0.02);
    }

    void setParameters(float cutoffHz, float resonance, Type type)
    {
        targetCutoff = juce::jlimit(20.0f, 20000.0f, cutoffHz);
        targetResonance = juce::jlimit(0.1f, 20.0f, resonance);
        currentType = type;
        
        smoothedCutoff.setTargetValue(targetCutoff);
        smoothedResonance.setTargetValue(targetResonance);
    }

    // Обработка одного сэмпла (стерео)
    void processSample(float& sampleL, float& sampleR)
    {
        float cutoff = smoothedCutoff.getNextValue();
        float resonance = smoothedResonance.getNextValue();

        // Пересчитываем коэффициенты только при изменении параметров
        if (std::abs(cutoff - lastCutoff) > 0.1f || std::abs(resonance - lastResonance) > 0.01f)
        {
            calculateCoefficients(cutoff, resonance);
            lastCutoff = cutoff;
            lastResonance = resonance;
        }

        // Применяем биквадратный фильтр (Direct Form I)
        float outL = b0 * sampleL + b1 * x1L + b2 * x2L - a1 * y1L - a2 * y2L;
        x2L = x1L; x1L = sampleL;
        y2L = y1L; y1L = outL;

        float outR = b0 * sampleR + b1 * x1R + b2 * x2R - a1 * y1R - a2 * y2R;
        x2R = x1R; x1R = sampleR;
        y2R = y1R; y1R = outR;

        sampleL = outL;
        sampleR = outR;
    }

    void reset()
    {
        x1L = x2L = y1L = y2L = 0.0f;
        x1R = x2R = y1R = y2R = 0.0f;
    }

private:
    void calculateCoefficients(float cutoff, float resonance)
    {
        if (currentType == Type::Bypass)
        {
            b0 = 1.0f; b1 = b2 = a1 = a2 = 0.0f;
            return;
        }

        float w0 = 2.0f * juce::MathConstants<float>::pi * cutoff / static_cast<float>(currentSampleRate);
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * resonance);

        float b0_raw, b1_raw, b2_raw, a0, a1_raw, a2_raw;

        switch (currentType)
        {
            case Type::LowPass:
                b0_raw = (1.0f - cosw0) / 2.0f;
                b1_raw = 1.0f - cosw0;
                b2_raw = (1.0f - cosw0) / 2.0f;
                a0 = 1.0f + alpha;
                a1_raw = -2.0f * cosw0;
                a2_raw = 1.0f - alpha;
                break;

            case Type::HighPass:
                b0_raw = (1.0f + cosw0) / 2.0f;
                b1_raw = -(1.0f + cosw0);
                b2_raw = (1.0f + cosw0) / 2.0f;
                a0 = 1.0f + alpha;
                a1_raw = -2.0f * cosw0;
                a2_raw = 1.0f - alpha;
                break;

            case Type::BandPass:
                b0_raw = sinw0 / 2.0f;
                b1_raw = 0.0f;
                b2_raw = -sinw0 / 2.0f;
                a0 = 1.0f + alpha;
                a1_raw = -2.0f * cosw0;
                a2_raw = 1.0f - alpha;
                break;

            case Type::Notch:
                b0_raw = 1.0f;
                b1_raw = -2.0f * cosw0;
                b2_raw = 1.0f;
                a0 = 1.0f + alpha;
                a1_raw = -2.0f * cosw0;
                a2_raw = 1.0f - alpha;
                break;

            default:
                b0_raw = 1.0f; b1_raw = b2_raw = a1_raw = a2_raw = 0.0f; a0 = 1.0f;
                break;
        }

        // Нормализация
        b0 = b0_raw / a0;
        b1 = b1_raw / a0;
        b2 = b2_raw / a0;
        a1 = a1_raw / a0;
        a2 = a2_raw / a0;
    }

    double currentSampleRate = 44100.0;
    Type currentType = Type::LowPass;

    // Коэффициенты фильтра
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;

    // Состояние фильтра (Left)
    float x1L = 0.0f, x2L = 0.0f;
    float y1L = 0.0f, y2L = 0.0f;

    // Состояние фильтра (Right)
    float x1R = 0.0f, x2R = 0.0f;
    float y1R = 0.0f, y2R = 0.0f;

    // Параметры
    float targetCutoff = 1000.0f;
    float targetResonance = 1.0f;
    float lastCutoff = 0.0f;
    float lastResonance = 0.0f;

    juce::LinearSmoothedValue<float> smoothedCutoff;
    juce::LinearSmoothedValue<float> smoothedResonance;
};
