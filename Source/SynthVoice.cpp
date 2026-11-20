#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    mainOsc.setType(OscillatorEngine::Type::Sine);
    mainOsc.setFrequency(440.0f);
    
    subOsc.setType(OscillatorEngine::Type::Square);
    subOsc.setSubOctave(-1);
    
    // Инициализация unison oscillators
    for (int i = 0; i < MAX_UNISON_VOICES; ++i)
    {
        unisonOscillators[i].setType(OscillatorEngine::Type::Saw);
        unisonDetuneMultipliers[i] = 1.0f;
    }
}

bool SynthVoice::canPlaySound(juce::SynthesiserSound* sound)
{
    return dynamic_cast<SynthSound*>(sound) != nullptr;
}

void SynthVoice::startNote(int midiNoteNumber, float velocity, 
                           juce::SynthesiserSound* sound, int currentPitchWheelPosition)
{
    juce::ignoreUnused(sound);
    
    currentMidiNote = midiNoteNumber;
    currentVelocity = velocity;
    targetPitch = static_cast<float>(midiNoteNumber);
    
    if (!portamentoEnabled)
        currentPitch = targetPitch;
    
    // Pitch wheel
    pitchBendSemitones = (currentPitchWheelPosition / 8192.0f - 1.0f) * 2.0f;
    
    // Запуск envelope
    ampEnvelope.noteOn(velocity);
    filterEnvelope.noteOn(velocity);
    
    // Сброс LFO фазы (опционально)
    lfo1.reset();
    lfo2.reset();
    
    // Синхронизация фазы осцилляторов
    mainOsc.syncPhase();
    subOsc.syncPhase();
    for (auto& osc : unisonOscillators)
        osc.syncPhase();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    juce::ignoreUnused(velocity);
    
    if (allowTailOff)
    {
        ampEnvelope.noteOff();
        filterEnvelope.noteOff();
    }
    else
    {
        clearCurrentNote();
        ampEnvelope.reset();
        filterEnvelope.reset();
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    pitchBendSemitones = (newPitchWheelValue / 8192.0f - 1.0f) * 2.0f;
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    if (controllerNumber == 1) // Mod Wheel
        modWheelValue = newControllerValue / 127.0f;
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::ignoreUnused(samplesPerBlock, outputChannels);
    
    currentSampleRate = sampleRate;
    
    mainOsc.prepare(sampleRate);
    subOsc.prepare(sampleRate);
    for (auto& osc : unisonOscillators)
        osc.prepare(sampleRate);
    
    ampEnvelope.prepare(sampleRate);
    filterEnvelope.prepare(sampleRate);
    lfo1.prepare(sampleRate);
    lfo2.prepare(sampleRate);
    filter.prepare(sampleRate);
    modMatrix.prepare(sampleRate);
    
    // Расчёт коэффициента portamento
    if (portamentoTime > 0.0f)
        portamentoCoeff = std::exp(-1.0f / (portamentoTime * 0.001f * sampleRate));
    else
        portamentoCoeff = 0.0f;
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    juce::AudioBuffer<float> tempBuffer(outputBuffer.getNumChannels(), numSamples);
    tempBuffer.clear();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // 1. Portamento (glide)
        if (portamentoEnabled && std::abs(currentPitch - targetPitch) > 0.01f)
            currentPitch += (targetPitch - currentPitch) * (1.0f - portamentoCoeff);
        else
            currentPitch = targetPitch;
        
        // 2. Получаем значения envelope и LFO
        float env1Value = ampEnvelope.getNextSample();
        float env2Value = filterEnvelope.getNextSample();
        float lfo1Value = lfo1.processSample();
        float lfo2Value = lfo2.processSample();
        
        // Key tracking: 0.0 при C0, 1.0 при C8
        float keyTracking = (currentPitch - 24.0f) / 96.0f;
        keyTracking = juce::jlimit(0.0f, 1.0f, keyTracking);
        
        // 3. Обновляем источники модуляции
        modMatrix.updateSources(
            env1Value,
            env2Value,
            lfo1Value,
            lfo2Value,
            currentVelocity,
            keyTracking,
            modWheelValue
        );
        
        // 4. Получаем модулированные значения
        float pitchMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::Pitch);
        float filterMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::FilterCutoff);
        float pwmMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::PulseWidth);
        float ampMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::Amplitude);
        float detuneMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::Detune);
        
        // 5. Расчёт финальной частоты
        float baseFrequency = getFrequencyForNote(static_cast<int>(currentPitch));
        float fractionalPart = currentPitch - std::floor(currentPitch);
        float interpolatedFreq = baseFrequency * std::pow(2.0f, fractionalPart / 12.0f);
        
        float totalPitchSemitones = pitchBendSemitones + pitchMod;
        float frequencyMultiplier = std::pow(2.0f, totalPitchSemitones / 12.0f);
        float finalFrequency = interpolatedFreq * frequencyMultiplier;
        
        // 6. Main oscillator
        mainOsc.setFrequency(finalFrequency);
        mainOsc.setModulatedPulseWidth(pwmMod);
        float mainSample = mainOsc.processSample();
        
        // 7. Unison oscillators
        float unisonSum = 0.0f;
        if (numUnisonVoices > 1)
        {
            updateUnisonDetune();
            for (int i = 0; i < numUnisonVoices; ++i)
            {
                float detunedFreq = finalFrequency * unisonDetuneMultipliers[i] * (1.0f + detuneMod * 0.01f);
                unisonOscillators[i].setFrequency(detunedFreq);
                unisonOscillators[i].setModulatedPulseWidth(pwmMod);
                unisonSum += unisonOscillators[i].processSample();
            }
            unisonSum /= numUnisonVoices;
            mainSample = mainSample * 0.5f + unisonSum * 0.5f;
        }
        
        // 8. Sub oscillator
        subOsc.setFrequency(finalFrequency);
        float subSample = subOsc.processSample();
        float oscOutput = mainSample * (1.0f - subMix) + subSample * subMix;
        
        // 9. Filter с модуляцией
        float env2Modulated = filterEnvAmount * env2Value * 10000.0f;
        float keyTrackModulated = filterKeyTrackAmount * keyTracking * 10000.0f;
        float lfoFilterMod = filterMod * 5000.0f;
        
        float modulatedCutoff = filterCutoff + env2Modulated + keyTrackModulated + lfoFilterMod;
        modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);
        
        filter.setParameters(modulatedCutoff, filterResonance, static_cast<FilterEngine::Type>(filterType));
        
        float filteredL = oscOutput;
        float filteredR = oscOutput;
        filter.processSample(filteredL, filteredR);
        
        // 10. Amplitude modulation
        float totalAmpMod = env1Value * (1.0f + ampMod);
        totalAmpMod = juce::jlimit(0.0f, 1.0f, totalAmpMod);
        
        // 11. Финальная амплитуда
        float sampleValue = filteredL * totalAmpMod * currentVelocity * currentGain;
        
        // Записываем в буфер (стерео)
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            tempBuffer.addSample(channel, sample, sampleValue);
    }

    outputBuffer.addFrom(0, startSample, tempBuffer, 0, 0, numSamples);
    if (outputBuffer.getNumChannels() > 1)
        outputBuffer.addFrom(1, startSample, tempBuffer, 1, 0, numSamples);

    if (!ampEnvelope.isActive())
        clearCurrentNote();
}

void SynthVoice::setOscillatorType(int type)
{
    mainOscType = type;
    mainOsc.setType(static_cast<OscillatorEngine::Type>(type));
    for (auto& osc : unisonOscillators)
        osc.setType(static_cast<OscillatorEngine::Type>(type));
}

void SynthVoice::setPulseWidth(float width)
{
    pulseWidth = width;
    mainOsc.setPulseWidth(width);
    for (auto& osc : unisonOscillators)
        osc.setPulseWidth(width);
}

void SynthVoice::setWavetablePosition(float position)
{
    wavetablePosition = position;
    mainOsc.setWavetablePosition(position);
}

void SynthVoice::setSubOscillatorType(int type)
{
    subOscType = type;
    subOsc.setType(static_cast<OscillatorEngine::Type>(type));
}

void SynthVoice::setSubOctave(int octave)
{
    subOctave = octave;
    subOsc.setSubOctave(octave);
}

void SynthVoice::setSubMix(float mix)
{
    subMix = juce::jlimit(0.0f, 1.0f, mix);
}

void SynthVoice::setAmpEnvelope(float attack, float decay, float sustain, float release)
{
    ampEnvelope.setParameters(attack, decay, sustain, release);
}

void SynthVoice::setFilterEnvelope(float attack, float decay, float sustain, float release)
{
    filterEnvelope.setParameters(attack, decay, sustain, release);
}

void SynthVoice::setAmpEnvVelocitySensitivity(float sensitivity)
{
    ampEnvVelocitySensitivity = sensitivity;
    ampEnvelope.setVelocitySensitivity(sensitivity);
}

void SynthVoice::setFilterEnvVelocitySensitivity(float sensitivity)
{
    filterEnvVelocitySensitivity = sensitivity;
    filterEnvelope.setVelocitySensitivity(sensitivity);
}

void SynthVoice::setLFO1Parameters(float rate, int waveform)
{
    lfo1.setRate(rate);
    lfo1.setWaveform(static_cast<LFOEngine::Waveform>(waveform));
}

void SynthVoice::setLFO2Parameters(float rate, int waveform)
{
    lfo2.setRate(rate);
    lfo2.setWaveform(static_cast<LFOEngine::Waveform>(waveform));
}

void SynthVoice::setFilterParameters(float cutoff, float resonance, int type)
{
    filterCutoff = cutoff;
    filterResonance = resonance;
    filterType = type;
}

void SynthVoice::setFilterEnvAmount(float amount)
{
    filterEnvAmount = amount;
}

void SynthVoice::setFilterKeyTrackAmount(float amount)
{
    filterKeyTrackAmount = amount;
}

void SynthVoice::setUnisonVoices(int numVoices)
{
    numUnisonVoices = juce::jlimit(1, MAX_UNISON_VOICES, numVoices);
    updateUnisonDetune();
}

void SynthVoice::setDetune(float detuneCents)
{
    detuneCents = juce::jlimit(0.0f, 100.0f, detuneCents);
    updateUnisonDetune();
}

void SynthVoice::setPortamentoTime(float timeMs)
{
    portamentoTime = juce::jmax(0.0f, timeMs);
    if (portamentoTime > 0.0f)
        portamentoCoeff = std::exp(-1.0f / (portamentoTime * 0.001f * currentSampleRate));
    else
        portamentoCoeff = 0.0f;
}

void SynthVoice::setPortamentoEnabled(bool enabled)
{
    portamentoEnabled = enabled;
}

void SynthVoice::setModulationSlot(int slotIndex, int source, int destination, float amount)
{
    modMatrix.setModulationSlot(
        slotIndex,
        static_cast<ModulationMatrix::Source>(source),
        static_cast<ModulationMatrix::Destination>(destination),
        amount
    );
}

void SynthVoice::setGain(float gainDb)
{
    currentGain = juce::Decibels::decibelsToGain(gainDb, -60.0f);
}

float SynthVoice::getFrequencyForNote(int noteNumber)
{
    return 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);
}

void SynthVoice::updateUnisonDetune()
{
    if (numUnisonVoices <= 1)
        return;
    
    float detuneSemitones = detuneCents / 100.0f;
    
    for (int i = 0; i < numUnisonVoices; ++i)
    {
        // Равномерное распределение детюна вокруг центральной частоты
        float offset = (static_cast<float>(i) / (numUnisonVoices - 1) - 0.5f) * 2.0f * detuneSemitones;
        unisonDetuneMultipliers[i] = std::pow(2.0f, offset / 12.0f);
    }
}
