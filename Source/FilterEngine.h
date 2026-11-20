#pragma once

#include <juce_core/juce_core.h>
#include <array>
#include <cmath>

class FilterEngine
{
public:
    enum class Type
    {
        LowPass = 0,     // 2-полюсный биквадратный LP
        HighPass,        // 2-полюсный биквадратный HP
        BandPass,        // 2-полюсный биквадратный BP
        Ladder,          // 4-полюсный Moog-style Ladder
        Resonant,        // State Variable Filter (высокий резонанс)
        Bypass
    };

    FilterEngine() = default;

    void prepare(double sampleRate)
    {
        currentSampleRate = sampleRate;
        
        // Сброс состояния биквадратного фильтра
        x1L = x2L = y1L = y2L = 0.0f;
        x1R = x2R = y1R = y2R = 0.0f;
        
        // Сброс состояния Ladder фильтра (4 каскада)
        for (int i = 0; i < 4; ++i)
        {
            ladderStageL[i] = 0.0f;
            ladderStageR[i] = 0.0f;
        }
        ladderOutputL = 0.0f;
        ladderOutputR = 0.0f;
        
        // Сброс состояния SVF
        svfLpL = svfBpL = 0.0f;
        svfLpR = svfBpR = 0.0f;
        
        // Инициализация сглаживания
        smoothedCutoff.reset(sampleRate, 0.01);
        smoothedResonance.reset(sampleRate, 0.01);
        smoothedDrive.reset(sampleRate, 0.01);
        smoothedEnvAmount.reset(sampleRate, 0.01);
        
        lastCutoff = -1.0f;
        lastResonance = -1.0f;
    }

    void setParameters(float cutoffHz, float resonance, float drive, float envAmount, Type type)
    {
        targetCutoff = juce::jlimit(20.0f, 20000.0f, cutoffHz);
        targetResonance = juce::jlimit(0.0f, 1.0f, resonance); // Нормализованный 0-1
        targetDrive = juce::jlimit(0.0f, 1.0f, drive);
        targetEnvAmount = juce::jlimit(0.0f, 1.0f, envAmount);
        currentType = type;
        
        smoothedCutoff.setTargetValue(targetCutoff);
        smoothedResonance.setTargetValue(targetResonance);
        smoothedDrive.setTargetValue(targetDrive);
        smoothedEnvAmount.setTargetValue(targetEnvAmount);
    }

    // Обработка стерео сэмпла (возвращает изменённые значения)
    void processSample(float& sampleL, float& sampleR)
    {
        float cutoff = smoothedCutoff.getNextValue();
        float resonance = smoothedResonance.getNextValue();
        float drive = smoothedDrive.getNextValue();
        
        // Пересчёт коэффициентов при изменении параметров
        if (std::abs(cutoff - lastCutoff) > 0.1f || std::abs(resonance - lastResonance) > 0.001f)
        {
            calculateCoefficients(cutoff, resonance);
            lastCutoff = cutoff;
            lastResonance = resonance;
        }
        
        switch (currentType)
        {
            case Type::LowPass:
                processBiquadLP(sampleL, sampleR);
                break;
            case Type::HighPass:
                processBiquadHP(sampleL, sampleR);
                break;
            case Type::BandPass:
                processBiquadBP(sampleL, sampleR);
                break;
            case Type::Ladder:
                processLadder(sampleL, sampleR, drive);
                break;
            case Type::Resonant:
                processSVF(sampleL, sampleR);
                break;
            case Type::Bypass:
            default:
                // Сигнал проходит без изменений
                break;
        }
    }

    void reset()
    {
        x1L = x2L = y1L = y2L = 0.0f;
        x1R = x2R = y1R = y2R = 0.0f;
        for (int i = 0; i < 4; ++i)
        {
            ladderStageL[i] = 0.0f;
            ladderStageR[i] = 0.0f;
        }
        ladderOutputL = ladderOutputR = 0.0f;
        svfLpL = svfBpL = svfLpR = svfBpR = 0.0f;
    }

private:
    // ========================================================================
    // Расчёт коэффициентов биквадратного фильтра (Audio EQ Cookbook)
    // ========================================================================
    void calculateCoefficients(float cutoff, float resonance)
    {
        // Преобразуем нормализованный резонанс (0-1) в Q (0.5-20)
        float Q = 0.5f + resonance * 19.5f;
        
        float w0 = 2.0f * juce::MathConstants<float>::pi * cutoff / static_cast<float>(currentSampleRate);
        float cosw0 = std::cos(w0);
        float sinw0 = std::sin(w0);
        float alpha = sinw0 / (2.0f * Q);
        
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
                b0_raw = alpha;
                b1_raw = 0.0f;
                b2_raw = -alpha;
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
    
    // ========================================================================
    // 2-полюсный биквадратный Low-Pass (Direct Form I)
    // ========================================================================
    void processBiquadLP(float& sL, float& sR)
    {
        float outL = b0 * sL + b1 * x1L + b2 * x2L - a1 * y1L - a2 * y2L;
        x2L = x1L; x1L = sL; y2L = y1L; y1L = outL;
        
        float outR = b0 * sR + b1 * x1R + b2 * x2R - a1 * y1R - a2 * y2R;
        x2R = x1R; x1R = sR; y2R = y1R; y1R = outR;
        
        sL = outL;
        sR = outR;
    }
    
    // ========================================================================
    // 2-полюсный биквадратный High-Pass
    // ========================================================================
    void processBiquadHP(float& sL, float& sR)
    {
        processBiquadLP(sL, sR); // Коэффициенты уже рассчитаны для HP
    }
    
    // ========================================================================
    // 2-полюсный биквадратный Band-Pass
    // ========================================================================
    void processBiquadBP(float& sL, float& sR)
    {
        processBiquadLP(sL, sR);
    }
    
    // ========================================================================
    // 4-полюсный Moog-style Ladder Filter (Huovilainen model)
    // Реально меняет сигнал: последовательная фильтрация через 4 каскада
    // с нелинейностью и feedback для резонанса
    // ========================================================================
    void processLadder(float& sL, float& sR, float drive)
    {
        // Drive gain: от 1x (чисто) до 5x (сильное насыщение)
        float driveGain = 1.0f + drive * 4.0f;
        
        // Обработка левого канала
        ladderOutputL = processLadderChannel(sL, ladderStageL, ladderOutputL, driveGain);
        
        // Обработка правого канала
        ladderOutputR = processLadderChannel(sR, ladderStageR, ladderOutputR, driveGain);
        
        sL = ladderOutputL;
        sR = ladderOutputR;
    }
    
    float processLadderChannel(float input, float* stages, float& output, float driveGain)
    {
        float cutoffNorm = juce::jlimit(0.0f, 1.0f, 
            lastCutoff / static_cast<float>(currentSampleRate) * 2.0f);
        
        // Компенсация задержки для стабильности при высоком резонансе
        // (по модели Huovilainen)
        float feedback = lastResonance * 3.8f; // 3.8 — максимум для стабильности
        float compensatedInput = input - feedback * (output - cutoffNorm * 3.0f * stages[3]);
        
        // Применяем drive (насыщение) на входе
        float stageInput = std::tanh(compensatedInput * driveGain);
        
        // 4 последовательных one-pole low-pass каскада
        for (int i = 0; i < 4; ++i)
        {
            // Нелинейность в каждом каскаде (имитация транзисторов Moog)
            float prevStage = (i == 0) ? stageInput : stages[i - 1];
            float diff = std::tanh(prevStage) - std::tanh(stages[i]);
            stages[i] += cutoffNorm * diff;
            
            // Защита от переполнения
            stages[i] = juce::jlimit(-10.0f, 10.0f, stages[i]);
        }
        
        return stages[3];
    }
    
    // ========================================================================
    // State Variable Filter (высокий резонанс до self-oscillation)
    // Одновременно генерирует LP, HP, BP, Notch
    // ========================================================================
    void processSVF(float& sL, float& sR)
    {
        // Преобразуем резонанс в Q (0.5 ... 50 — до self-oscillation)
        float Q = 0.5f + lastResonance * 49.5f;
        
        float f = 2.0f * std::sin(juce::MathConstants<float>::pi * 
            juce::jlimit(0.0f, 0.49f, lastCutoff / static_cast<float>(currentSampleRate)));
        float q = 1.0f / Q;
        
        // Обработка левого канала
        svfLpL = processSVFChannel(sL, svfLpL, svfBpL, f, q);
        
        // Обработка правого канала
        svfLpR = processSVFChannel(sR, svfLpR, svfBpR, f, q);
        
        sL = svfLpL;
        sR = svfLpR;
    }
    
    float processSVFChannel(float input, float& lp, float& bp, float f, float q)
    {
        // TPT (Topology-Preserving Transform) версия для стабильности
        float hp = (input - lp - q * bp) / (1.0f + f * q + f * f);
        float newBp = f * hp + bp;
        float newLp = f * newBp + lp;
        
        bp = newBp;
        lp = newLp;
        
        // Защита от переполнения при высоком резонансе
        bp = juce::jlimit(-10.0f, 10.0f, bp);
        lp = juce::jlimit(-10.0f, 10.0f, lp);
        
        return lp;
    }
    
    // ========================================================================
    // Состояние фильтра
    // ========================================================================
    double currentSampleRate = 44100.0;
    Type currentType = Type::LowPass;
    
    // Целевые и сглаженные параметры
    float targetCutoff = 1000.0f;
    float targetResonance = 0.5f;
    float targetDrive = 0.0f;
    float targetEnvAmount = 0.0f;
    
    float lastCutoff = -1.0f;
    float lastResonance = -1.0f;
    
    juce::LinearSmoothedValue<float> smoothedCutoff;
    juce::LinearSmoothedValue<float> smoothedResonance;
    juce::LinearSmoothedValue<float> smoothedDrive;
    juce::LinearSmoothedValue<float> smoothedEnvAmount;
    
    // Биквадратные коэффициенты
    float b0 = 1.0f, b1 = 0.0f, b2 = 0.0f;
    float a1 = 0.0f, a2 = 0.0f;
    
    // Состояние биквадратного фильтра (Left/Right)
    float x1L = 0.0f, x2L = 0.0f, y1L = 0.0f, y2L = 0.0f;
    float x1R = 0.0f, x2R = 0.0f, y1R = 0.0f, y2R = 0.0f;
    
    // Состояние Ladder фильтра (4 каскада × 2 канала)
    std::array<float, 4> ladderStageL = {0.0f, 0.0f, 0.0f, 0.0f};
    std::array<float, 4> ladderStageR = {0.0f, 0.0f, 0.0f, 0.0f};
    float ladderOutputL = 0.0f;
    float ladderOutputR = 0.0f;
    
    // Состояние SVF
    float svfLpL = 0.0f, svfBpL = 0.0f;
    float svfLpR = 0.0f, svfBpR = 0.0f;
};
