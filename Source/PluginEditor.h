#pragma once

// ИСПРАВЛЕНИЕ: Явное подключение необходимых модулей GUI и привязок
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "PluginProcessor.h"

class HuntericeFirstPluginAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit HuntericeFirstPluginAudioProcessorEditor(HuntericeFirstPluginAudioProcessor&);
    ~HuntericeFirstPluginAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    HuntericeFirstPluginAudioProcessor& processorRef;

    // Заголовок
    juce::Label titleLabel;

    // Слайдеры (ручки) для параметров
    juce::Slider driveSlider;
    juce::Slider mixSlider;
    juce::Slider outputSlider;

    // Подписи к слайдерам
    juce::Label driveLabel;
    juce::Label mixLabel;
    juce::Label outputLabel;

    // Привязки слайдеров к параметрам (автоматизация DAW + сохранение)
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    std::unique_ptr<SliderAttachment> driveAttachment;
    std::unique_ptr<SliderAttachment> mixAttachment;
    std::unique_ptr<SliderAttachment> outputAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeFirstPluginAudioProcessorEditor)
};
