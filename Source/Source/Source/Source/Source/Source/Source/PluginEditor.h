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

    // Title
    juce::Label titleLabel;

    // Main Oscillator
    juce::ComboBox oscTypeBox;
    juce::Slider pulseWidthSlider;
    juce::Slider wavetablePosSlider;
    juce::Label oscTypeLabel, pulseWidthLabel, wavetablePosLabel;

    // Sub Oscillator
    juce::ComboBox subTypeBox;
    juce::Slider subOctaveSlider;
    juce::Slider subMixSlider;
    juce::Label subTypeLabel, subOctaveLabel, subMixLabel;

    // ADSR
    juce::Slider attackSlider, decaySlider, sustainSlider, releaseSlider, volumeSlider;
    juce::Label attackLabel, decayLabel, sustainLabel, releaseLabel, volumeLabel;

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

    std::unique_ptr<ComboBoxAttachment> oscTypeAttachment, subTypeAttachment;
    std::unique_ptr<SliderAttachment> pulseWidthAttachment, wavetablePosAttachment;
    std::unique_ptr<SliderAttachment> subOctaveAttachment, subMixAttachment;
    std::unique_ptr<SliderAttachment> attackAttachment, decayAttachment, sustainAttachment, releaseAttachment, volumeAttachment;
    std::unique_ptr<SliderAttachment> lfoRateAttachment;
    std::unique_ptr<ComboBoxAttachment> lfoWaveAttachment;
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment;
    std::unique_ptr<ComboBoxAttachment> filterTypeAttachment;
    std::unique_ptr<SliderAttachment> pitchModAttachment, filterModAttachment, ampModAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeSynthAudioProcessorEditor)
};
