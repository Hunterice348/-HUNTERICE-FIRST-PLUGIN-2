#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeSynthAudioProcessor::HuntericeSynthAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    synth.addSound(new SynthSound());
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SynthVoice());

    // Oscillator
    oscTypeParam = apvts.getRawParameterValue("OSC_TYPE");
    pulseWidthParam = apvts.getRawParameterValue("PULSE_WIDTH");
    wavetablePosParam = apvts.getRawParameterValue("WT_POS");
    
    // Sub Oscillator
    subTypeParam = apvts.getRawParameterValue("SUB_TYPE");
    subOctaveParam = apvts.getRawParameterValue("SUB_OCT");
    subMixParam = apvts.getRawParameterValue("SUB_MIX");

    // Amp Envelope
    ampAttackParam = apvts.getRawParameterValue("AMP_ATTACK");
    ampDecayParam = apvts.getRawParameterValue("AMP_DECAY");
    ampSustainParam = apvts.getRawParameterValue("AMP_SUSTAIN");
    ampReleaseParam = apvts.getRawParameterValue("AMP_RELEASE");
    ampEnvVelSensParam = apvts.getRawParameterValue("AMP_VEL_SENS");
    
    // Filter Envelope
    filtAttackParam = apvts.getRawParameterValue("FILT_ATTACK");
    filtDecayParam = apvts.getRawParameterValue("FILT_DECAY");
    filtSustainParam = apvts.getRawParameterValue("FILT_SUSTAIN");
    filtReleaseParam = apvts.getRawParameterValue("FILT_RELEASE");
    filtEnvVelSensParam = apvts.getRawParameterValue("FILT_VEL_SENS");
    filtEnvAmountParam = apvts.getRawParameterValue("FILT_ENV_AMT");
    filtKeyTrackParam = apvts.getRawParameterValue("FILT_KEY_TRACK");
    
    // LFOs
    lfo1RateParam = apvts.getRawParameterValue("LFO1_RATE");
    lfo1WaveParam = apvts.getRawParameterValue("LFO1_WAVE");
    lfo2RateParam = apvts.getRawParameterValue("LFO2_RATE");
    lfo2WaveParam = apvts.getRawParameterValue("LFO2_WAVE");

    // Filter
    filterCutoffParam = apvts.getRawParameterValue("FILTER_CUTOFF");
    filterResonanceParam = apvts.getRawParameterValue("FILTER_RES");
    filterTypeParam = apvts.getRawParameterValue("FILTER_TYPE");

    // Unison & Detune
    unisonVoicesParam = apvts.getRawParameterValue("UNISON_VOICES");
    detuneParam = apvts.getRawParameterValue("DETUNE");
    
    // Portamento
    portamentoEnabledParam = apvts.getRawParameterValue("PORTA_ENABLED");
    portamentoTimeParam = apvts.getRawParameterValue("PORTA_TIME");
    
    // Modulation Matrix
    for (int i = 0; i < 8; ++i)
    {
        modSourceParams[i] = apvts.getRawParameterValue("MOD_SRC_" + std::to_string(i));
        modDestParams[i] = apvts.getRawParameterValue("MOD_DST_" + std::to_string(i));
        modAmountParams[i] = apvts.getRawParameterValue("MOD_AMT_" + std::to_string(i));
    }

    // Master
    volumeParam = apvts.getRawParameterValue("VOLUME");
}

HuntericeSynthAudioProcessor::~HuntericeSynthAudioProcessor() = default;

const juce::String HuntericeSynthAudioProcessor::getName() const { return JucePlugin_Name; }

void HuntericeSynthAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate(sampleRate);
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
            voice->prepareToPlay(sampleRate, samplesPerBlock, getTotalNumOutputChannels());
    }
    updateSynthParameters();
}

void HuntericeSynthAudioProcessor::releaseResources() {}

bool HuntericeSynthAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    return layouts.getMainOutputChannelSet() == juce::AudioChannelSet::stereo();
}

void HuntericeSynthAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();
    updateSynthParameters();
    synth.renderNextBlock(buffer, midiMessages, 0, buffer.getNumSamples());
}

bool HuntericeSynthAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* HuntericeSynthAudioProcessor::createEditor()
{
    return new HuntericeSynthAudioProcessorEditor(*this);
}

void HuntericeSynthAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HuntericeSynthAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState != nullptr && xmlState->hasTagName(apvts.state.getType()))
        apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
}

void HuntericeSynthAudioProcessor::updateSynthParameters()
{
    for (int i = 0; i < synth.getNumVoices(); ++i)
    {
        if (auto voice = dynamic_cast<SynthVoice*>(synth.getVoice(i)))
        {
            // Oscillator
            voice->setOscillatorType(static_cast<int>(oscTypeParam->load()));
            voice->setPulseWidth(pulseWidthParam->load());
            voice->setWavetablePosition(wavetablePosParam->load());
            
            // Sub Oscillator
            voice->setSubOscillatorType(static_cast<int>(subTypeParam->load()));
            voice->setSubOctave(static_cast<int>(subOctaveParam->load()));
            voice->setSubMix(subMixParam->load());

            // Amp Envelope
            voice->setAmpEnvelope(
                ampAttackParam->load() / 1000.0f,
                ampDecayParam->load() / 1000.0f,
                ampSustainParam->load(),
                ampReleaseParam->load() / 1000.0f
            );
            voice->setAmpEnvVelocitySensitivity(ampEnvVelSensParam->load());
            
            // Filter Envelope
            voice->setFilterEnvelope(
                filtAttackParam->load() / 1000.0f,
                filtDecayParam->load() / 1000.0f,
                filtSustainParam->load(),
                filtReleaseParam->load() / 1000.0f
            );
            voice->setFilterEnvVelocitySensitivity(filtEnvVelSensParam->load());
            voice->setFilterEnvAmount(filtEnvAmountParam->load());
            voice->setFilterKeyTrackAmount(filtKeyTrackParam->load());

            // LFOs
            voice->setLFO1Parameters(lfo1RateParam->load(), static_cast<int>(lfo1WaveParam->load()));
            voice->setLFO2Parameters(lfo2RateParam->load(), static_cast<int>(lfo2WaveParam->load()));

            // Filter
            voice->setFilterParameters(
                filterCutoffParam->load(),
                filterResonanceParam->load(),
                static_cast<int>(filterTypeParam->load())
            );

            // Unison & Detune
            voice->setUnisonVoices(static_cast<int>(unisonVoicesParam->load()));
            voice->setDetune(detuneParam->load());
            
            // Portamento
            voice->setPortamentoEnabled(portamentoEnabledParam->load() > 0.5f);
            voice->setPortamentoTime(portamentoTimeParam->load());
            
            // Modulation Matrix
            for (int slot = 0; slot < 8; ++slot)
            {
                voice->setModulationSlot(
                    slot,
                    static_cast<int>(modSourceParams[slot]->load()),
                    static_cast<int>(modDestParams[slot]->load()),
                    modAmountParams[slot]->load()
                );
            }

            // Master
            voice->setGain(volumeParam->load());
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout HuntericeSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Oscillator
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("OSC_TYPE", 1), "Osc Type", 0, 7, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PULSE_WIDTH", 1), "Pulse Width", juce::NormalisableRange<float>(0.01f, 0.99f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("WT_POS", 1), "Wavetable Pos", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    
    // Sub Oscillator
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("SUB_TYPE", 1), "Sub Type", 0, 7, 3));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("SUB_OCT", 1), "Sub Octave", -3, 0, -1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("SUB_MIX", 1), "Sub Mix", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // Amp Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("AMP_ATTACK", 1), "Amp Attack", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 10.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("AMP_DECAY", 1), "Amp Decay", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 100.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("AMP_SUSTAIN", 1), "Amp Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("AMP_RELEASE", 1), "Amp Release", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 500.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("AMP_VEL_SENS", 1), "Amp Vel Sens", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    
    // Filter Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILT_ATTACK", 1), "Filt Attack", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 10.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILT_DECAY", 1), "Filt Decay", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 200.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILT_SUSTAIN", 1), "Filt Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILT_RELEASE", 1), "Filt Release", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 500.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILT_VEL_SENS", 1), "Filt Vel Sens", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILT_ENV_AMT", 1), "Filt Env Amount", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILT_KEY_TRACK", 1), "Filt Key Track", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    
    // LFOs
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("LFO1_RATE", 1), "LFO1 Rate", juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f), 2.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("LFO1_WAVE", 1), "LFO1 Wave", 0, 6, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("LFO2_RATE", 1), "LFO2 Rate", juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f), 5.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("LFO2_WAVE", 1), "LFO2 Wave", 0, 6, 0));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILTER_CUTOFF", 1), "Filter Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 5000.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILTER_RES", 1), "Filter Resonance", juce::NormalisableRange<float>(0.5f, 20.0f, 0.1f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("FILTER_TYPE", 1), "Filter Type", 0, 4, 0));

    // Unison & Detune
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("UNISON_VOICES", 1), "Unison Voices", 1, 8, 1));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DETUNE", 1), "Detune", juce::NormalisableRange<float>(0.0f, 100.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("cents")));
    
    // Portamento
    params.push_back(std::make_unique<juce::AudioParameterBool>(juce::ParameterID("PORTA_ENABLED", 1), "Portamento", false));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("PORTA_TIME", 1), "Portamento Time", juce::NormalisableRange<float>(0.0f, 2000.0f, 1.0f), 50.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    
    // Modulation Matrix (8 слотов)
    for (int i = 0; i < 8; ++i)
    {
        params.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("MOD_SRC_" + std::to_string(i), 1),
            "Mod Source " + std::to_string(i + 1),
            0, 6, 0));
        
        params.push_back(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID("MOD_DST_" + std::to_string(i), 1),
            "Mod Dest " + std::to_string(i + 1),
            0, 4, 0));
        
        params.push_back(std::make_unique<juce::AudioParameterFloat>(
            juce::ParameterID("MOD_AMT_" + std::to_string(i), 1),
            "Mod Amount " + std::to_string(i + 1),
            juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    }

    // Master
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("VOLUME", 1), "Volume", juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HuntericeSynthAudioProcessor();
}
