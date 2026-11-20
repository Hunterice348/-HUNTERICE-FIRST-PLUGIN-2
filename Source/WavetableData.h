#pragma once

#include <juce_core/juce_core.h>
#include <vector>
#include <array>
#include <cmath>

namespace WavetableData
{
    // Размер одной таблицы (степень двойки для быстрой интерполяции)
    constexpr int TABLE_SIZE = 2048;
    
    // Количество уровней mip-mapping (каждый уровень имеет в 2 раза меньше гармоник)
    constexpr int NUM_MIP_LEVELS = 11;
    
    // Генерация band-limited Saw wave для конкретного уровня гармоник
    inline void generateBandLimitedSaw(std::vector<float>& table, int numHarmonics)
    {
        table.assign(TABLE_SIZE, 0.0f);
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            double phase = static_cast<double>(i) / TABLE_SIZE;
            double value = 0.0;
            for (int h = 1; h <= numHarmonics; ++h)
            {
                // Saw: сумма sin(n*x)/n с чередующимся знаком
                value += std::sin(2.0 * juce::MathConstants<double>::pi * h * phase) / h;
            }
            // Нормализация: сумма ряда = pi/2 при бесконечном числе гармоник
            table[i] = static_cast<float>((2.0 / juce::MathConstants<double>::pi) * value);
        }
    }
    
    // Генерация band-limited Square wave
    inline void generateBandLimitedSquare(std::vector<float>& table, int numHarmonics)
    {
        table.assign(TABLE_SIZE, 0.0f);
        for (int i = 0; i < TABLE_SIZE; ++i)
        {
            double phase = static_cast<double>(i) / TABLE_SIZE;
            double value = 0.0;
            // Square содержит только нечётные гармоники
            for (int h = 1; h <= numHarmonics; h += 2)
            {
                value += std::sin(2.0 * juce::MathConstants<double>::pi * h * phase) / h;
            }
            table[i] = static_cast<float>((4.0 / juce::MathConstants<double>::pi) * value);
        }
    }
    
    // Класс для хранения mip-mapped таблиц
    class MipMappedTable
    {
    public:
        enum class WaveType { Saw, Square };
        
        MipMappedTable() = default;
        
        void initialise(WaveType type)
        {
            // Максимальное число гармоник на верхнем уровне = TABLE_SIZE / 2
            int maxHarmonics = TABLE_SIZE / 2;
            
            for (int level = 0; level < NUM_MIP_LEVELS; ++level)
            {
                int numHarmonics = maxHarmonics >> level; // Делим на 2 на каждом уровне
                if (numHarmonics < 1) numHarmonics = 1;
                
                tables[level].resize(TABLE_SIZE);
                
                if (type == WaveType::Saw)
                    generateBandLimitedSaw(tables[level], numHarmonics);
                else
                    generateBandLimitedSquare(tables[level], numHarmonics);
            }
        }
        
        // Выбор правильного mip-уровня в зависимости от частоты
        // freq: текущая частота осциллятора
        // sampleRate: частота дискретизации
        float processSample(double phase, float freq, double sampleRate) const
        {
            // Nyquist frequency
            double nyquist = sampleRate * 0.5;
            
            // Определяем уровень: сколько гармоник поместится до Nyquist
            // level = log2(maxHarmonics * freq / nyquist)
            double levelDouble = std::log2(static_cast<double>(TABLE_SIZE / 2) * freq / nyquist);
            int level = static_cast<int>(std::floor(levelDouble));
            level = juce::jlimit(0, NUM_MIP_LEVELS - 1, level);
            
            // Интерполяция между двумя соседними уровнями для плавности
            float frac = static_cast<float>(levelDouble - level);
            
            float sample1 = readTable(level, phase);
            float sample2 = readTable(juce::jmin(level + 1, NUM_MIP_LEVELS - 1), phase);
            
            return sample1 + frac * (sample2 - sample1);
        }
        
    private:
        float readTable(int level, double phase) const
        {
            // Нормализуем фазу в диапазон [0, 1)
            double normalizedPhase = phase - std::floor(phase);
            if (normalizedPhase < 0.0) normalizedPhase += 1.0;
            
            // Позиция в таблице
            double pos = normalizedPhase * TABLE_SIZE;
            int idx0 = static_cast<int>(pos) & (TABLE_SIZE - 1);
            int idx1 = (idx0 + 1) & (TABLE_SIZE - 1);
            float frac = static_cast<float>(pos - std::floor(pos));
            
            // Линейная интерполяция
            const auto& table = tables[level];
            return table[idx0] + frac * (table[idx1] - table[idx0]);
        }
        
        std::array<std::vector<float>, NUM_MIP_LEVELS> tables;
    };
    
    // Глобальные таблицы (инициализируются один раз)
    inline MipMappedTable& getSawTable()
    {
        static MipMappedTable table;
        static bool initialised = false;
        if (!initialised)
        {
            table.initialise(MipMappedTable::WaveType::Saw);
            initialised = true;
        }
        return table;
    }
    
    inline MipMappedTable& getSquareTable()
    {
        static MipMappedTable table;
        static bool initialised = false;
        if (!initialised)
        {
            table.initialise(MipMappedTable::WaveType::Square);
            initialised = true;
        }
        return table;
    }
}
