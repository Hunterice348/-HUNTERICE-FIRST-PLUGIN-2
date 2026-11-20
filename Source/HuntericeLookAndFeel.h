#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

class HuntericeLookAndFeel : public juce::LookAndFeel_V4
{
public:
    HuntericeLookAndFeel();
    
    // Цветовая палитра
    static const juce::Colour bgBlack;
    static const juce::Colour bgGraphite;
    static const juce::Colour bgGraphiteLight;
    static const juce::Colour silver;
    static const juce::Colour silverLight;
    static const juce::Colour darkRed;
    static const juce::Colour coldWhite;
    static const juce::Colour accentRed;
    
    // Переопределения для кастомного стиля
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                         juce::Slider& slider) override;
    
    void drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                         bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
    
    void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                     int buttonX, int buttonY, int buttonW, int buttonH,
                     juce::ComboBox& box) override;
    
    void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override;
    
    void drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                         float sliderPos, float minSliderPos, float maxSliderPos,
                         const juce::Slider::SliderStyle style, juce::Slider& slider) override;
    
    juce::Font getLabelFont(juce::Label& label) override;
    
private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeLookAndFeel)
};
