#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>

class DistortionEngine
{
public:
    enum class Mode
    {
        HardClip = 0,
        SoftClip,
        Tube,
        Diode,
        Fuzz,
        Foldback,
        WaveShaping,
        Asymmetric,
        Digital,
        Industrial
    };

    DistortionEngine() = default;

    void prepare(double sampleRate, int numChannels)
    {
        currentSampleRate = sampleRate;
        
        // Инициализация сглаживания (20 мс для избежания щелчков)
        const double smoothTime = 0.02;
        driveSmoothed.reset(sampleRate, smoothTime);
        mixSmoothed.reset(sampleRate, smoothTime);
        outputSmoothed.reset(sampleRate, smoothTime);
        toneSmoothed.reset(sampleRate, smoothTime);
        biasSmoothed.reset(sampleRate, smoothTime);
        thresholdSmoothed.reset(sampleRate, smoothTime);
        symmetrySmoothed.reset(sampleRate, smoothTime);

        // Инициализация фильтров низких частот для каждого канала (для параметра Tone)
        filterStates.resize(numChannels, 0.0f);
        filterAlpha = 0.0f;
    }

    // Обновление целевых значений параметров (вызывать один раз за блок в processBlock)
    void setTargetValues(float drive, float mix, float outputDb, float tone, float bias, float threshold, float symmetry)
    {
        driveSmoothed.setTargetValue(drive);
        mixSmoothed.setTargetValue(mix);
        outputSmoothed.setTargetValue(juce::Decibels::decibelsToGain(outputDb, -60.0f));
        toneSmoothed.setTargetValue(tone); // 0.0 ... 1.0
        biasSmoothed.setTargetValue(bias); // -0.5 ... 0.5
        thresholdSmoothed.setTargetValue(threshold); // 0.1 ... 1.0
        symmetrySmoothed.setTargetValue(symmetry); // -1.0 ... 1.0
    }

    // Обработка аудиобуфера
    void process(juce::AudioBuffer<float>& buffer, Mode mode)
    {
        const int numChannels = buffer.getNumChannels();
        const int numSamples = buffer.getNumSamples();

        // Убедимся, что массив фильтров соответствует количеству каналов
        if (filterStates.size() != static_cast<size_t>(numChannels))
            filterStates.resize(numChannels, 0.0f);

        for (int sample = 0; sample < numSamples; ++sample)
        {
            // Получаем сглаженные значения на текущий сэмпл
            const float drive     = driveSmoothed.getNextValue();
            const float mix       = mixSmoothed.getNextValue();
            const float outGain   = outputSmoothed.getNextValue();
            const float toneNorm  = toneSmoothed.getNextValue();
            const float bias      = biasSmoothed.getNextValue();
            const float threshold = thresholdSmoothed.getNextValue();
            const float symmetry  = symmetrySmoothed.getNextValue();

            // Расчет коэффициента фильтра Tone (1-полюсный НЧ-фильтр)
            // toneNorm 0.0 = 200Hz, 1.0 = 20000Hz
            const float cutoffFreq = 200.0f + (toneNorm * toneNorm * 19800.0f);
            const float newAlpha = juce::jlimit(0.0f, 1.0f, 2.0f * juce::MathConstants<float>::pi * cutoffFreq / currentSampleRate);
            
            // Плавное изменение альфа-коэффициента фильтра
            filterAlpha += 0.01f * (newAlpha - filterAlpha);

            for (int channel = 0; channel < numChannels; ++channel)
            {
                float dry = buffer.getSample(channel, sample);
                float wet = dry;

                // 1. Применяем Bias (смещение по постоянному току)
                wet += bias;

                // 2. Применяем Drive (усиление перед искажением)
                const float driveGain = 1.0f + (drive * 49.0f); // от 1x до 50x
                wet *= driveGain;

                // 3. Основной алгоритм искажения
                switch (mode)
                {
                    case Mode::HardClip:
                        wet = juce::jlimit(-threshold, threshold, wet);
                        break;

                    case Mode::SoftClip:
                        wet = std::tanh(wet);
                        break;

                    case Mode::Tube:
                        // Кубическое сглаживание, имитирующее ламповое насыщение
                        wet = wet * (1.5f - 0.5f * wet * wet);
                        wet = juce::jlimit(-1.0f, 1.0f, wet);
                        break;

                    case Mode::Diode:
                        // Асимметричное ограничение, имитирующее диодный мост
                        wet = (wet > 0.0f) ? (wet / (1.0f + wet)) : (wet * 2.0f);
                        break;

                    case Mode::Fuzz:
                        // Экстремальное ограничение с обрезанием верхов
                        wet = std::tanh(wet * 5.0f) * 0.8f;
                        break;

                    case Mode::Foldback:
                        // Классический foldback (загибание волны)
                        {
                            float absWet = std::abs(wet);
                            if (absWet > threshold)
                                absWet = threshold - std::abs(absWet - threshold);
                            wet = std::copysign(absWet, wet);
                        }
                        break;

                    case Mode::WaveShaping:
                        // Полиномиальное формообразование
                        wet = wet - (wet * wet * wet) / 3.0f;
                        wet += symmetry * (wet * wet) * 0.5f; // Влияние симметрии
                        wet = juce::jlimit(-1.0f, 1.0f, wet);
                        break;

                    case Mode::Asymmetric:
                        // Разное усиление и ограничение для положительной и отрицательной полуволн
                        {
                            float asymDrive = 1.0f + symmetry; // -1.0...1.0 -> 0.0...2.0
                            if (asymDrive < 0.1f) asymDrive = 0.1f;
                            wet = (wet > 0.0f) ? std::tanh(wet * asymDrive) : std::tanh(wet / asymDrive);
                        }
                        break;

                    case Mode::Digital:
                        // Квантование (биткраш-эффект), управляемое threshold
                        {
                            const float levels = juce::jmap(threshold, 4.0f, 256.0f);
                            wet = std::round(wet * levels) / levels;
                        }
                        break;

                    case Mode::Industrial:
                        // Жесткое выпрямление и модуляция
                        wet = std::abs(wet) * 2.0f - 1.0f;
                        wet = juce::jlimit(-threshold, threshold, wet);
                        break;
                }

                // 4. Применяем Tone (1-полюсный НЧ-фильтр)
                filterStates[channel] += filterAlpha * (wet - filterStates[channel]);
                wet = filterStates[channel];

                // 5. Dry/Wet Mix
                wet = dry * (1.0f - mix) + wet * mix;

                // 6. Output Gain
                wet *= outGain;

                // 7. ЗАЩИТА: Финальный Brickwall Limiter (предотвращение inter-sample peaks и NaN)
                wet = juce::jlimit(-1.0f, 1.0f, wet);

                buffer.setSample(channel, sample, wet);
            }
        }
    }

    // Геттеры для привязки к APVTS (если потребуется прямое чтение)
    juce::LinearSmoothedValue<float>& getDriveSmoothed() { return driveSmoothed; }
    juce::LinearSmoothedValue<float>& getMixSmoothed() { return mixSmoothed; }
    juce::LinearSmoothedValue<float>& getOutputSmoothed() { return outputSmoothed; }
    juce::LinearSmoothedValue<float>& getToneSmoothed() { return toneSmoothed; }
    juce::LinearSmoothedValue<float>& getBiasSmoothed() { return biasSmoothed; }
    juce::LinearSmoothedValue<float>& getThresholdSmoothed() { return thresholdSmoothed; }
    juce::LinearSmoothedValue<float>& getSymmetrySmoothed() { return symmetrySmoothed; }

private:
    double currentSampleRate = 44100.0;

    juce::LinearSmoothedValue<float> driveSmoothed;
    juce::LinearSmoothedValue<float> mixSmoothed;
    juce::LinearSmoothedValue<float> outputSmoothed;
    juce::LinearSmoothedValue<float> toneSmoothed;
    juce::LinearSmoothedValue<float> biasSmoothed;
    juce::LinearSmoothedValue<float> thresholdSmoothed;
    juce::LinearSmoothedValue<float> symmetrySmoothed;

    std::vector<float> filterStates;
    float filterAlpha = 0.0f;
};
