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

    // Oscillator
    std::atomic<float>* oscTypeParam { nullptr };
    std::atomic<float>* pulseWidthParam { nullptr };
    std::atomic<float>* wavetablePosParam { nullptr };
    
    // Sub Oscillator
    std::atomic<float>* subTypeParam { nullptr };
    std::atomic<float>* subOctaveParam { nullptr };
    std::atomic<float>* subMixParam { nullptr };

    // Amp Envelope
    std::atomic<float>* ampAttackParam { nullptr };
    std::atomic<float>* ampDecayParam { nullptr };
    std::atomic<float>* ampSustainParam { nullptr };
    std::atomic<float>* ampReleaseParam { nullptr };
    std::atomic<float>* ampEnvVelSensParam { nullptr };
    
    // Filter Envelope
    std::atomic<float>* filtAttackParam { nullptr };
    std::atomic<float>* filtDecayParam { nullptr };
    std::atomic<float>* filtSustainParam { nullptr };
    std::atomic<float>* filtReleaseParam { nullptr };
    std::atomic<float>* filtEnvVelSensParam { nullptr };
    std::atomic<float>* filtEnvAmountParam { nullptr };
    std::atomic<float>* filtKeyTrackParam { nullptr };
    
    // LFOs
    std::atomic<float>* lfo1RateParam { nullptr };
    std::atomic<float>* lfo1WaveParam { nullptr };
    std::atomic<float>* lfo2RateParam { nullptr };
    std::atomic<float>* lfo2WaveParam { nullptr };

    // Filter
    std::atomic<float>* filterCutoffParam { nullptr };
    std::atomic<float>* filterResonanceParam { nullptr };
    std::atomic<float>* filterTypeParam { nullptr };

    // Unison & Detune
    std::atomic<float>* unisonVoicesParam { nullptr };
    std::atomic<float>* detuneParam { nullptr };
    
    // Portamento
    std::atomic<float>* portamentoEnabledParam { nullptr };
    std::atomic<float>* portamentoTimeParam { nullptr };
    
    // Modulation Matrix (8 слотов)
    std::array<std::atomic<float>*, 8> modSourceParams;
    std::array<std::atomic<float>*, 8> modDestParams;
    std::array<std::atomic<float>*, 8> modAmountParams;

    // Master
    std::atomic<float>* volumeParam { nullptr };

    void updateSynthParameters();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(HuntericeSynthAudioProcessor)
};
