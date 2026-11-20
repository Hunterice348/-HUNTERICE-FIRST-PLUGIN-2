#include "PluginProcessor.h"
#include "PluginEditor.h"

HuntericeSynthAudioProcessor::HuntericeSynthAudioProcessor()
    : AudioProcessor(BusesProperties().withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    synth.addSound(new SynthSound());
    for (int i = 0; i < 8; ++i)
        synth.addVoice(new SynthVoice());

    // Oscillator + ADSR
    waveformParam = apvts.getRawParameterValue("WAVEFORM");
    attackParam = apvts.getRawParameterValue("ATTACK");
    decayParam = apvts.getRawParameterValue("DECAY");
    sustainParam = apvts.getRawParameterValue("SUSTAIN");
    releaseParam = apvts.getRawParameterValue("RELEASE");
    volumeParam = apvts.getRawParameterValue("VOLUME");

    // LFO
    lfoRateParam = apvts.getRawParameterValue("LFO_RATE");
    lfoWaveformParam = apvts.getRawParameterValue("LFO_WAVE");

    // Filter
    filterCutoffParam = apvts.getRawParameterValue("FILTER_CUTOFF");
    filterResonanceParam = apvts.getRawParameterValue("FILTER_RES");
    filterTypeParam = apvts.getRawParameterValue("FILTER_TYPE");

    // Modulation
    pitchModParam = apvts.getRawParameterValue("MOD_PITCH");
    filterModParam = apvts.getRawParameterValue("MOD_FILTER");
    ampModParam = apvts.getRawParameterValue("MOD_AMP");
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
            voice->setOscillatorWaveform(static_cast<int>(waveformParam->load()));
            voice->setADSR(
                attackParam->load() / 1000.0f,
                decayParam->load() / 1000.0f,
                sustainParam->load(),
                releaseParam->load() / 1000.0f
            );
            voice->setGain(volumeParam->load());

            voice->setLFOParameters(
                lfoRateParam->load(),
                static_cast<int>(lfoWaveformParam->load())
            );

            voice->setFilterParameters(
                filterCutoffParam->load(),
                filterResonanceParam->load(),
                static_cast<int>(filterTypeParam->load())
            );

            voice->setModulationAmounts(
                pitchModParam->load(),
                filterModParam->load(),
                ampModParam->load()
            );
        }
    }
}

juce::AudioProcessorValueTreeState::ParameterLayout HuntericeSynthAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Oscillator + ADSR (из предыдущей версии)
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("WAVEFORM", 1), "Waveform", 0, 3, 0));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("ATTACK", 1), "Attack", juce::NormalisableRange<float>(1.0f, 5000.0f, 1.0f), 10.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("DECAY", 1), "Decay", juce::NormalisableRange<float>(1.0f, 5000.0f, 1.0f), 100.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("SUSTAIN", 1), "Sustain", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.7f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("RELEASE", 1), "Release", juce::NormalisableRange<float>(1.0f, 10000.0f, 1.0f), 500.0f, juce::AudioParameterFloatAttributes().withLabel("ms")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("VOLUME", 1), "Volume", juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f), 0.0f, juce::AudioParameterFloatAttributes().withLabel("dB")));

    // LFO
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("LFO_RATE", 1), "LFO Rate", juce::NormalisableRange<float>(0.1f, 20.0f, 0.01f), 2.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("LFO_WAVE", 1), "LFO Wave", 0, 6, 0));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILTER_CUTOFF", 1), "Filter Cutoff", juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f), 5000.0f, juce::AudioParameterFloatAttributes().withLabel("Hz")));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("FILTER_RES", 1), "Filter Resonance", juce::NormalisableRange<float>(0.5f, 20.0f, 0.1f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterInt>(juce::ParameterID("FILTER_TYPE", 1), "Filter Type", 0, 4, 0));

    // Modulation Matrix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MOD_PITCH", 1), "LFO→Pitch", juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MOD_FILTER", 1), "LFO→Filter", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID("MOD_AMP", 1), "LFO→Amp", juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    return { params.begin(), params.end() };
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HuntericeSynthAudioProcessor();
}
