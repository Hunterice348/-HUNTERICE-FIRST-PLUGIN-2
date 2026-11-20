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

    // Oscillator
    juce::ComboBox oscTypeBox;
    juce::Slider pulseWidthSlider, wavetablePosSlider;
    juce::Label oscTypeLabel, pulseWidthLabel, wavetablePosLabel;

    // Sub Oscillator
    juce::ComboBox subTypeBox;
    juce::Slider subOctaveSlider, subMixSlider;
    juce::Label subTypeLabel, subOctaveLabel, subMixLabel;

    // Amp Envelope
    juce::Slider ampAttackSlider, ampDecaySlider, ampSustainSlider, ampReleaseSlider, ampVelSensSlider;
    juce::Label ampAttackLabel, ampDecayLabel, ampSustainLabel, ampReleaseLabel, ampVelSensLabel;

    // Filter Envelope
    juce::Slider filtAttackSlider, filtDecaySlider, filtSustainSlider, filtReleaseSlider, filtVelSensSlider;
    juce::Slider filtEnvAmountSlider, filtKeyTrackSlider;
    juce::Label filtAttackLabel, filtDecayLabel, filtSustainLabel, filtReleaseLabel, filtVelSensLabel;
    juce::Label filtEnvAmountLabel, filtKeyTrackLabel;

    // LFOs
    juce::Slider lfo1RateSlider, lfo2RateSlider;
    juce::ComboBox lfo1WaveBox, lfo2WaveBox;
    juce::Label lfo1RateLabel, lfo1WaveLabel, lfo2RateLabel, lfo2WaveLabel;

    // Filter
    juce::Slider filterCutoffSlider, filterResSlider;
    juce::ComboBox filterTypeBox;
    juce::Label filterCutoffLabel, filterResLabel, filterTypeLabel;

    // Unison & Detune
    juce::Slider unisonVoicesSlider, detuneSlider;
    juce::Label unisonVoicesLabel, detuneLabel;

    // Portamento
    juce::ToggleButton portamentoToggle;
    juce::Slider portamentoTimeSlider;
    juce::Label portamentoLabel, portamentoTimeLabel;

    // Modulation Matrix (4 слота для UI, остальные скрыты)
    juce::ComboBox modSourceBox[4], modDestBox[4];
    juce::Slider modAmountSlider[4];
    juce::Label modSourceLabel[4], modDestLabel[4], modAmountLabel[4];

    // Master
    juce::Slider volumeSlider;
    juce::Label volumeLabel;

    // Attachments
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;

    std::unique_ptr<ComboBoxAttachment> oscTypeAttachment, subTypeAttachment;
    std::unique_ptr<SliderAttachment> pulseWidthAttachment, wavetablePosAttachment;
    std::unique_ptr<SliderAttachment> subOctaveAttachment, subMixAttachment;
    
    std::unique_ptr<SliderAttachment> ampAttackAttachment, ampDecayAttachment, ampSustainAttachment, ampReleaseAttachment, ampVelSensAttachment;
    std::unique_ptr<SliderAttachment> filtAttackAttachment, filtDecayAttachment, filtSustainAttachment, filtReleaseAttachment, filtVelSensAttachment;
    std::unique_ptr<SliderAttachment> filtEnvAmountAttachment, filtKeyTrackAttachment;
    
    std::unique_ptr<SliderAttachment> lfo1RateAttachment, lfo2RateAttachment;
    std::unique_ptr<ComboBoxAttachment> lfo1WaveAttachment, lfo2WaveAttachment;
    
    std::unique_ptr<SliderAttachment> filterCutoffAttachment, filterResAttachment;
    std::unique_ptr<ComboBoxAttachment> filterTypeAttachment;
    
    std::unique_ptr<SliderAttachment> unisonVoicesAttachment, detuneAttachment;
    std::unique_ptr<ButtonAttachment> portamentoAttachment;
    std::unique_ptr<SliderAttachment> portamentoTimeAttachment;
    
    std::unique_ptr<ComboBoxAttachment> modSourceAttachment[4], modDestAttachment[4];
    std::unique_ptr<SliderAttachment> modAmountAttachment[4];
    
    std::unique_ptr<SliderAttachment> volumeAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeSynthAudioProcessorEditor)
};
