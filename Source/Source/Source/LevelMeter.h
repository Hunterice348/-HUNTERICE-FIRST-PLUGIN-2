#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_basics/juce_audio_basics.h>

class LevelMeter : public juce::Component, public juce::Timer
{
public:
    LevelMeter()
    {
        startTimerHz(30); // Обновление 30 раз в секунду
    }
    
    void setLevel(float newLevel)
    {
        currentLevel = juce::jlimit(0.0f, 1.0f, newLevel);
        peakLevel = juce::jmax(peakLevel, currentLevel);
        repaint();
    }
    
    void timerCallback() override
    {
        // Пик медленно затухает
        peakLevel *= 0.95f;
        repaint();
    }
    
    void paint(juce::Graphics& g) override
    {
        auto bounds = getLocalBounds().toFloat();
        
        // Фон
        g.setColour(HuntericeLookAndFeel::bgGraphite);
        g.fillRoundedRectangle(bounds, 3.0f);
        
        // Граница
        g.setColour(HuntericeLookAndFeel::silver);
        g.drawRoundedRectangle(bounds, 3.0f, 1.0f);
        
        // Уровень (градиент от зелёного к красному)
        auto meterBounds = bounds.reduced(2);
        auto levelHeight = meterBounds.getHeight() * currentLevel;
        auto levelBounds = meterBounds.withY(meterBounds.getBottom() - levelHeight).withHeight(levelHeight);
        
        juce::ColourGradient gradient(
            juce::Colour(0xFF00FF00), 0.0f, meterBounds.getBottom(),
            juce::Colour(0xFFFF0000), 0.0f, meterBounds.getY(),
            false);
        gradient.addColour(0.7, juce::Colour(0xFFFFFF00)); // Жёлтый на 70%
        
        g.setGradientFill(gradient);
        g.fillRoundedRectangle(levelBounds, 2.0f);
        
        // Пик-индикатор
        if (peakLevel > 0.01f)
        {
            auto peakY = meterBounds.getBottom() - meterBounds.getHeight() * peakLevel;
            g.setColour(HuntericeLookAndFeel::coldWhite);
            g.fillRect(meterBounds.getX(), peakY - 2, meterBounds.getWidth(), 2);
        }
    }
    
private:
    float currentLevel = 0.0f;
    float peakLevel = 0.0f;
};
