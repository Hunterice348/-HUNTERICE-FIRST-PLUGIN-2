#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"

class HuntericeSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit HuntericeSynthAudioProcessorEditor(HuntericeSynthAudioProcessor&);
    ~HuntericeSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    HuntericeSynthAudioProcessor& processorRef;

    // UI Components
    juce::Label titleLabel;

    // Oscillator + ADSR
    juce::ComboBox waveformBox;
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, volumeSlider;
    juce::Label waveformLabel, attackLabel, decayLabel, sustainLabel, releaseLabel, volumeLabel;

    // LFO
    juce::Slider lfoRateSlider;
    juce::ComboBox lfoWaveBox;
    juce::Label lfoRateLabel, lfoWaveLabel;

    // Filter
    juce::Slider filterCutoffSlider, filterResSlider;
    juce::ComboBox filterTypeBox;
    juce::Label filterCutoffLabel, filterResLabel, filterTypeLabel;

    // Modulation
    juce::Slider pitchModSlider, filterModSlider, ampModSlider;
    juce::Label pitchModLabel, filterModLabel, ampModLabel;

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ComboBoxAttachment> waveformAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment, decayAttachment, sustainAttachment, releaseAttachment, volumeAttachment;
    std::unique_ptr<SliderAttachment> lfoRateAttachment;
    std::unique_ptr<ComboBoxAttachment> lfoWaveAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment;
    std::unique_ptr<ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<SliderAttachment> pitchModAttachment, filterModAttachment, ampModAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeSynthAudioProcessorEditor)
};
