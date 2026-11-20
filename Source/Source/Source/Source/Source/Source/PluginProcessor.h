#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include "SynthVoice.h"
#include "SynthSound.h"

class HuntericeSynthAudioProcessor : public juce::AudioProcessor
{
public:
    HuntericeSynthAudioProcessor();
    ~HuntericeSynthAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;
    bool acceptsMidi() const override { return true; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }

    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int index) override { juce::ignoreUnused(index); }
    const juce::String getProgramName(int index) override { juce::ignoreUnused(index); return {}; }
    void changeProgramName(int index, const juce::String& newName) override { juce::ignoreUnused(index, newName); }

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState& getAPVTS() { return apvts; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

    juce::AudioProcessorValueTreeState apvts;
    juce::Synthesiser synth;

    // Oscillator + ADSR
    std::atomic<float>* waveformParam { nullptr };
    std::atomic<float>* attackParam { nullptr };
    std::atomic<float>* decayParam { nullptr };
    std::atomic<float>* sustainParam { nullptr };
    std::atomic<float>* releaseParam { nullptr };
    std::atomic<float>* volumeParam { nullptr };

    // LFO
    std::atomic<float>* lfoRateParam { nullptr };
    std::atomic<float>* lfoWaveformParam { nullptr };

    // Filter
    std::atomic<float>* filterCutoffParam { nullptr };
    std::atomic<float>* filterResonanceParam { nullptr };
    std::atomic<float>* filterTypeParam { nullptr };

    // Modulation Matrix
    std::atomic<float>* pitchModParam { nullptr };
    std::atomic<float>* filterModParam { nullptr };
    std::atomic<float>* ampModParam { nullptr };

    void updateSynthParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeSynthAudioProcessor)
};
