#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "SynthSound.h"
#include "LFOEngine.h"
#include "FilterEngine.h"
#include "ModulationMatrix.h"
#include "OscillatorEngine.h"

class SynthVoice : public juce::SynthesiserVoice
{
public:
    SynthVoice();

    bool canPlaySound(juce::SynthesiserSound* sound) override;
    void startNote(int midiNoteNumber, float velocity, 
                   juce::SynthesiserSound* sound, int currentPitchWheelPosition) override;
    void stopNote(float velocity, bool allowTailOff) override;
    void pitchWheelMoved(int newPitchWheelValue) override;
    void controllerMoved(int controllerNumber, int newControllerValue) override;
    void prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels) override;
    void renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples) override;

    // Oscillator параметры
    void setOscillatorType(int type);
    void setPulseWidth(float width);
    void setSubOctave(int octave);
    void setSubMix(float mix);
    void setWavetablePosition(float position);
    void setSubOscillatorType(int type);
    
    // ADSR
    void setADSR(float attack, float decay, float sustain, float release);
    void setGain(float gainDb);
    
    // LFO
    void setLFOParameters(float rate, int waveform);
    
    // Filter
    void setFilterParameters(float cutoff, float resonance, int type);
    
    // Modulation Matrix
    void setModulationAmounts(float pitchMod, float filterMod, float ampMod);

private:
    // Main Oscillator
    OscillatorEngine mainOsc;
    int mainOscType = 0;
    float pulseWidth = 0.5f;
    float wavetablePosition = 0.0f;
    
    // Sub Oscillator
    OscillatorEngine subOsc;
    int subOscType = 0;
    int subOctave = -1;
    float subMix = 0.0f;  // 0.0 ... 1.0

    // ADSR
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    // LFO
    LFOEngine lfo;

    // Filter
    FilterEngine filter;
    float filterCutoff = 1000.0f;
    float filterResonance = 1.0f;
    int filterType = 0;

    // Modulation Matrix
    ModulationMatrix modMatrix;

    // Gain
    float currentGain = 1.0f;

    // Pitch state
    double currentSampleRate = 44100.0;
    int currentMidiNote = 60;
    float currentVelocity = 0.0f;
    float pitchBendSemitones = 0.0f;

    float getFrequencyForNote(int noteNumber);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};
