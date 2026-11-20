#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
HuntericeFirstPluginAudioProcessor::HuntericeFirstPluginAudioProcessor()
    : AudioProcessor(BusesProperties()
                     .withInput("Input", juce::AudioChannelSet::stereo(), true)
                     .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    // Кэшируем указатели на атомарные значения параметров
    // Это самый быстрый способ чтения параметров в аудиопотоке
    driveParamRaw  = apvts.getRawParameterValue("DRIVE");
    mixParamRaw    = apvts.getRawParameterValue("MIX");
    outputParamRaw = apvts.getRawParameterValue("OUTPUT");
}

HuntericeFirstPluginAudioProcessor::~HuntericeFirstPluginAudioProcessor() = default;

//==============================================================================
const juce::String HuntericeFirstPluginAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool HuntericeFirstPluginAudioProcessor::acceptsMidi() const     { return false; }
bool HuntericeFirstPluginAudioProcessor::producesMidi() const    { return false; }
bool HuntericeFirstPluginAudioProcessor::isMidiEffect() const    { return false; }
double HuntericeFirstPluginAudioProcessor::getTailLengthSeconds() const { return 0.0; }

//==============================================================================
int HuntericeFirstPluginAudioProcessor::getNumPrograms()         { return 1; }
int HuntericeFirstPluginAudioProcessor::getCurrentProgram()      { return 0; }

void HuntericeFirstPluginAudioProcessor::setCurrentProgram(int index)
{
    juce::ignoreUnused(index);
}

const juce::String HuntericeFirstPluginAudioProcessor::getProgramName(int index)
{
    juce::ignoreUnused(index);
    return {};
}

void HuntericeFirstPluginAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    juce::ignoreUnused(index, newName);
}

//==============================================================================
void HuntericeFirstPluginAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused(samplesPerBlock);

    // 20ms время сглаживания — компромисс между плавностью и отзывчивостью
    const double smoothTimeSeconds = 0.02;

    driveSmoothed.reset(sampleRate, smoothTimeSeconds);
    mixSmoothed.reset(sampleRate, smoothTimeSeconds);
    outputSmoothed.reset(sampleRate, smoothTimeSeconds);

    // Устанавливаем начальные значения без скачков
    driveSmoothed.setCurrentAndTargetValue(driveParamRaw->load());
    mixSmoothed.setCurrentAndTargetValue(mixParamRaw->load());
    outputSmoothed.setCurrentAndTargetValue(outputParamRaw->load());
}

void HuntericeFirstPluginAudioProcessor::releaseResources() {}

//==============================================================================
bool HuntericeFirstPluginAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    // Поддерживаем только стерео (вход = выход)
    const auto mainIn  = layouts.getMainInputChannelSet();
    const auto mainOut = layouts.getMainOutputChannelSet();

    if (mainOut != juce::AudioChannelSet::stereo())
        return false;

    if (mainIn != mainOut)
        return false;

    return true;
}

//==============================================================================
inline float HuntericeFirstPluginAudioProcessor::applyDistortion(float sample, float drive) noexcept
{
    // drive: 0.0 ... 1.0 -> gain: 1x ... 50x
    const float gain = 1.0f + drive * 49.0f;

    // Усиливаем сигнал
    const float driven = sample * gain;

    // Soft-clipping через гиперболический тангенс
    // tanh даёт мягкое насыщение без жёсткого клиппинга
    return std::tanh(driven);
}

//==============================================================================
void HuntericeFirstPluginAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                                      juce::MidiBuffer& midiMessages)
{
    juce::ignoreUnused(midiMessages);
    juce::ScopedNoDenormals noDenormals;

    const int totalNumInputChannels  = getTotalNumInputChannels();
    const int totalNumOutputChannels = getTotalNumOutputChannels();

    // Очищаем выходные каналы, которых нет на входе
    for (int i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // Обновляем целевые значения smoothed-параметров (один раз на блок)
    driveSmoothed.setTargetValue(driveParamRaw->load());
    mixSmoothed.setTargetValue(mixParamRaw->load());
    outputSmoothed.setTargetValue(outputParamRaw->load());

    const int numSamples = buffer.getNumSamples();

    float* leftData  = buffer.getWritePointer(0);
    float* rightData = buffer.getWritePointer(1);

    // ВАЖНО: внешний цикл по сэмплам, внутренний по каналам.
    // Это гарантирует, что smoothed-значения одинаковы для L и R на каждом сэмпле,
    // и исключает рассинхронизацию каналов.
    for (int sample = 0; sample < numSamples; ++sample)
    {
        // Получаем СГЛАЖЕННЫЕ значения параметров (без щелчков)
        const float drive  = driveSmoothed.getNextValue();
        const float mix    = mixSmoothed.getNextValue();
        const float output = outputSmoothed.getNextValue();

        // Преобразуем dB в линейный gain
        const float outputGain = juce::Decibels::decibelsToGain(output, -60.0f);

        // --- Левый канал ---
        const float dryL = leftData[sample];
        const float wetL = applyDistortion(dryL, drive);
        const float mixedL = dryL * (1.0f - mix) + wetL * mix;
        leftData[sample] = mixedL * outputGain;

        // --- Правый канал ---
        const float dryR = rightData[sample];
        const float wetR = applyDistortion(dryR, drive);
        const float mixedR = dryR * (1.0f - mix) + wetR * mix;
        rightData[sample] = mixedR * outputGain;
    }
}

//==============================================================================
bool HuntericeFirstPluginAudioProcessor::hasEditor() const { return true; }

juce::AudioProcessorEditor* HuntericeFirstPluginAudioProcessor::createEditor()
{
    return new HuntericeFirstPluginAudioProcessorEditor(*this);
}

//==============================================================================
void HuntericeFirstPluginAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void HuntericeFirstPluginAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));

    if (xmlState != nullptr)
    {
        if (xmlState->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xmlState));
        }
    }
}

//==============================================================================
juce::AudioProcessorValueTreeState::ParameterLayout
HuntericeFirstPluginAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Drive: 0.0 ... 1.0 (коэффициент искажения)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("DRIVE", 1),
        "Drive",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        0.0f,
        juce::AudioParameterFloatAttributes()
    ));

    // Mix: 0.0 ... 1.0 (dry/wet)
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("MIX", 1),
        "Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.001f),
        1.0f,
        juce::AudioParameterFloatAttributes()
    ));

    // Output: -60 dB ... +12 dB
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID("OUTPUT", 1),
        "Output",
        juce::NormalisableRange<float>(-60.0f, 12.0f, 0.1f),
        0.0f,
        juce::AudioParameterFloatAttributes().withLabel("dB")
    ));

    return { params.begin(), params.end() };
}

//==============================================================================
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new HuntericeFirstPluginAudioProcessor();
}
