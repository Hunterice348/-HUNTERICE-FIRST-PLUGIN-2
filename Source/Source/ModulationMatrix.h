#pragma once

#include <juce_core/juce_core.h>
#include <array>

class ModulationMatrix
{
public:
    // Источники модуляции
    enum class Source
    {
        Env1 = 0,
        Env2,
        LFO1,
        LFO2,
        Velocity,
        KeyTracking,
        ModWheel,
        NumSources
    };

    // Цели модуляции
    enum class Destination
    {
        Pitch = 0,
        FilterCutoff,
        PulseWidth,
        Amplitude,
        Detune,
        NumDestinations
    };

    ModulationMatrix()
    {
        // Инициализация всех слотов модуляции нулевыми значениями
        for (auto& slot : modulationSlots)
        {
            slot.source = Source::Env1;
            slot.destination = Destination::Pitch;
            slot.amount = 0.0f;
        }
    }

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        
        // Сглаживание для всех целей
        for (int i = 0; i < static_cast<int>(Destination::NumDestinations); ++i)
        {
            smoothedValues[i].reset(sampleRate, 0.02);
        }
    }

    // Установка слота модуляции
    void setModulationSlot(int slotIndex, Source source, Destination destination, float amount)
    {
        if (slotIndex >= 0 && slotIndex < MAX_MODULATION_SLOTS)
        {
            modulationSlots[slotIndex].source = source;
            modulationSlots[slotIndex].destination = destination;
            modulationSlots[slotIndex].amount = amount;
        }
    }

    // Обновление значений источников (вызывается каждый сэмпл)
    void updateSources(float env1, float env2, float lfo1, float lfo2, 
                       float velocity, float keyTracking, float modWheel)
    {
        sourceValues[static_cast<int>(Source::Env1)] = env1;
        sourceValues[static_cast<int>(Source::Env2)] = env2;
        sourceValues[static_cast<int>(Source::LFO1)] = lfo1;
        sourceValues[static_cast<int>(Source::LFO2)] = lfo2;
        sourceValues[static_cast<int>(Source::Velocity)] = velocity;
        sourceValues[static_cast<int>(Source::KeyTracking)] = keyTracking;
        sourceValues[static_cast<int>(Source::ModWheel)] = modWheel;
    }

    // Получение модулированного значения для конкретной цели
    float getModulatedValue(Destination destination)
    {
        float totalModulation = 0.0f;

        // Суммируем вклад всех слотов, назначенных на эту цель
        for (const auto& slot : modulationSlots)
        {
            if (slot.destination == destination)
            {
                float sourceValue = sourceValues[static_cast<int>(slot.source)];
                totalModulation += sourceValue * slot.amount;
            }
        }

        // Сглаживание для избежания щелчков
        smoothedValues[static_cast<int>(destination)].setTargetValue(totalModulation);
        return smoothedValues[static_cast<int>(destination)].getNextValue();
    }

    // Прямой доступ к smoothed values для инициализации
    juce::LinearSmoothedValue<float>& getSmoothedValue(Destination dest)
    {
        return smoothedValues[static_cast<int>(dest)];
    }

private:
    static constexpr int MAX_MODULATION_SLOTS = 16;

    struct ModulationSlot
    {
        Source source;
        Destination destination;
        float amount;
    };

    double currentSampleRate = 44100.0;
    std::array<ModulationSlot, MAX_MODULATION_SLOTS> modulationSlots;
    std::array<float, static_cast<int>(Source::NumSources)> sourceValues = {0.0f};
    std::array<juce::LinearSmoothedValue<float>, static_cast<int>(Destination::NumDestinations)> smoothedValues;
};
