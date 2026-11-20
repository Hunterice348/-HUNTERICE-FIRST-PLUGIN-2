#pragma once

#include <juce_core/juce_core.h>

class ModulationMatrix
{
public:
    ModulationMatrix() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        
        // Сглаживание глубины модуляции
        smoothedPitchMod.reset(sampleRate, 0.02);
        smoothedFilterMod.reset(sampleRate, 0.02);
        smoothedAmpMod.reset(sampleRate, 0.02);
    }

    void setModulationAmounts(float pitchMod, float filterMod, float ampMod)
    {
        // pitchMod: -1.0 ... 1.0 (в полутонах, умножается на диапазон)
        // filterMod: 0.0 ... 1.0 (множитель частоты среза)
        // ampMod: 0.0 ... 1.0 (глубина тремоло)
        
        smoothedPitchMod.setTargetValue(pitchMod);
        smoothedFilterMod.setTargetValue(filterMod);
        smoothedAmpMod.setTargetValue(ampMod);
    }

    // Применяет модуляцию к pitch (в полутонах)
    float applyPitchModulation(float lfoValue)
    {
        float depth = smoothedPitchMod.getNextValue();
        return lfoValue * depth * 12.0f; // ±12 полутонов максимум
    }

    // Применяет модуляцию к cutoff фильтра (множитель)
    float applyFilterModulation(float baseCutoff, float lfoValue)
    {
        float depth = smoothedFilterMod.getNextValue();
        // LFO модулирует cutoff в диапазоне 0.1x ... 10x
        float multiplier = std::pow(10.0f, lfoValue * depth);
        return baseCutoff * multiplier;
    }

    // Применяет модуляцию к амплитуде (тремоло)
    float applyAmpModulation(float lfoValue)
    {
        float depth = smoothedAmpMod.getNextValue();
        // Преобразуем из -1...1 в 0...1, затем применяем глубину
        float normalized = (lfoValue + 1.0f) * 0.5f;
        return 1.0f - (depth * (1.0f - normalized));
    }

    juce::LinearSmoothedValue<float>& getPitchModSmoothed() { return smoothedPitchMod; }
    juce::LinearSmoothedValue<float>& getFilterModSmoothed() { return smoothedFilterMod; }
    juce::LinearSmoothedValue<float>& getAmpModSmoothed() { return smoothedAmpMod; }

private:
    double currentSampleRate = 44100.0;
    
    juce::LinearSmoothedValue<float> smoothedPitchMod;
    juce::LinearSmoothedValue<float> smoothedFilterMod;
    juce::LinearSmoothedValue<float> smoothedAmpMod;
};
