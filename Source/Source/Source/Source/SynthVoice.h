#pragma once

#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_dsp/juce_dsp.h>
#include "SynthSound.h"
#include "LFOEngine.h"
#include "FilterEngine.h"
#include "ModulationMatrix.h"

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

    // Setters для параметров
    void setOscillatorWaveform(int waveform);
    void setADSR(float attack, float decay, float sustain, float release);
    void setGain(float gainDb);
    
    // Новые параметры
    void setLFOParameters(float rate, int waveform);
    void setFilterParameters(float cutoff, float resonance, int type);
    void setModulationAmounts(float pitchMod, float filterMod, float ampMod);

private:
    // Oscillator
    juce::dsp::Oscillator<float> oscillator;
    int currentWaveform = 0;

    // ADSR
    juce::ADSR adsr;
    juce::ADSR::Parameters adsrParams;

    // LFO
    LFOEngine lfo;
    float lfoRate = 1.0f;
    int lfoWaveform = 0;

    // Filter
    FilterEngine filter;
    float filterCutoff = 1000.0f;
    float filterResonance = 1.0f;
    int filterType = 0;

    // Modulation Matrix
    ModulationMatrix modMatrix;
    float pitchModAmount = 0.0f;
    float filterModAmount = 0.0f;
    float ampModAmount = 0.0f;

    // Gain
    float currentGain = 1.0f;

    // Pitch
    double currentSampleRate = 44100.0;
    int currentMidiNote = 60;
    float currentVelocity = 0.0f;
    float pitchBendSemitones = 0.0f;

    float getFrequencyForNote(int noteNumber);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(SynthVoice)
};
