#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeSynthAudioProcessorEditor::HuntericeSynthAudioProcessorEditor(HuntericeSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1000, 700);
    setResizable(false, false);

    // Title
    titleLabel.setText("HUNTERICE SYNTH", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    // Helper для слайдеров
    auto configureSlider = [this](juce::Slider& slider, juce::Label& label, const juce::String& text)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 80, 20);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFF4A90E2));
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF3A3A3A));
        slider.setColour(juce::Slider::thumbColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
    };

    // Main Oscillator
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
    oscTypeLabel.setText("Osc Type", juce::dontSendNotification);
    oscTypeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(oscTypeLabel);
    
    configureSlider(pulseWidthSlider, pulseWidthLabel, "Pulse Width");
    configureSlider(wavetablePosSlider, wavetablePosLabel, "WT Position");

    // Sub Oscillator
    subTypeBox.addItem("Sine", 1);
    subTypeBox.addItem("Triangle", 2);
    subTypeBox.addItem("Saw", 3);
    subTypeBox.addItem("Square", 4);
    subTypeBox.addItem("Pulse", 5);
    subTypeBox.addItem("Noise", 6);
    subTypeBox.setSelectedId(4); // Square по умолчанию
    addAndMakeVisible(subTypeBox);
    subTypeLabel.setText("Sub Type", juce::dontSendNotification);
    subTypeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(subTypeLabel);
    
    configureSlider(subOctaveSlider, subOctaveLabel, "Sub Octave");
    configureSlider(subMixSlider, subMixLabel, "Sub Mix");

    // ADSR
    configureSlider(attackSlider, attackLabel, "Attack");
    configureSlider(decaySlider, decayLabel, "Decay");
    configureSlider(sustainSlider, sustainLabel, "Sustain");
    configureSlider(releaseSlider, releaseLabel, "Release");
    configureSlider(volumeSlider, volumeLabel, "Volume");

    // LFO
    configureSlider(lfoRateSlider, lfoRateLabel, "LFO Rate");
    lfoWaveBox.addItem("Sine", 1);
    lfoWaveBox.addItem("Triangle", 2);
    lfoWaveBox.addItem("Saw Up", 3);
    lfoWaveBox.addItem("Saw Down", 4);
    lfoWaveBox.addItem("Square", 5);
    lfoWaveBox.addItem("Random", 6);
    lfoWaveBox.addItem("S&H", 7);
    lfoWaveBox.setSelectedId(1);
    addAndMakeVisible(lfoWaveBox);
    lfoWaveLabel.setText("LFO Wave", juce::dontSendNotification);
    lfoWaveLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(lfoWaveLabel);

    // Filter
    configureSlider(filterCutoffSlider, filterCutoffLabel, "Cutoff");
    configureSlider(filterResSlider, filterResLabel, "Resonance");
    filterTypeBox.addItem("LowPass", 1);
    filterTypeBox.addItem("HighPass", 2);
    filterTypeBox.addItem("BandPass", 3);
    filterTypeBox.addItem("Notch", 4);
    filterTypeBox.addItem("Bypass", 5);
    filterTypeBox.setSelectedId(1);
    addAndMakeVisible(filterTypeBox);
    filterTypeLabel.setText("Filter Type", juce::dontSendNotification);
    filterTypeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(filterTypeLabel);

    // Modulation
    configureSlider(pitchModSlider, pitchModLabel, "LFO→Pitch");
    configureSlider(filterModSlider, filterModLabel, "LFO→Filter");
    configureSlider(ampModSlider, ampModLabel, "LFO→Amp");

    // Attachments
    oscTypeAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "OSC_TYPE", oscTypeBox);
    pulseWidthAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "PULSE_WIDTH", pulseWidthSlider);
    wavetablePosAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "WT_POS", wavetablePosSlider);
    
    subTypeAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "SUB_TYPE", subTypeBox);
    subOctaveAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "SUB_OCT", subOctaveSlider);
    subMixAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "SUB_MIX", subMixSlider);

    attackAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "ATTACK", attackSlider);
    decayAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "DECAY", decaySlider);
    sustainAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "SUSTAIN", sustainSlider);
    releaseAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "RELEASE", releaseSlider);
    volumeAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "VOLUME", volumeSlider);

    lfoRateAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "LFO_RATE", lfoRateSlider);
    lfoWaveAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "LFO_WAVE", lfoWaveBox);

    filterCutoffAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "FILTER_CUTOFF", filterCutoffSlider);
    filterResAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "FILTER_RES", filterResSlider);
    filterTypeAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "FILTER_TYPE", filterTypeBox);

    pitchModAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "MOD_PITCH", pitchModSlider);
    filterModAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "MOD_FILTER", filterModSlider);
    ampModAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "MOD_AMP", ampModSlider);
}

HuntericeSynthAudioProcessorEditor::~HuntericeSynthAudioProcessorEditor() = default;

void HuntericeSynthAudioProcessorEditor::paint(juce::Graphics& g)
{
    juce::ColourGradient gradient(
        juce::Colour(0xFF1A1A2E), 0.0f, 0.0f,
        juce::Colour(0xFF16213E), 0.0f, static_cast<float>(getHeight()),
        false);
    g.setGradientFill(gradient);
    g.fillAll();

    g.setColour(juce::Colour(0xFF4A90E2));
    g.drawLine(20.0f, 60.0f, static_cast<float>(getWidth() - 20), 60.0f, 2.0f);
    g.drawRect(getLocalBounds(), 2);
}

void HuntericeSynthAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();
    titleLabel.setBounds(bounds.removeFromTop(50));
    bounds.removeFromTop(10);

    // Row 1: Main Oscillator
    auto row1 = bounds.removeFromTop(140).reduced(10);
    int sliderW = row1.getWidth() / 8;

    oscTypeLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    oscTypeBox.setBounds(row1.removeFromLeft(sliderW).reduced(5, 25));
    
    pulseWidthLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    pulseWidthSlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));
    
    wavetablePosLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    wavetablePosSlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));
    
    row1.removeFromLeft(sliderW); // space

    // Sub Oscillator
    subTypeLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    subTypeBox.setBounds(row1.removeFromLeft(sliderW).reduced(5, 25));
    
    subOctaveLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    subOctaveSlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));
    
    subMixLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    subMixSlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));

    // Row 2: ADSR
    auto row2 = bounds.removeFromTop(140).reduced(10);
    int sliderW2 = row2.getWidth() / 5;

    attackLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    attackSlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(110));
    decayLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    decaySlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(110));
    sustainLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    sustainSlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(110));
    releaseLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    releaseSlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(110));
    volumeLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    volumeSlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(110));

    // Row 3: LFO + Filter
    auto row3 = bounds.removeFromTop(160).reduced(10);
    int sliderW3 = row3.getWidth() / 6;

    lfoRateLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    lfoRateSlider.setBounds(row3.removeFromLeft(sliderW3).withHeight(130));
    lfoWaveLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    lfoWaveBox.setBounds(row3.removeFromLeft(sliderW3).reduced(5, 25));
    row3.removeFromLeft(sliderW3); // space

    filterCutoffLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    filterCutoffSlider.setBounds(row3.removeFromLeft(sliderW3).withHeight(130));
    filterResLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    filterResSlider.setBounds(row3.removeFromLeft(sliderW3).withHeight(130));
    filterTypeLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    filterTypeBox.setBounds(row3.removeFromLeft(sliderW3).reduced(5, 25));

    // Row 4: Modulation Matrix
    auto row4 = bounds.reduced(10);
    int sliderW4 = row4.getWidth() / 3;

    pitchModLabel.setBounds(row4.removeFromLeft(sliderW4).removeFromTop(20));
    pitchModSlider.setBounds(row4.removeFromLeft(sliderW4).withHeight(130));
    filterModLabel.setBounds(row4.removeFromLeft(sliderW4).removeFromTop(20));
    filterModSlider.setBounds(row4.removeFromLeft(sliderW4).withHeight(130));
    ampModLabel.setBounds(row4.removeFromLeft(sliderW4).removeFromTop(20));
    ampModSlider.setBounds(row4.removeFromLeft(sliderW4).withHeight(130));
}
