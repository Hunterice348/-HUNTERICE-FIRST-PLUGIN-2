#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include "WavetableData.h"

class OscillatorEngine
{
public:
    enum class Type
    {
        Sine = 0,
        Triangle,
        Saw,
        Square,
        Pulse,
        Noise,
        Sub,
        Wavetable
    };

    OscillatorEngine() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        phase = 0.0;
        lastOutput = 0.0f;
        lastLastOutput = 0.0f;
        triangleState = 0.0f;
        noiseGen.setSeed(12345);
    }

    void setType(Type newType)
    {
        currentType = newType;
    }

    void setFrequency(float freqHz)
    {
        currentFrequency = juce::jlimit(0.1f, 20000.0f, freqHz);
    }

    void setPulseWidth(float width)
    {
        // 0.01 ... 0.99 (избегаем 0 и 1 для стабильности)
        currentPulseWidth = juce::jlimit(0.01f, 0.99f, width);
    }

    void setSubOctave(int octave)
    {
        // -1 = октава ниже, -2 = две октавы ниже
        subOctave = juce::jlimit(-3, 0, octave);
    }

    void setWavetablePosition(float position)
    {
        // 0.0 = Sine, 0.33 = Triangle, 0.66 = Saw, 1.0 = Square
        wavetablePosition = juce::jlimit(0.0f, 1.0f, position);
    }

    // Основной метод обработки — возвращает значение в диапазоне -1.0 ... 1.0
    float processSample()
    {
        float output = 0.0f;
        
        // Инкремент фазы (используем double для точности)
        double phaseIncrement = static_cast<double>(currentFrequency) / currentSampleRate;

        switch (currentType)
        {
            case Type::Sine:
                output = processSine();
                break;

            case Type::Triangle:
                output = processTriangle();
                break;

            case Type::Saw:
                output = processSaw();
                break;

            case Type::Square:
                output = processSquare();
                break;

            case Type::Pulse:
                output = processPulse();
                break;

            case Type::Noise:
                output = processNoise();
                break;

            case Type::Sub:
                output = processSub();
                break;

            case Type::Wavetable:
                output = processWavetable();
                break;
        }

        // Обновляем фазу
        phase += phaseIncrement;
        if (phase >= 1.0)
            phase -= 1.0;
        if (phase < 0.0)
            phase += 1.0;

        return output;
    }

    void reset()
    {
        phase = 0.0;
        lastOutput = 0.0f;
        lastLastOutput = 0.0f;
        triangleState = 0.0f;
    }

    // Синхронизация фазы (для hard sync)
    void syncPhase()
    {
        phase = 0.0;
    }

    double getCurrentPhase() const { return phase; }

private:
    // ========================================================================
    // SINE — не требует антиалиасинга (одна гармоника)
    // ========================================================================
    float processSine()
    {
        return static_cast<float>(std::sin(phase * 2.0 * juce::MathConstants<double>::pi));
    }

    // ========================================================================
    // TRIANGLE — PolyBLAMP антиалиасинг
    // ========================================================================
    float processTriangle()
    {
        double phaseIncrement = currentFrequency / currentSampleRate;
        
        // Naive triangle: интегрируем square wave
        float square = (phase < 0.5) ? 1.0f : -1.0f;
        triangleState += static_cast<float>(4.0 * phaseIncrement) * square;
        
        // Ограничение для стабильности
        if (triangleState > 1.0f) triangleState = 1.0f;
        if (triangleState < -1.0f) triangleState = -1.0f;
        
        // PolyBLAMP для сглаживания разрывов производной
        float blamp = polyBLAMP(phase, phaseIncrement);
        
        return triangleState + blamp * 4.0f * static_cast<float>(phaseIncrement);
    }

    // ========================================================================
    // SAW — PolyBLEP антиалиасинг через mip-mapped wavetable
    // ========================================================================
    float processSaw()
    {
        return WavetableData::getSawTable().processSample(
            phase, currentFrequency, currentSampleRate);
    }

    // ========================================================================
    // SQUARE — PolyBLEP антиалиасинг через mip-mapped wavetable
    // ========================================================================
    float processSquare()
    {
        return WavetableData::getSquareTable().processSample(
            phase, currentFrequency, currentSampleRate);
    }

    // ========================================================================
    // PULSE — Square с переменной шириной импульса + PolyBLEP
    // ========================================================================
    float processPulse()
    {
        double phaseIncrement = currentFrequency / currentSampleRate;
        
        // Naive pulse
        float naive = (phase < currentPulseWidth) ? 1.0f : -1.0f;
        
        // DC offset correction: pulse имеет DC-компоненту
        float dcOffset = 2.0f * currentPulseWidth - 1.0f;
        
        // PolyBLEP на обоих переходах
        float blep1 = polyBLEP(phase, phaseIncrement);
        float blep2 = polyBLEP(fmod(phase - currentPulseWidth + 1.0, 1.0), phaseIncrement);
        
        return naive - dcOffset - 2.0f * blep1 + 2.0f * blep2;
    }

    // ========================================================================
    // NOISE — белый шум (антиалиасинг не требуется)
    // ========================================================================
    float processNoise()
    {
        return noiseGen.nextFloat() * 2.0f - 1.0f;
    }

    // ========================================================================
    // SUB — осциллятор на октаву (или несколько) ниже
    // ========================================================================
    float processSub()
    {
        // Делим фазу на 2 в степени octave
        double subPhase = phase * std::pow(2.0, subOctave);
        subPhase = subPhase - std::floor(subPhase);
        
        // Sub обычно играет square wave (классика синтезаторов)
        return (subPhase < 0.5) ? 1.0f : -1.0f;
    }

    // ========================================================================
    // WAVETABLE — морфинг между формами волн
    // ========================================================================
    float processWavetable()
    {
        // Интерполяция между 4 формами волн:
        // 0.00 - 0.33: Sine → Triangle
        // 0.33 - 0.66: Triangle → Saw
        // 0.66 - 1.00: Saw → Square
        
        float sine = processSine();
        float triangle = processTriangle();
        float saw = processSaw();
        float square = processSquare();
        
        if (wavetablePosition < 0.33f)
        {
            float t = wavetablePosition / 0.33f;
            return sine * (1.0f - t) + triangle * t;
        }
        else if (wavetablePosition < 0.66f)
        {
            float t = (wavetablePosition - 0.33f) / 0.33f;
            return triangle * (1.0f - t) + saw * t;
        }
        else
        {
            float t = (wavetablePosition - 0.66f) / 0.34f;
            return saw * (1.0f - t) + square * t;
        }
    }

    // ========================================================================
    // PolyBLEP — Polynomial Band-Limited Step
    // Корректирует разрывы в naively сгенерированных waveforms
    // ========================================================================
    static float polyBLEP(double t, double dt)
    {
        if (dt <= 0.0) return 0.0f;
        
        if (t < dt)
        {
            // Вблизи перехода (в начале периода)
            t /= dt;
            return static_cast<float>(t + t - t * t - 1.0);
        }
        else if (t > 1.0 - dt)
        {
            // Вблизи перехода (в конце периода)
            t = (t - 1.0) / dt;
            return static_cast<float>(t * t + t + t + 1.0);
        }
        return 0.0f;
    }

    // ========================================================================
    // PolyBLAMP — Polynomial Band-Limited Ramp
    // Для сглаживания разрывов производной (Triangle)
    // ========================================================================
    static float polyBLAMP(double t, double dt)
    {
        if (dt <= 0.0) return 0.0f;
        
        if (t < dt)
        {
            t /= dt;
            return static_cast<float>((1.0 / 3.0) * (t * t * t));
        }
        else if (t > 1.0 - dt)
        {
            t = (t - 1.0) / dt;
            return static_cast<float>(-(1.0 / 3.0) * (t * t * t));
        }
        return 0.0f;
    }

    // ========================================================================
    // Состояние осциллятора
    // ========================================================================
    double currentSampleRate = 44100.0;
    double phase = 0.0;  // Используем double для точности фазы
    float currentFrequency = 440.0f;
    Type currentType = Type::Sine;
    
    // Pulse width (0.01 ... 0.99)
    float currentPulseWidth = 0.5f;
    
    // Sub oscillator
    int subOctave = -1;
    
    // Wavetable morphing position (0.0 ... 1.0)
    float wavetablePosition = 0.0f;
    
    // Triangle integrator state
    float triangleState = 0.0f;
    
    // Noise generator
    juce::Random noiseGen;
    
    // Для DC blocking filter (опционально)
    float lastOutput = 0.0f;
    float lastLastOutput = 0.0f;
};
