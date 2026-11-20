#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeSynthAudioProcessorEditor::HuntericeSynthAudioProcessorEditor(HuntericeSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(900, 600);
    setResizable(false, false);

    // Title
    titleLabel.setText("HUNTERICE SYNTH", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(28.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    // Waveform ComboBox
    waveformBox.addItem("Sine", 1);
    waveformBox.addItem("Saw", 2);
    waveformBox.addItem("Square", 3);
    waveformBox.addItem("Triangle", 4);
    waveformBox.setSelectedId(1);
    addAndMakeVisible(waveformBox);
    waveformLabel.setText("Oscillator", juce::dontSendNotification);
    waveformLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(waveformLabel);

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
        label.setFont(juce::FontOptions(12.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
    };

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
    waveformAttachment = std::make_unique<ComboBoxAttachment>(processorRef.getAPVTS(), "WAVEFORM", waveformBox);
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

    // Row 1: Oscillator + ADSR
    auto row1 = bounds.removeFromTop(140).reduced(10);
    int sliderW = row1.getWidth() / 7;

    waveformLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    waveformBox.setBounds(row1.removeFromLeft(sliderW).reduced(5, 25));
    row1.removeFromLeft(sliderW); // space

    attackLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    attackSlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));
    decayLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    decaySlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));
    sustainLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    sustainSlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));
    releaseLabel.setBounds(row1.removeFromLeft(sliderW).removeFromTop(20));
    releaseSlider.setBounds(row1.removeFromLeft(sliderW).withHeight(110));

    // Row 2: LFO + Filter
    auto row2 = bounds.removeFromTop(160).reduced(10);
    int sliderW2 = row2.getWidth() / 6;

    lfoRateLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    lfoRateSlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(130));
    lfoWaveLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    lfoWaveBox.setBounds(row2.removeFromLeft(sliderW2).reduced(5, 25));
    row2.removeFromLeft(sliderW2); // space

    filterCutoffLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    filterCutoffSlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(130));
    filterResLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    filterResSlider.setBounds(row2.removeFromLeft(sliderW2).withHeight(130));
    filterTypeLabel.setBounds(row2.removeFromLeft(sliderW2).removeFromTop(20));
    filterTypeBox.setBounds(row2.removeFromLeft(sliderW2).reduced(5, 25));

    // Row 3: Modulation Matrix
    auto row3 = bounds.reduced(10);
    int sliderW3 = row3.getWidth() / 4;

    pitchModLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    pitchModSlider.setBounds(row3.removeFromLeft(sliderW3).withHeight(130));
    filterModLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    filterModSlider.setBounds(row3.removeFromLeft(sliderW3).withHeight(130));
    ampModLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    ampModSlider.setBounds(row3.removeFromLeft(sliderW3).withHeight(130));
    volumeLabel.setBounds(row3.removeFromLeft(sliderW3).removeFromTop(20));
    volumeSlider.setBounds(row3.removeFromLeft(sliderW3).withHeight(130));
}
