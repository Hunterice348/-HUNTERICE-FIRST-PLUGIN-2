void SynthVoice::renderNextBlock(juce::AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
    juce::AudioBuffer<float> tempBuffer(outputBuffer.getNumChannels(), numSamples);
    tempBuffer.clear();

    for (int sample = 0; sample < numSamples; ++sample)
    {
        // 1. Portamento
        if (portamentoEnabled && std::abs(currentPitch - targetPitch) > 0.01f)
            currentPitch += (targetPitch - currentPitch) * (1.0f - portamentoCoeff);
        else
            currentPitch = targetPitch;
        
        // 2. Envelope и LFO
        float env1Value = ampEnvelope.getNextSample();
        float env2Value = filterEnvelope.getNextSample();
        float lfo1Value = lfo1.processSample();
        float lfo2Value = lfo2.processSample();
        
        // Key tracking
        float keyTracking = (currentPitch - 24.0f) / 96.0f;
        keyTracking = juce::jlimit(0.0f, 1.0f, keyTracking);
        
        // 3. Модуляция
        modMatrix.updateSources(env1Value, env2Value, lfo1Value, lfo2Value,
                                currentVelocity, keyTracking, modWheelValue);
        
        float pitchMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::Pitch);
        float filterMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::FilterCutoff);
        float pwmMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::PulseWidth);
        float ampMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::Amplitude);
        float detuneMod = modMatrix.getModulatedValue(ModulationMatrix::Destination::Detune);
        
        // 4. Расчёт частоты
        float baseFrequency = getFrequencyForNote(static_cast<int>(currentPitch));
        float fractionalPart = currentPitch - std::floor(currentPitch);
        float interpolatedFreq = baseFrequency * std::pow(2.0f, fractionalPart / 12.0f);
        
        // ИСПРАВЛЕНИЕ #3: Pitch bend применяется НА КАЖДОМ СЭМПЛЕ
        float totalPitchSemitones = pitchBendSemitones + pitchMod;
        float frequencyMultiplier = std::pow(2.0f, totalPitchSemitones / 12.0f);
        float finalFrequency = interpolatedFreq * frequencyMultiplier;
        
        // 5. Main oscillator
        mainOsc.setFrequency(finalFrequency);
        mainOsc.setModulatedPulseWidth(pwmMod);
        float mainSample = mainOsc.processSample();
        
        // 6. Unison
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
        
        // 7. Sub oscillator
        subOsc.setFrequency(finalFrequency);
        float subSample = subOsc.processSample();
        float oscOutput = mainSample * (1.0f - subMix) + subSample * subMix;
        
        // 8. Filter
        float env2Modulated = filterEnvAmount * env2Value * 10000.0f;
        float keyTrackModulated = filterKeyTrackAmount * keyTracking * 10000.0f;
        float lfoFilterMod = filterMod * 5000.0f;
        
        float modulatedCutoff = filterCutoff + env2Modulated + keyTrackModulated + lfoFilterMod;
        modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);
        
        filter.setParameters(modulatedCutoff, filterResonance, filterDrive,
                            static_cast<FilterEngine::Type>(filterType));
        
        float filteredL = oscOutput;
        float filteredR = oscOutput;
        filter.processSample(filteredL, filteredR);
        
        // 9. Amplitude modulation
        float totalAmpMod = env1Value * (1.0f + ampMod);
        totalAmpMod = juce::jlimit(0.0f, 1.0f, totalAmpMod);
        
        // 10. Финальная амплитуда
        float sampleValue = filteredL * totalAmpMod * currentVelocity * currentGain;
        
        // ИСПРАВЛЕНИЕ #4: Защита от клиппинга
        sampleValue = juce::jlimit(-0.99f, 0.99f, sampleValue);
        
        for (int channel = 0; channel < outputBuffer.getNumChannels(); ++channel)
            tempBuffer.addSample(channel, sample, sampleValue);
    }

    outputBuffer.addFrom(0, startSample, tempBuffer, 0, 0, numSamples);
    if (outputBuffer.getNumChannels() > 1)
        outputBuffer.addFrom(1, startSample, tempBuffer, 1, 0, numSamples);

    if (!ampEnvelope.isActive())
        clearCurrentNote();
}
