#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HuntericeFirstPluginAudioProcessorEditor::HuntericeFirstPluginAudioProcessorEditor(
    HuntericeFirstPluginAudioProcessor& p)
    : AudioProcessorEditor(&p), processorRef(p)
{
    // Фиксированный размер окна
    setSize(500, 320);
    setResizable(false, false);

    // --- Заголовок ---
    titleLabel.setText("HUNTERICE FIRST PLUGIN", juce::dontSendNotification);
    titleLabel.setFont(juce::FontOptions(22.0f, juce::Font::bold));
    titleLabel.setJustificationType(juce::Justification::centred);
    titleLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(titleLabel);

    // --- Настройка общего стиля слайдеров ---
    auto configureSlider = [this](juce::Slider& slider, juce::Label& label,
                                  const juce::String& text)
    {
        slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 100, 20);
        slider.setColour(juce::Slider::rotarySliderFillColourId, juce::Colour(0xFFE8A838));
        slider.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF3A3A3A));
        slider.setColour(juce::Slider::thumbColourId, juce::Colour(0xFFFFFFFF));
        slider.setColour(juce::Slider::textBoxTextColourId, juce::Colours::white);
        slider.setColour(juce::Slider::textBoxOutlineColourId, juce::Colours::transparentWhite);
        addAndMakeVisible(slider);

        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::FontOptions(14.0f, juce::Font::bold));
        label.setJustificationType(juce::Justification::centred);
        label.setColour(juce::Label::textColourId, juce::Colours::white);
        addAndMakeVisible(label);
    };

    configureSlider(driveSlider,  driveLabel,  "DRIVE");
    configureSlider(mixSlider,    mixLabel,    "MIX");
    configureSlider(outputSlider, outputLabel, "OUTPUT");

    // --- Привязки к параметрам (автоматизация + state) ---
    driveAttachment  = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "DRIVE",  driveSlider);
    mixAttachment    = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "MIX",    mixSlider);
    outputAttachment = std::make_unique<SliderAttachment>(processorRef.getAPVTS(), "OUTPUT", outputSlider);
}

HuntericeFirstPluginAudioProcessorEditor::~HuntericeFirstPluginAudioProcessorEditor() = default;

//==============================================================================
void HuntericeFirstPluginAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Градиентный фон
    juce::ColourGradient gradient(
        juce::Colour(0xFF1A1A1A), 0.0f, 0.0f,
        juce::Colour(0xFF2D2D2D), 0.0f, static_cast<float>(getHeight()),
        false);
    g.setGradientFill(gradient);
    g.fillAll();

    // Декоративная линия под заголовком
    g.setColour(juce::Colour(0xFFE8A838));
    g.drawLine(20.0f, 55.0f,
               static_cast<float>(getWidth() - 20), 55.0f, 2.0f);

    // Рамка
    g.setColour(juce::Colour(0xFFE8A838));
    g.drawRect(getLocalBounds(), 2);
}

//==============================================================================
void HuntericeFirstPluginAudioProcessorEditor::resized()
{
    auto bounds = getLocalBounds();

    // --- Заголовок ---
    titleLabel.setBounds(bounds.removeFromTop(50));

    // --- Отступ ---
    bounds.removeFromTop(20);

    // --- Область для трёх ручек ---
    auto controlsArea = bounds.reduced(30, 10);
    const int sliderWidth  = controlsArea.getWidth() / 3;
    const int labelHeight  = 28;
    const int sliderHeight = controlsArea.getHeight() - labelHeight;

    // DRIVE
    {
        auto cell = controlsArea.removeFromLeft(sliderWidth);
        driveLabel.setBounds(cell.removeFromTop(labelHeight));
        driveSlider.setBounds(cell.reduced(5));
    }

    // MIX
    {
        auto cell = controlsArea.removeFromLeft(sliderWidth);
        mixLabel.setBounds(cell.removeFromTop(labelHeight));
        mixSlider.setBounds(cell.reduced(5));
    }

    // OUTPUT
    {
        auto cell = controlsArea; // оставшаяся область
        outputLabel.setBounds(cell.removeFromTop(labelHeight));
        outputSlider.setBounds(cell.reduced(5));
    }
}
