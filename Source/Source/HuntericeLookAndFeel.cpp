#include "HuntericeLookAndFeel.h"

const juce::Colour HuntericeLookAndFeel::bgBlack = juce::Colour(0xFF0A0A0A);
const juce::Colour HuntericeLookAndFeel::bgGraphite = juce::Colour(0xFF1A1A1A);
const juce::Colour HuntericeLookAndFeel::bgGraphiteLight = juce::Colour(0xFF2A2A2A);
const juce::Colour HuntericeLookAndFeel::silver = juce::Colour(0xFF8A8A8A);
const juce::Colour HuntericeLookAndFeel::silverLight = juce::Colour(0xFFB0B0B0);
const juce::Colour HuntericeLookAndFeel::darkRed = juce::Colour(0xFF8B0000);
const juce::Colour HuntericeLookAndFeel::coldWhite = juce::Colour(0xFFE0E0E0);
const juce::Colour HuntericeLookAndFeel::accentRed = juce::Colour(0xFFA52A2A);

HuntericeLookAndFeel::HuntericeLookAndFeel()
{
    // Устанавливаем глобальные цвета
    setColour(juce::ResizableWindow::backgroundColourId, bgBlack);
    setColour(juce::Label::textColourId, coldWhite);
    setColour(juce::ComboBox::backgroundColourId, bgGraphite);
    setColour(juce::ComboBox::textColourId, coldWhite);
    setColour(juce::ComboBox::arrowColourId, silver);
    setColour(juce::TextEditor::backgroundColourId, bgGraphite);
    setColour(juce::TextEditor::textColourId, coldWhite);
    setColour(juce::TextEditor::highlightColourId, darkRed);
}

void HuntericeLookAndFeel::drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float rotaryStartAngle, float rotaryEndAngle,
                                           juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    auto radius = juce::jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto centreX = bounds.getCentreX();
    auto centreY = bounds.getCentreY();
    auto rx = centreX - radius;
    auto ry = centreY - radius;
    auto rw = radius * 2.0f;
    auto angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    
    // Фон (тёмный круг)
    g.setColour(bgGraphite);
    g.fillEllipse(rx, ry, rw, rw);
    
    // Внешнее кольцо (серебристое)
    g.setColour(silver);
    g.drawEllipse(rx, ry, rw, rw, 2.0f);
    
    // Индикатор (красная дуга)
    juce::Path valueArc;
    valueArc.addCentredArc(centreX, centreY, radius * 0.8f, radius * 0.8f,
                          0.0f, rotaryStartAngle, angle, true);
    g.setColour(accentRed);
    g.strokePath(valueArc, juce::PathStrokeType(3.0f));
    
    // Указатель (линия)
    juce::Path pointer;
    auto pointerLength = radius * 0.6f;
    auto pointerThickness = 3.0f;
    pointer.addRoundedRectangle(-pointerThickness * 0.5f, -radius * 0.85f, pointerThickness, pointerLength, 1.0f);
    pointer.applyTransform(juce::AffineTransform::rotation(angle).translated(centreX, centreY));
    
    g.setColour(coldWhite);
    g.fillPath(pointer);
    
    // Центральная точка
    g.setColour(silver);
    g.fillEllipse(centreX - 3, centreY - 3, 6, 6);
}

void HuntericeLookAndFeel::drawToggleButton(juce::Graphics& g, juce::ToggleButton& button,
                                           bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    auto bounds = button.getLocalBounds().toFloat();
    
    // Фон кнопки
    g.setColour(button.getToggleState() ? darkRed : bgGraphite);
    g.fillRoundedRectangle(bounds, 4.0f);
    
    // Граница
    g.setColour(shouldDrawButtonAsHighlighted ? silverLight : silver);
    g.drawRoundedRectangle(bounds, 4.0f, 1.5f);
    
    // Текст
    g.setColour(coldWhite);
    g.setFont(12.0f);
    g.drawFittedText(button.getButtonText(), bounds.reduced(4).toNearestInt(),
                    juce::Justification::centred, 2);
}

void HuntericeLookAndFeel::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                       int buttonX, int buttonY, int buttonW, int buttonH,
                                       juce::ComboBox& box)
{
    auto bounds = box.getLocalBounds().toFloat();
    
    // Фон
    g.setColour(bgGraphite);
    g.fillRoundedRectangle(bounds, 4.0f);
    
    // Граница
    g.setColour(isButtonDown ? accentRed : silver);
    g.drawRoundedRectangle(bounds, 4.0f, 1.5f);
    
    // Стрелка
    juce::Path arrow;
    auto arrowX = static_cast<float>(buttonX) + buttonW * 0.5f;
    auto arrowY = static_cast<float>(buttonY) + buttonH * 0.5f;
    arrow.addTriangle(arrowX - 4.0f, arrowY - 2.0f,
                     arrowX + 4.0f, arrowY - 2.0f,
                     arrowX, arrowY + 3.0f);
    g.setColour(silver);
    g.fillPath(arrow);
}

void HuntericeLookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
{
    label.setBounds(4, 0, box.getWidth() - 24, box.getHeight());
    label.setFont(getLabelFont(label));
}

void HuntericeLookAndFeel::drawLinearSlider(juce::Graphics& g, int x, int y, int width, int height,
                                           float sliderPos, float minSliderPos, float maxSliderPos,
                                           const juce::Slider::SliderStyle style, juce::Slider& slider)
{
    auto bounds = juce::Rectangle<int>(x, y, width, height).toFloat();
    
    // Фон
    g.setColour(bgGraphite);
    g.fillRoundedRectangle(bounds, 2.0f);
    
    // Заполнение
    auto fillBounds = (slider.isHorizontal()) ?
        bounds.withWidth(sliderPos - x) :
        bounds.withY(sliderPos).withHeight(height - (sliderPos - y));
    
    g.setColour(accentRed);
    g.fillRoundedRectangle(fillBounds, 2.0f);
    
    // Граница
    g.setColour(silver);
    g.drawRoundedRectangle(bounds, 2.0f, 1.0f);
}

juce::Font HuntericeLookAndFeel::getLabelFont(juce::Label& label)
{
    return juce::Font(12.0f, juce::Font::bold);
}
