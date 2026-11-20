#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeSynthAudioProcessorEditor::HuntericeSynthAudioProcessorEditor(HuntericeSynthAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    setSize(1200, 900);
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
        label.setFont(juce::FontOptions(10.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
    };

    // ... [Oscillator, Sub, Envelopes, LFOs, Filter, Unison, Portamento секции аналогичны предыдущим версиям] ...
    
    // Для краткости показываю только Modulation Matrix и Master
    
    // Modulation Matrix (4 слота)
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
    }

    // Master Volume
    configureSlider(volumeSlider, volumeLabel, "Volume");

    // Attachments
    // ... [все предыдущие attachments] ...
    
    // Modulation Matrix attachments
    for (int i = 0; i < 4; ++i)
    {
        modSourceAttachment[i] = std::make_unique<ComboBoxAttachment>(
            processorRef.getAPVTS(), "MOD_SRC_" + std::to_string(i), modSourceBox[i]);
        modDestAttachment[i] = std::make_unique<ComboBoxAttachment>(
            processorRef.getAPVTS(), "MOD_DST_" + std::to_string(i), modDestBox[i]);
        modAmountAttachment[i] = std::make_unique<SliderAttachment>(
            processorRef.getAPVTS(), "MOD_AMT_" + std::to_string(i), modAmountSlider[i]);
    }
    
    volumeAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "VOLUME", volumeSlider);
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

    // ... [предыдущие секции layout] ...
    
    // Modulation Matrix (последняя строка)
    auto modRow = bounds.reduced(10);
    int modWidth = modRow.getWidth() / 4;
    
    for (int i = 0; i < 4; ++i)
    {
        auto cell = modRow.removeFromLeft(modWidth).reduced(5);
        modSourceLabel[i].setBounds(cell.removeFromTop(15));
        modSourceBox[i].setBounds(cell.removeFromTop(25));
        modDestLabel[i].setBounds(cell.removeFromTop(15));
        modDestBox[i].setBounds(cell.removeFromTop(25));
        modAmountLabel[i].setBounds(cell.removeFromTop(15));
        modAmountSlider[i].setBounds(cell.withHeight(100));
    }
}
