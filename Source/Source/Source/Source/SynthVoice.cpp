#include "SynthVoice.h"

SynthVoice::SynthVoice()
{
    mainOsc.setType(OscillatorEngine::Type::Sine);
    mainOsc.setFrequency(440.0f);
    
    subOsc.setType(OscillatorEngine::Type::Square);
    subOsc.setSubOctave(-1);
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
    mainOsc.setFrequency(frequency);
    subOsc.setFrequency(frequency);
    
    // Pitch wheel
    pitchBendSemitones = (currentPitchWheelPosition / 8192.0f - 1.0f) * 2.0f;
    
    // Синхронизация фазы при новой ноте (для стабильности)
    mainOsc.syncPhase();
    subOsc.syncPhase();
    
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
    
    mainOsc.prepare(sampleRate);
    subOsc.prepare(sampleRate);
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
        // 1. LFO
        float lfoValue = lfo.processSample();

        // 2. Pitch modulation
        float pitchMod = modMatrix.applyPitchModulation(lfoValue);
        float totalPitchSemitones = pitchBendSemitones + pitchMod;
        float frequencyMultiplier = std::pow(2.0f, totalPitchSemitones / 12.0f);
        
        float baseFrequency = getFrequencyForNote(currentMidiNote);
        float modulatedFrequency = baseFrequency * frequencyMultiplier;
        
        mainOsc.setFrequency(modulatedFrequency);
        subOsc.setFrequency(modulatedFrequency);

        // 3. Main oscillator
        float mainSample = mainOsc.processSample();
        
        // 4. Sub oscillator (смешиваем с main)
        float subSample = subOsc.processSample();
        float oscOutput = mainSample * (1.0f - subMix) + subSample * subMix;

        // 5. Filter с LFO модуляцией
        float modulatedCutoff = modMatrix.applyFilterModulation(filterCutoff, lfoValue);
        filter.setParameters(modulatedCutoff, filterResonance, static_cast<FilterEngine::Type>(filterType));
        
        float filteredL = oscOutput;
        float filteredR = oscOutput;
        filter.processSample(filteredL, filteredR);

        // 6. Amp modulation (tremolo)
        float ampMod = modMatrix.applyAmpModulation(lfoValue);

        // 7. ADSR envelope
        float envelopeValue = adsr.getNextSample();

        // 8. Финальная амплитуда
        float sampleValue = filteredL * envelopeValue * currentVelocity * currentGain * ampMod;

        // Записываем в буфер (стерео)
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            tempBuffer.addSample(channel, sample, sampleValue);
    }

    outputBuffer.addFrom(0, startSample, tempBuffer, 0, 0, numSamples);
    if (outputBuffer.getNumChannels() > 1)
        outputBuffer.addFrom(1, startSample, tempBuffer, 1, 0, numSamples);

    if (!adsr.isActive())
        clearCurrentNote();
}

void SynthVoice::setOscillatorType(int type)
{
    mainOscType = type;
    mainOsc.setType(static_cast<OscillatorEngine::Type>(type));
}

void SynthVoice::setPulseWidth(float width)
{
    pulseWidth = width;
    mainOsc.setPulseWidth(width);
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
    modMatrix.setModulationAmounts(pitchMod, filterMod, ampMod);
}

float SynthVoice::getFrequencyForNote(int noteNumber)
{
    return 440.0f * std::pow(2.0f, (noteNumber - 69) / 12.0f);
}
