#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>
#include "PluginProcessor.h"
#include "HuntericeLookAndFeel.h"
#include "LevelMeter.h"
#include "PianoKeyboard.h"

class HuntericeSynthAudioProcessorEditor : public juce::AudioProcessorEditor
{
public:
    explicit HuntericeSynthAudioProcessorEditor(HuntericeSynthAudioProcessor&);
    ~HuntericeSynthAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;

private:
    HuntericeSynthAudioProcessor& processorRef;
    HuntericeLookAndFeel lookAndFeel;
    
    // === HEADER ===
    juce::Label titleLabel;
    juce::Label subtitleLabel;
    
    // === METERS ===
    LevelMeter inputMeterL, inputMeterR;
    LevelMeter outputMeterL, outputMeterR;
    juce::Label inputLabel, outputLabel;
    
    // === TABS ===
    juce::TabbedComponent tabs;
    
    // === OSCILLATOR TAB ===
    juce::ComboBox oscTypeBox;
    juce::Slider pulseWidthSlider, wavetablePosSlider;
    juce::ComboBox subTypeBox;
    juce::Slider subOctaveSlider, subMixSlider;
    juce::Label oscTypeLabel, pulseWidthLabel, wavetablePosLabel;
    juce::Label subTypeLabel, subOctaveLabel, subMixLabel;
    
    // === ENVELOPE TAB ===
    juce::Slider ampAttackSlider, ampDecaySlider, ampSustainSlider, ampReleaseSlider;
    juce::Slider filtAttackSlider, filtDecaySlider, filtSustainSlider, filtReleaseSlider;
    juce::Label ampAttackLabel, ampDecayLabel, ampSustainLabel, ampReleaseLabel;
    juce::Label filtAttackLabel, filtDecayLabel, filtSustainLabel, filtReleaseLabel;
    
    // === FILTER TAB ===
    juce::Slider filterCutoffSlider, filterResSlider, filterDriveSlider;
    juce::ComboBox filterTypeBox;
    juce::Label filterCutoffLabel, filterResLabel, filterDriveLabel, filterTypeLabel;
    
    // === LFO TAB ===
    juce::Slider lfo1RateSlider, lfo2RateSlider;
    juce::ComboBox lfo1WaveBox, lfo2WaveBox;
    juce::Label lfo1RateLabel, lfo1WaveLabel, lfo2RateLabel, lfo2WaveLabel;
    
    // === MODULATION TAB ===
    juce::ComboBox modSourceBox[4], modDestBox[4];
    juce::Slider modAmountSlider[4];
    juce::Label modSourceLabel[4], modDestLabel[4], modAmountLabel[4];
    
    // === EFFECTS TAB ===
    juce::ToggleButton distEnableBtn, satEnableBtn, chorusEnableBtn;
    juce::ToggleButton delayEnableBtn, reverbEnableBtn, phaserEnableBtn;
    juce::Slider distDriveSlider, distMixSlider;
    juce::Slider satDriveSlider, satMixSlider;
    juce::Slider chorusRateSlider, chorusDepthSlider, chorusMixSlider;
    juce::Slider delayTimeSlider, delayFeedbackSlider, delayMixSlider;
    juce::Slider reverbSizeSlider, reverbDampSlider, reverbMixSlider;
    juce::Slider phaserRateSlider, phaserDepthSlider, phaserMixSlider;
    juce::Slider widthSlider;
    juce::Label distDriveLabel, distMixLabel;
    juce::Label satDriveLabel, satMixLabel;
    juce::Label chorusRateLabel, chorusDepthLabel, chorusMixLabel;
    juce::Label delayTimeLabel, delayFeedbackLabel, delayMixLabel;
    juce::Label reverbSizeLabel, reverbDampLabel, reverbMixLabel;
    juce::Label phaserRateLabel, phaserDepthLabel, phaserMixLabel;
    juce::Label widthLabel;
    
    // === MASTER TAB ===
    juce::Slider volumeSlider, unisonVoicesSlider, detuneSlider;
    juce::ToggleButton portamentoToggle;
    juce::Slider portamentoTimeSlider;
    juce::Label volumeLabel, unisonVoicesLabel, detuneLabel;
    juce::Label portamentoLabel, portamentoTimeLabel;
    
    // === KEYBOARD ===
    juce::MidiKeyboardState keyboardState;
    PianoKeyboard keyboard;
    
    // === ATTACHMENTS ===
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    
    std::vector<std::unique_ptr<SliderAttachment>> sliderAttachments;
    std::vector<std::unique_ptr<ComboBoxAttachment>> comboAttachments;
    std::vector<std::unique_ptr<ButtonAttachment>> buttonAttachments;
    
    void createOscillatorTab(juce::Component& tab);
    void createEnvelopeTab(juce::Component& tab);
    void createFilterTab(juce::Component& tab);
    void createLFOTab(juce::Component& tab);
    void createModulationTab(juce::Component& tab);
    void createEffectsTab(juce::Component& tab);
    void createMasterTab(juce::Component& tab);
    
    void configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& text);
    void configureToggle(juce::ToggleButton& button, const juce::String& text);
    void addSliderAttachment(juce::Slider& slider, const juce::String& paramID);
    void addComboAttachment(juce::ComboBox& combo, const juce::String& paramID);
    void addButtonAttachment(juce::ToggleButton& button, const juce::String& paramID);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeSynthAudioProcessorEditor)
};
