#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    oscillator.initialise([](float x) { return std::sin(x); }, 128);
    oscillator.setFrequency(440.0f);
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
    
    float frequency = getFrequencyForNote(midiNoteNumber);
    oscillator.setFrequency(frequency);
    
    // Pitch wheel
    pitchBendSemitones = (currentPitchWheelPosition / 8192.0f - 1.0f) * 2.0f; // ±2 полутона
    
    adsr.noteOn();
    lfo.reset();
}

void SynthVoice::stopNote(float velocity, bool allowTailOff)
{
    juce::ignoreUnused(velocity);
    
    if (allowTailOff)
        adsr.noteOff();
    else
    {
        clearCurrentNote();
        adsr.reset();
    }
}

void SynthVoice::pitchWheelMoved(int newPitchWheelValue)
{
    pitchBendSemitones = (newPitchWheelValue / 8192.0f - 1.0f) * 2.0f;
}

void SynthVoice::controllerMoved(int controllerNumber, int newControllerValue)
{
    juce::ignoreUnused(controllerNumber, newControllerValue);
}

void SynthVoice::prepareToPlay(double sampleRate, int samplesPerBlock, int outputChannels)
{
    juce::ignoreUnused(samplesPerBlock, outputChannels);
    
    currentSampleRate = sampleRate;
    
    oscillator.prepare({ sampleRate, static_cast<juce::uint32>(outputChannels), static_cast<juce::uint32>(samplesPerBlock) });
    adsr.setSampleRate(sampleRate);
    lfo.prepare(sampleRate);
    filter.prepare(sampleRate);
    modMatrix.prepare(sampleRate);
}

void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    juce::AudioBuffer<float> tempBuffer(outputBuffer.getNumChannels(), numSamples);
    tempBuffer.clear();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // 1. Получаем значение LFO
        float lfoValue = lfo.processSample();

        // 2. Применяем pitch modulation от LFO
        float pitchMod = modMatrix.applyPitchModulation(lfoValue);
        float totalPitchSemitones = pitchBendSemitones + pitchMod;
        float frequencyMultiplier = std::pow(2.0f, totalPitchSemitones / 12.0f);
        
        float baseFrequency = getFrequencyForNote(currentMidiNote);
        oscillator.setFrequency(baseFrequency * frequencyMultiplier);

        // 3. Генерируем осциллятор
        float oscValue = oscillator.processSample(0.0f);

        // 4. Применяем filter modulation от LFO
        float modulatedCutoff = modMatrix.applyFilterModulation(filterCutoff, lfoValue);
        filter.setParameters(modulatedCutoff, filterResonance, static_cast<FilterEngine::Type>(filterType));
        
        float filteredL = oscValue;
        float filteredR = oscValue;
        filter.processSample(filteredL, filteredR);

        // 5. Применяем amp modulation от LFO (tremolo)
        float ampMod = modMatrix.applyAmpModulation(lfoValue);

        // 6. Применяем ADSR envelope
        float envelopeValue = adsr.getNextSample();

        // 7. Финальная амплитуда
        float sampleValue = filteredL * envelopeValue * currentVelocity * currentGain * ampMod;

        // Записываем в буфер (стерео)
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
        {
            tempBuffer.addSample(channel, sample, sampleValue);
        }
    }

    outputBuffer.addFrom(0, startSample, tempBuffer, 0, 0, numSamples);
    if (outputBuffer.getNumChannels() > 1)
        outputBuffer.addFrom(1, startSample, tempBuffer, 1, 0, numSamples);

    if (!adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::setOscillatorWaveform(int waveform)
{
    if (waveform == currentWaveform) return;
    currentWaveform = waveform;

    switch (waveform)
    {
        case 0: oscillator.initialise([](float x) { return std::sin(x); }, 128); break;
        case 1: oscillator.initialise([](float x) {
            float xWrapped = std::fmod(x, 2.0f * juce::MathConstants<float>::pi);
            if (xWrapped < 0.0f) xWrapped += 2.0f * juce::MathConstants<float>::pi;
            return (xWrapped / juce::MathConstants<float>::pi) - 1.0f;
        }, 128); break;
        case 2: oscillator.initialise([](float x) { return std::sin(x) > 0.0f ? 1.0f : -1.0f; }, 128); break;
        case 3: oscillator.initialise([](float x) {
            float xWrapped = std::fmod(x, 2.0f * juce::MathConstants<float>::pi);
            if (xWrapped < 0.0f) xWrapped += 2.0f * juce::MathConstants<float>::pi;
            return 2.0f * std::abs(xWrapped / juce::MathConstants<float>::pi - 1.0f) - 1.0f;
        }, 128); break;
    }
}

void SynthVoice::setADSR(float attack, float decay, float sustain, float release)
{
    adsrParams.attack = attack;
    adsrParams.decay = decay;
    adsrParams.sustain = sustain;
    adsrParams.release = release;
    adsr.setParameters(adsrParams);
}

void SynthVoice::setGain(float gainDb)
{
    currentGain = juce::Decibels::decibelsToGain(gainDb, -60.0f);
}

void SynthVoice::setLFOParameters(float rate, int waveform)
{
    lfoRate = rate;
    lfoWaveform = waveform;
    lfo.setRate(rate);
    lfo.setWaveform(static_cast<LFOEngine::Waveform>(waveform));
}

void SynthVoice::setFilterParameters(float cutoff, float resonance, int type)
{
    filterCutoff = cutoff;
    filterResonance = resonance;
    filterType = type;
}

void SynthVoice::setModulationAmounts(float pitchMod, float filterMod, float ampMod)
{
    pitchModAmount = pitchMod;
    filterModAmount = filterMod;
    ampModAmount = ampMod;
    modMatrix.setModulationAmounts(pitchMod, filterMod, ampMod);
}

float SynthVoice::getFrequencyForNote(int noteNumber)
{
    return 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);
}
