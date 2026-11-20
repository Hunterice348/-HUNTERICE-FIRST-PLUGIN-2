#pragma once

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

class PianoKeyboard : public juce::MidiKeyboardComponent
{
public:
    PianoKeyboard(juce::MidiKeyboardState& state, Orientation orientation)
        : juce::MidiKeyboardComponent(state, orientation)
    {
        setKeyWidth(20.0f);
        setAvailableRange(24, 96); // C1 до C7
    }
    
    void drawWhiteKey(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area) override
    {
        bool isDown = isNoteOnForMouseDownCallback(midiNoteNumber);
        bool isOver = isMouseOverKey(midiNoteNumber);
        
        g.setColour(isDown ? HuntericeLookAndFeel::darkRed : 
                   isOver ? HuntericeLookAndFeel::silverLight : 
                   HuntericeLookAndFeel::coldWhite);
        g.fillRoundedRectangle(area, 2.0f);
        
        g.setColour(HuntericeLookAndFeel::silver);
        g.drawRoundedRectangle(area, 2.0f, 1.0f);
    }
    
    void drawBlackKey(int midiNoteNumber, juce::Graphics& g, juce::Rectangle<float> area) override
    {
        bool isDown = isNoteOnForMouseDownCallback(midiNoteNumber);
        bool isOver = isMouseOverKey(midiNoteNumber);
        
        g.setColour(isDown ? HuntericeLookAndFeel::accentRed : 
                   isOver ? HuntericeLookAndFeel::bgGraphiteLight : 
                   HuntericeLookAndFeel::bgBlack);
        g.fillRoundedRectangle(area, 2.0f);
        
        g.setColour(HuntericeLookAndFeel::silver);
        g.drawRoundedRectangle(area, 2.0f, 1.0f);
    }
    
private:
    bool isMouseOverKey(int midiNoteNumber)
    {
        auto mousePos = getMouseXYRelative();
        auto keyBounds = getRectangleForKey(midiNoteNumber);
        return keyBounds.toFloat().contains(mousePos.toFloat());
    }
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PianoKeyboard)
};
