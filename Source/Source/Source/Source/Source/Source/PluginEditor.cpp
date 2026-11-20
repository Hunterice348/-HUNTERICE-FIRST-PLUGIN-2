#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeSynthAudioProcessorEditor::HuntericeSynthAudioProcessorEditor(HuntericeSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p), keyboard(keyboardState, juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setLookAndFeel(&lookAndFeel);
    setSize(1200, 800);
    setResizable(true, true);
    setResizeLimits(1000, 700, 1600, 1000);
    
    // === HEADER ===
    titleLabel.setText("HUNTERICE", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(48.0f, juce::Font::bold));
    titleLabel.setColour(juce::Label::textColourId, HuntericeLookAndFeel::accentRed);
    addAndMakeVisible(titleLabel);
    
    subtitleLabel.setText("SYNTHESIZER", juce::dontSendNotification);
    subtitleLabel.setFont(juce::FontOptions(18.0f));
    subtitleLabel.setColour(juce::Label::textColourId, HuntericeLookAndFeel::silver);
    addAndMakeVisible(subtitleLabel);
    
    // === METERS ===
    inputLabel.setText("IN", juce::dontSendNotification);
    inputLabel.setColour(juce::Label::textColourId, HuntericeLookAndFeel::coldWhite);
    addAndMakeVisible(inputLabel);
    addAndMakeVisible(inputMeterL);
    addAndMakeVisible(inputMeterR);
    
    outputLabel.setText("OUT", juce::dontSendNotification);
    outputLabel.setColour(juce::Label::textColourId, HuntericeLookAndFeel::coldWhite);
    addAndMakeVisible(outputLabel);
    addAndMakeVisible(outputMeterL);
    addAndMakeVisible(outputMeterR);
    
    // === TABS ===
    addAndMakeVisible(tabs);
    tabs.setTabBarDepth(1);
    
    // Создаём табы
    auto* oscTab = new juce::Component();
    createOscillatorTab(*oscTab);
    tabs.addTab("OSCILLATOR", HuntericeLookAndFeel::bgGraphite, oscTab, true);
    
    auto* envTab = new juce::Component();
    createEnvelopeTab(*envTab);
    tabs.addTab("ENVELOPE", HuntericeLookAndFeel::bgGraphite, envTab, true);
    
    auto* filtTab = new juce::Component();
    createFilterTab(*filtTab);
    tabs.addTab("FILTER", HuntericeLookAndFeel::bgGraphite, filtTab, true);
    
    auto* lfoTab = new juce::Component();
    createLFOTab(*lfoTab);
    tabs.addTab("LFO", HuntericeLookAndFeel::bgGraphite, lfoTab, true);
    
    auto* modTab = new juce::Component();
    createModulationTab(*modTab);
    tabs.addTab("MODULATION", HuntericeLookAndFeel::bgGraphite, modTab, true);
    
    auto* fxTab = new juce::Component();
    createEffectsTab(*fxTab);
    tabs.addTab("EFFECTS", HuntericeLookAndFeel::bgGraphite, fxTab, true);
    
    auto* masterTab = new juce::Component();
    createMasterTab(*masterTab);
    tabs.addTab("MASTER", HuntericeLookAndFeel::bgGraphite, masterTab, true);
    
    // === KEYBOARD ===
    addAndMakeVisible(keyboard);
}

HuntericeSynthAudioProcessorEditor::~HuntericeSynthAudioProcessorEditor()
{
    setLookAndFeel(nullptr);
}

void HuntericeSynthAudioProcessorEditor::configureSlider(juce::Slider& slider, juce::Label& label, const juce::String& text)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
    addAndMakeVisible(slider);
    
    label.setText(text, juce::dontSendNotification);
    label.setFont(juce::FontOptions(11.0f, juce::Font::bold));
    label.setJustificationType(juce::Justification::centred);
    label.setColour(juce::Label::textColourId, HuntericeLookAndFeel::silverLight);
    addAndMakeVisible(label);
}

void HuntericeSynthAudioProcessorEditor::configureToggle(juce::ToggleButton& button, const juce::String& text)
{
    button.setButtonText(text);
    addAndMakeVisible(button);
}

void HuntericeSynthAudioProcessorEditor::addSliderAttachment(juce::Slider& slider, const juce::String& paramID)
{
    sliderAttachments.push_back(std::make_unique<SliderAttachment>(processorRef.getAPVTS(), paramID, slider));
}

void HuntericeSynthAudioProcessorEditor::addComboAttachment(juce::ComboBox& combo, const juce::String& paramID)
{
    comboAttachments.push_back(std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), paramID, combo));
}

void HuntericeSynthAudioProcessorEditor::addButtonAttachment(juce::ToggleButton& button, const juce::String& paramID)
{
    buttonAttachments.push_back(std::make_unique<ButtonAttachment>(processorRef.getAPVTS(), paramID, button));
}

void HuntericeSynthAudioProcessorEditor::createOscillatorTab(juce::Component& tab)
{
    configureSlider(pulseWidthSlider, pulseWidthLabel, "Pulse Width");
    configureSlider(wavetablePosSlider, wavetablePosLabel, "WT Position");
    configureSlider(subOctaveSlider, subOctaveLabel, "Sub Octave");
    configureSlider(subMixSlider, subMixLabel, "Sub Mix");
    
    oscTypeBox.addItem("Sine", 1);
    oscTypeBox.addItem("Triangle", 2);
    oscTypeBox.addItem("Saw", 3);
    oscTypeBox.addItem("Square", 4);
    oscTypeBox.addItem("Pulse", 5);
    oscTypeBox.addItem("Noise", 6);
    oscTypeBox.addItem("Sub", 7);
    oscTypeBox.addItem("Wavetable", 8);
    oscTypeBox.setSelectedId(1);
    addAndMakeVisible(oscTypeBox);
    oscTypeLabel.setText("Oscillator Type", juce::dontSendNotification);
    addAndMakeVisible(oscTypeLabel);
    
    subTypeBox.addItem("Sine", 1);
    subTypeBox.addItem("Triangle", 2);
    subTypeBox.addItem("Saw", 3);
    subTypeBox.addItem("Square", 4);
    subTypeBox.setSelectedId(4);
    addAndMakeVisible(subTypeBox);
    subTypeLabel.setText("Sub Type", juce::dontSendNotification);
    addAndMakeVisible(subTypeLabel);
    
    tab.addAndMakeVisible(oscTypeBox);
    tab.addAndMakeVisible(oscTypeLabel);
    tab.addAndMakeVisible(pulseWidthSlider);
    tab.addAndMakeVisible(pulseWidthLabel);
    tab.addAndMakeVisible(wavetablePosSlider);
    tab.addAndMakeVisible(wavetablePosLabel);
    tab.addAndMakeVisible(subTypeBox);
    tab.addAndMakeVisible(subTypeLabel);
    tab.addAndMakeVisible(subOctaveSlider);
    tab.addAndMakeVisible(subOctaveLabel);
    tab.addAndMakeVisible(subMixSlider);
    tab.addAndMakeVisible(subMixLabel);
    
    addComboAttachment(oscTypeBox, "OSC_TYPE");
    addSliderAttachment(pulseWidthSlider, "PULSE_WIDTH");
    addSliderAttachment(wavetablePosSlider, "WT_POS");
    addComboAttachment(subTypeBox, "SUB_TYPE");
    addSliderAttachment(subOctaveSlider, "SUB_OCT");
    addSliderAttachment(subMixSlider, "SUB_MIX");
}

void HuntericeSynthAudioProcessorEditor::createEnvelopeTab(juce::Component& tab)
{
    configureSlider(ampAttackSlider, ampAttackLabel, "Attack");
    configureSlider(ampDecaySlider, ampDecayLabel, "Decay");
    configureSlider(ampSustainSlider, ampSustainLabel, "Sustain");
    configureSlider(ampReleaseSlider, ampReleaseLabel, "Release");
    
    configureSlider(filtAttackSlider, filtAttackLabel, "Attack");
    configureSlider(filtDecaySlider, filtDecayLabel, "Decay");
    configureSlider(filtSustainSlider, filtSustainLabel, "Sustain");
    configureSlider(filtReleaseSlider, filtReleaseLabel, "Release");
    
    tab.addAndMakeVisible(ampAttackSlider);
    tab.addAndMakeVisible(ampAttackLabel);
    tab.addAndMakeVisible(ampDecaySlider);
    tab.addAndMakeVisible(ampDecayLabel);
    tab.addAndMakeVisible(ampSustainSlider);
    tab.addAndMakeVisible(ampSustainLabel);
    tab.addAndMakeVisible(ampReleaseSlider);
    tab.addAndMakeVisible(ampReleaseLabel);
    
    tab.addAndMakeVisible(filtAttackSlider);
    tab.addAndMakeVisible(filtAttackLabel);
    tab.addAndMakeVisible(filtDecaySlider);
    tab.addAndMakeVisible(filtDecayLabel);
    tab.addAndMakeVisible(filtSustainSlider);
    tab.addAndMakeVisible(filtSustainLabel);
    tab.addAndMakeVisible(filtReleaseSlider);
    tab.addAndMakeVisible(filtReleaseLabel);
    
    addSliderAttachment(ampAttackSlider, "AMP_ATTACK");
    addSliderAttachment(ampDecaySlider, "AMP_DECAY");
    addSliderAttachment(ampSustainSlider, "AMP_SUSTAIN");
    addSliderAttachment(ampReleaseSlider, "AMP_RELEASE");
    
    addSliderAttachment(filtAttackSlider, "FILT_ATTACK");
    addSliderAttachment(filtDecaySlider, "FILT_DECAY");
    addSliderAttachment(filtSustainSlider, "FILT_SUSTAIN");
    addSliderAttachment(filtReleaseSlider, "FILT_RELEASE");
}

void HuntericeSynthAudioProcessorEditor::createFilterTab(juce::Component& tab)
{
    configureSlider(filterCutoffSlider, filterCutoffLabel, "Cutoff");
    configureSlider(filterResSlider, filterResLabel, "Resonance");
    configureSlider(filterDriveSlider, filterDriveLabel, "Drive");
    
    filterTypeBox.addItem("Low-Pass", 1);
    filterTypeBox.addItem("High-Pass", 2);
    filterTypeBox.addItem("Band-Pass", 3);
    filterTypeBox.addItem("Ladder (Moog)", 4);
    filterTypeBox.addItem("Resonant (SVF)", 5);
    filterTypeBox.setSelectedId(1);
    addAndMakeVisible(filterTypeBox);
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    addAndMakeVisible(filterTypeLabel);
    
    tab.addAndMakeVisible(filterCutoffSlider);
    tab.addAndMakeVisible(filterCutoffLabel);
    tab.addAndMakeVisible(filterResSlider);
    tab.addAndMakeVisible(filterResLabel);
    tab.addAndMakeVisible(filterDriveSlider);
    tab.addAndMakeVisible(filterDriveLabel);
    tab.addAndMakeVisible(filterTypeBox);
    tab.addAndMakeVisible(filterTypeLabel);
    
    addSliderAttachment(filterCutoffSlider, "FILTER_CUTOFF");
    addSliderAttachment(filterResSlider, "FILTER_RES");
    addSliderAttachment(filterDriveSlider, "FILTER_DRIVE");
    addComboAttachment(filterTypeBox, "FILTER_TYPE");
}

void HuntericeSynthAudioProcessorEditor::createLFOTab(juce::Component& tab)
{
    configureSlider(lfo1RateSlider, lfo1RateLabel, "Rate");
    configureSlider(lfo2RateSlider, lfo2RateLabel, "Rate");
    
    lfo1WaveBox.addItem("Sine", 1);
    lfo1WaveBox.addItem("Triangle", 2);
    lfo1WaveBox.addItem("Saw Up", 3);
    lfo1WaveBox.addItem("Saw Down", 4);
    lfo1WaveBox.addItem("Square", 5);
    lfo1WaveBox.addItem("Random", 6);
    lfo1WaveBox.addItem("S&H", 7);
    lfo1WaveBox.setSelectedId(1);
    addAndMakeVisible(lfo1WaveBox);
    lfo1WaveLabel.setText("Waveform", juce::dontSendNotification);
    addAndMakeVisible(lfo1WaveLabel);
    
    lfo2WaveBox.addItem("Sine", 1);
    lfo2WaveBox.addItem("Triangle", 2);
    lfo2WaveBox.addItem("Saw Up", 3);
    lfo2WaveBox.addItem("Saw Down", 4);
    lfo2WaveBox.addItem("Square", 5);
    lfo2WaveBox.addItem("Random", 6);
    lfo2WaveBox.addItem("S&H", 7);
    lfo2WaveBox.setSelectedId(1);
    addAndMakeVisible(lfo2WaveBox);
    lfo2WaveLabel.setText("Waveform", juce::dontSendNotification);
    addAndMakeVisible(lfo2WaveLabel);
    
    tab.addAndMakeVisible(lfo1RateSlider);
    tab.addAndMakeVisible(lfo1RateLabel);
    tab.addAndMakeVisible(lfo1WaveBox);
    tab.addAndMakeVisible(lfo1WaveLabel);
    
    tab.addAndMakeVisible(lfo2RateSlider);
    tab.addAndMakeVisible(lfo2RateLabel);
    tab.addAndMakeVisible(lfo2WaveBox);
    tab.addAndMakeVisible(lfo2WaveLabel);
    
    addSliderAttachment(lfo1RateSlider, "LFO1_RATE");
    addComboAttachment(lfo1WaveBox, "LFO1_WAVE");
    addSliderAttachment(lfo2RateSlider, "LFO2_RATE");
    addComboAttachment(lfo2WaveBox, "LFO2_WAVE");
}

void HuntericeSynthAudioProcessorEditor::createModulationTab(juce::Component& tab)
{
    for (int i = 0; i < 4; ++i)
    {
        modSourceBox[i].addItem("Env1", 1);
        modSourceBox[i].addItem("Env2", 2);
        modSourceBox[i].addItem("LFO1", 3);
        modSourceBox[i].addItem("LFO2", 4);
        modSourceBox[i].addItem("Velocity", 5);
        modSourceBox[i].addItem("KeyTrack", 6);
        modSourceBox[i].addItem("ModWheel", 7);
        modSourceBox[i].setSelectedId(1);
        addAndMakeVisible(modSourceBox[i]);
        
        modDestBox[i].addItem("Pitch", 1);
        modDestBox[i].addItem("Filter", 2);
        modDestBox[i].addItem("PWM", 3);
        modDestBox[i].addItem("Amp", 4);
        modDestBox[i].addItem("Detune", 5);
        modDestBox[i].setSelectedId(1);
        addAndMakeVisible(modDestBox[i]);
        
        configureSlider(modAmountSlider[i], modAmountLabel[i], "Amount " + juce::String(i + 1));
        
        tab.addAndMakeVisible(modSourceBox[i]);
        tab.addAndMakeVisible(modDestBox[i]);
        tab.addAndMakeVisible(modAmountSlider[i]);
        tab.addAndMakeVisible(modAmountLabel[i]);
        
        addComboAttachment(modSourceBox[i], "MOD_SRC_" + juce::String(i));
        addComboAttachment(modDestBox[i], "MOD_DST_" + juce::String(i));
        addSliderAttachment(modAmountSlider[i], "MOD_AMT_" + juce::String(i));
    }
}

void HuntericeSynthAudioProcessorEditor::createEffectsTab(juce::Component& tab)
{
    // Distortion
    configureToggle(distEnableBtn, "DISTORTION");
    configureSlider(distDriveSlider, distDriveLabel, "Drive");
    configureSlider(distMixSlider, distMixLabel, "Mix");
    
    // Saturation
    configureToggle(satEnableBtn, "SATURATION");
    configureSlider(satDriveSlider, satDriveLabel, "Drive");
    configureSlider(satMixSlider, satMixLabel, "Mix");
    
    // Chorus
    configureToggle(chorusEnableBtn, "CHORUS");
    configureSlider(chorusRateSlider, chorusRateLabel, "Rate");
    configureSlider(chorusDepthSlider, chorusDepthLabel, "Depth");
    configureSlider(chorusMixSlider, chorusMixLabel, "Mix");
    
    // Delay
    configureToggle(delayEnableBtn, "DELAY");
    configureSlider(delayTimeSlider, delayTimeLabel, "Time");
    configureSlider(delayFeedbackSlider, delayFeedbackLabel, "Feedback");
    configureSlider(delayMixSlider, delayMixLabel, "Mix");
    
    // Reverb
    configureToggle(reverbEnableBtn, "REVERB");
    configureSlider(reverbSizeSlider, reverbSizeLabel, "Size");
    configureSlider(reverbDampSlider, reverbDampLabel, "Damp");
    configureSlider(reverbMixSlider, reverbMixLabel, "Mix");
    
    // Phaser
    configureToggle(phaserEnableBtn, "PHASER");
    configureSlider(phaserRateSlider, phaserRateLabel, "Rate");
    configureSlider(phaserDepthSlider, phaserDepthLabel, "Depth");
    configureSlider(phaserMixSlider, phaserMixLabel, "Mix");
    
    // Width
    configureSlider(widthSlider, widthLabel, "Stereo Width");
    
    // Добавляем все в tab
    tab.addAndMakeVisible(distEnableBtn);
    tab.addAndMakeVisible(distDriveSlider);
    tab.addAndMakeVisible(distDriveLabel);
    tab.addAndMakeVisible(distMixSlider);
    tab.addAndMakeVisible(distMixLabel);
    
    tab.addAndMakeVisible(satEnableBtn);
    tab.addAndMakeVisible(satDriveSlider);
    tab.addAndMakeVisible(satDriveLabel);
    tab.addAndMakeVisible(satMixSlider);
    tab.addAndMakeVisible(satMixLabel);
    
    tab.addAndMakeVisible(chorusEnableBtn);
    tab.addAndMakeVisible(chorusRateSlider);
    tab.addAndMakeVisible(chorusRateLabel);
    tab.addAndMakeVisible(chorusDepthSlider);
    tab.addAndMakeVisible(chorusDepthLabel);
    tab.addAndMakeVisible(chorusMixSlider);
    tab.addAndMakeVisible(chorusMixLabel);
    
    tab.addAndMakeVisible(delayEnableBtn);
    tab.addAndMakeVisible(delayTimeSlider);
    tab.addAndMakeVisible(delayTimeLabel);
    tab.addAndMakeVisible(delayFeedbackSlider);
    tab.addAndMakeVisible(delayFeedbackLabel);
    tab.addAndMakeVisible(delayMixSlider);
    tab.addAndMakeVisible(delayMixLabel);
    
    tab.addAndMakeVisible(reverbEnableBtn);
    tab.addAndMakeVisible(reverbSizeSlider);
    tab.addAndMakeVisible(reverbSizeLabel);
    tab.addAndMakeVisible(reverbDampSlider);
    tab.addAndMakeVisible(reverbDampLabel);
    tab.addAndMakeVisible(reverbMixSlider);
    tab.addAndMakeVisible(reverbMixLabel);
    
    tab.addAndMakeVisible(phaserEnableBtn);
    tab.addAndMakeVisible(phaserRateSlider);
    tab.addAndMakeVisible(phaserRateLabel);
    tab.addAndMakeVisible(phaserDepthSlider);
    tab.addAndMakeVisible(phaserDepthLabel);
    tab.addAndMakeVisible(phaserMixSlider);
    tab.addAndMakeVisible(phaserMixLabel);
    
    tab.addAndMakeVisible(widthSlider);
    tab.addAndMakeVisible(widthLabel);
    
    // Attachments
    addButtonAttachment(distEnableBtn, "DIST_ENABLED");
    addSliderAttachment(distDriveSlider, "DIST_DRIVE");
    addSliderAttachment(distMixSlider, "DIST_MIX");
    
    addButtonAttachment(satEnableBtn, "SAT_ENABLED");
    addSliderAttachment(satDriveSlider, "SAT_DRIVE");
    addSliderAttachment(satMixSlider, "SAT_MIX");
    
    addButtonAttachment(chorusEnableBtn, "CHORUS_ENABLED");
    addSliderAttachment(chorusRateSlider, "CHORUS_RATE");
    addSliderAttachment(chorusDepthSlider, "CHORUS_DEPTH");
    addSliderAttachment(chorusMixSlider, "CHORUS_MIX");
    
    addButtonAttachment(delayEnableBtn, "DELAY_ENABLED");
    addSliderAttachment(delayTimeSlider, "DELAY_TIME");
    addSliderAttachment(delayFeedbackSlider, "DELAY_FEEDBACK");
    addSliderAttachment(delayMixSlider, "DELAY_MIX");
    
    addButtonAttachment(reverbEnableBtn, "REVERB_ENABLED");
    addSliderAttachment(reverbSizeSlider, "REVERB_SIZE");
    addSliderAttachment(reverbDampSlider, "REVERB_DAMP");
    addSliderAttachment(reverbMixSlider, "REVERB_MIX");
    
    addButtonAttachment(phaserEnableBtn, "PHASER_ENABLED");
    addSliderAttachment(phaserRateSlider, "PHASER_RATE");
    addSliderAttachment(phaserDepthSlider, "PHASER_DEPTH");
    addSliderAttachment(phaserMixSlider, "PHASER_MIX");
    
    addSliderAttachment(widthSlider, "STEREO_WIDTH");
}

void HuntericeSynthAudioProcessorEditor::createMasterTab(juce::Component& tab)
{
    configureSlider(volumeSlider, volumeLabel, "Volume");
    configureSlider(unisonVoicesSlider, unisonVoicesLabel, "Unison Voices");
    configureSlider(detuneSlider, detuneLabel, "Detune");
    configureSlider(portamentoTimeSlider, portamentoTimeLabel, "Time");
    configureToggle(portamentoToggle, "PORTAMENTO");
    
    tab.addAndMakeVisible(volumeSlider);
    tab.addAndMakeVisible(volumeLabel);
    tab.addAndMakeVisible(unisonVoicesSlider);
    tab.addAndMakeVisible(unisonVoicesLabel);
    tab.addAndMakeVisible(detuneSlider);
    tab.addAndMakeVisible(detuneLabel);
    tab.addAndMakeVisible(portamentoToggle);
    tab.addAndMakeVisible(portamentoTimeSlider);
    tab.addAndMakeVisible(portamentoTimeLabel);
    
    addSliderAttachment(volumeSlider, "VOLUME");
    addSliderAttachment(unisonVoicesSlider, "UNISON_VOICES");
    addSliderAttachment(detuneSlider, "DETUNE");
    addButtonAttachment(portamentoToggle, "PORTA_ENABLED");
    addSliderAttachment(portamentoTimeSlider, "PORTA_TIME");
}

void HuntericeSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Фон
    g.fillAll(HuntericeLookAndFeel::bgBlack);
    
    // Декоративные линии
    g.setColour(HuntericeLookAndFeel::darkRed);
    g.drawLine(20.0f, 80.0f, static_cast<float>(getWidth() - 20), 80.0f, 2.0f);
    
    // Рамка
    g.setColour(HuntericeLookAndFeel::silver);
    g.drawRect(getLocalBounds(), 2);
}

void HuntericeSynthAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    
    // Header
    titleLabel.setBounds(20, 10, 300, 50);
    subtitleLabel.setBounds(20, 50, 300, 25);
    
    // Meters
    inputLabel.setBounds(getWidth() - 200, 10, 40, 20);
    inputMeterL.setBounds(getWidth() - 160, 10, 20, 60);
    inputMeterR.setBounds(getWidth() - 130, 10, 20, 60);
    
    outputLabel.setBounds(getWidth() - 100, 10, 40, 20);
    outputMeterL.setBounds(getWidth() - 60, 10, 20, 60);
    outputMeterR.setBounds(getWidth() - 30, 10, 20, 60);
    
    // Tabs
    tabs.setBounds(20, 90, getWidth() - 40, getHeight() - 200);
    
    // Keyboard
    keyboard.setBounds(20, getHeight() - 100, getWidth() - 40, 80);
    
    // Layout для каждого таба
    for (int i = 0; i < tabs.getNumTabs(); ++i)
    {
        if (auto* tab = tabs.getTabContentComponent(i))
        {
            auto tabBounds = tab->getLocalBounds().reduced(20);
            
            if (i == 0) // Oscillator
            {
                int sliderW = tabBounds.getWidth() / 6;
                tabBounds.removeFromTop(20);
                
                oscTypeLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                oscTypeBox.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(40));
                
                pulseWidthLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                pulseWidthSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                
                wavetablePosLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                wavetablePosSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                
                subTypeLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                subTypeBox.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(40));
                
                subOctaveLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                subOctaveSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
            }
            else if (i == 1) // Envelope
            {
                int sliderW = tabBounds.getWidth() / 8;
                tabBounds.removeFromTop(20);
                
                // Amp Envelope
                ampAttackLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                ampAttackSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                ampDecayLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                ampDecaySlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                ampSustainLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                ampSustainSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                ampReleaseLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                ampReleaseSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                
                // Filter Envelope
                filtAttackLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                filtAttackSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                filtDecayLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                filtDecaySlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                filtSustainLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                filtSustainSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
                filtReleaseLabel.setBounds(tabBounds.removeFromLeft(sliderW).removeFromTop(20));
                filtReleaseSlider.setBounds(tabBounds.removeFromLeft(sliderW).withHeight(120));
            }
            // ... остальные табы аналогично
        }
    }
}
