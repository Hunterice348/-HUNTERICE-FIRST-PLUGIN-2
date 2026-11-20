#pragma once

#include <juce_core/juce_core.h>
#include <vector>

namespace FactoryPresets
{
    struct PresetData
    {
        juce::String name;
        juce::String category;
        juce::String xml;
    };

    // Helper для генерации XML пресетов Audio Effect
    inline juce::String makeEffectPresetXml(float drive, float mix, float output, float tone,
                                            float bias, float threshold, float symmetry, int mode)
    {
        return juce::String(
            "<PARAMS>\n"
            "  <PARAM id=\"DRIVE\" value=\"") + drive + "\">\n"
            "  <PARAM id=\"MIX\" value=\"" + mix + "\">\n"
            "  <PARAM id=\"OUTPUT\" value=\"" + output + "\">\n"
            "  <PARAM id=\"TONE\" value=\"" + tone + "\">\n"
            "  <PARAM id=\"BIAS\" value=\"" + bias + "\">\n"
            "  <PARAM id=\"THRESHOLD\" value=\"" + threshold + "\">\n"
            "  <PARAM id=\"SYMMETRY\" value=\"" + symmetry + "\">\n"
            "  <PARAM id=\"DIST_MODE\" value=\"" + juce::String(mode) + "\">\n"
            "</PARAMS>";
    }

    // Helper для генерации XML пресетов Synthesizer
    inline juce::String makeSynthPresetXml(
        int oscType, float pulseWidth, float wtPos,
        int subType, int subOct, float subMix,
        float ampAttack, float ampDecay, float ampSustain, float ampRelease,
        float filtAttack, float filtDecay, float filtSustain, float filtRelease,
        float filtEnvAmt, float filtKeyTrack,
        float lfo1Rate, int lfo1Wave, float lfo2Rate, int lfo2Wave,
        float filterCutoff, float filterRes, float filterDrive, int filterType,
        int unisonVoices, float detune, bool portaEnabled, float portaTime,
        float volume,
        bool distEnabled, float distDrive, float distMix,
        bool satEnabled, float satDrive, float satMix,
        bool chorusEnabled, float chorusRate, float chorusDepth, float chorusMix,
        bool delayEnabled, float delayTime, float delayFeedback, float delayMix,
        bool reverbEnabled, float reverbSize, float reverbDamp, float reverbMix,
        bool phaserEnabled, float phaserRate, float phaserDepth, float phaserMix,
        float stereoWidth)
    {
        juce::String xml = "<PARAMS>\n";
        xml += "  <PARAM id=\"OSC_TYPE\" value=\"" + juce::String(oscType) + "\">\n";
        xml += "  <PARAM id=\"PULSE_WIDTH\" value=\"" + juce::String(pulseWidth) + "\">\n";
        xml += "  <PARAM id=\"WT_POS\" value=\"" + juce::String(wtPos) + "\">\n";
        xml += "  <PARAM id=\"SUB_TYPE\" value=\"" + juce::String(subType) + "\">\n";
        xml += "  <PARAM id=\"SUB_OCT\" value=\"" + juce::String(subOct) + "\">\n";
        xml += "  <PARAM id=\"SUB_MIX\" value=\"" + juce::String(subMix) + "\">\n";
        xml += "  <PARAM id=\"AMP_ATTACK\" value=\"" + juce::String(ampAttack) + "\">\n";
        xml += "  <PARAM id=\"AMP_DECAY\" value=\"" + juce::String(ampDecay) + "\">\n";
        xml += "  <PARAM id=\"AMP_SUSTAIN\" value=\"" + juce::String(ampSustain) + "\">\n";
        xml += "  <PARAM id=\"AMP_RELEASE\" value=\"" + juce::String(ampRelease) + "\">\n";
        xml += "  <PARAM id=\"FILT_ATTACK\" value=\"" + juce::String(filtAttack) + "\">\n";
        xml += "  <PARAM id=\"FILT_DECAY\" value=\"" + juce::String(filtDecay) + "\">\n";
        xml += "  <PARAM id=\"FILT_SUSTAIN\" value=\"" + juce::String(filtSustain) + "\">\n";
        xml += "  <PARAM id=\"FILT_RELEASE\" value=\"" + juce::String(filtRelease) + "\">\n";
        xml += "  <PARAM id=\"FILT_ENV_AMT\" value=\"" + juce::String(filtEnvAmt) + "\">\n";
        xml += "  <PARAM id=\"FILT_KEY_TRACK\" value=\"" + juce::String(filtKeyTrack) + "\">\n";
        xml += "  <PARAM id=\"LFO1_RATE\" value=\"" + juce::String(lfo1Rate) + "\">\n";
        xml += "  <PARAM id=\"LFO1_WAVE\" value=\"" + juce::String(lfo1Wave) + "\">\n";
        xml += "  <PARAM id=\"LFO2_RATE\" value=\"" + juce::String(lfo2Rate) + "\">\n";
        xml += "  <PARAM id=\"LFO2_WAVE\" value=\"" + juce::String(lfo2Wave) + "\">\n";
        xml += "  <PARAM id=\"FILTER_CUTOFF\" value=\"" + juce::String(filterCutoff) + "\">\n";
        xml += "  <PARAM id=\"FILTER_RES\" value=\"" + juce::String(filterRes) + "\">\n";
        xml += "  <PARAM id=\"FILTER_DRIVE\" value=\"" + juce::String(filterDrive) + "\">\n";
        xml += "  <PARAM id=\"FILTER_TYPE\" value=\"" + juce::String(filterType) + "\">\n";
        xml += "  <PARAM id=\"UNISON_VOICES\" value=\"" + juce::String(unisonVoices) + "\">\n";
        xml += "  <PARAM id=\"DETUNE\" value=\"" + juce::String(detune) + "\">\n";
        xml += "  <PARAM id=\"PORTA_ENABLED\" value=\"" + juce::String(portaEnabled ? 1 : 0) + "\">\n";
        xml += "  <PARAM id=\"PORTA_TIME\" value=\"" + juce::String(portaTime) + "\">\n";
        xml += "  <PARAM id=\"VOLUME\" value=\"" + juce::String(volume) + "\">\n";
        xml += "  <PARAM id=\"DIST_ENABLED\" value=\"" + juce::String(distEnabled ? 1 : 0) + "\">\n";
        xml += "  <PARAM id=\"DIST_DRIVE\" value=\"" + juce::String(distDrive) + "\">\n";
        xml += "  <PARAM id=\"DIST_MIX\" value=\"" + juce::String(distMix) + "\">\n";
        xml += "  <PARAM id=\"SAT_ENABLED\" value=\"" + juce::String(satEnabled ? 1 : 0) + "\">\n";
        xml += "  <PARAM id=\"SAT_DRIVE\" value=\"" + juce::String(satDrive) + "\">\n";
        xml += "  <PARAM id=\"SAT_MIX\" value=\"" + juce::String(satMix) + "\">\n";
        xml += "  <PARAM id=\"CHORUS_ENABLED\" value=\"" + juce::String(chorusEnabled ? 1 : 0) + "\">\n";
        xml += "  <PARAM id=\"CHORUS_RATE\" value=\"" + juce::String(chorusRate) + "\">\n";
        xml += "  <PARAM id=\"CHORUS_DEPTH\" value=\"" + juce::String(chorusDepth) + "\">\n";
        xml += "  <PARAM id=\"CHORUS_MIX\" value=\"" + juce::String(chorusMix) + "\">\n";
        xml += "  <PARAM id=\"DELAY_ENABLED\" value=\"" + juce::String(delayEnabled ? 1 : 0) + "\">\n";
        xml += "  <PARAM id=\"DELAY_TIME\" value=\"" + juce::String(delayTime) + "\">\n";
        xml += "  <PARAM id=\"DELAY_FEEDBACK\" value=\"" + juce::String(delayFeedback) + "\">\n";
        xml += "  <PARAM id=\"DELAY_MIX\" value=\"" + juce::String(delayMix) + "\">\n";
        xml += "  <PARAM id=\"REVERB_ENABLED\" value=\"" + juce::String(reverbEnabled ? 1 : 0) + "\">\n";
        xml += "  <PARAM id=\"REVERB_SIZE\" value=\"" + juce::String(reverbSize) + "\">\n";
        xml += "  <PARAM id=\"REVERB_DAMP\" value=\"" + juce::String(reverbDamp) + "\">\n";
        xml += "  <PARAM id=\"REVERB_MIX\" value=\"" + juce::String(reverbMix) + "\">\n";
        xml += "  <PARAM id=\"PHASER_ENABLED\" value=\"" + juce::String(phaserEnabled ? 1 : 0) + "\">\n";
        xml += "  <PARAM id=\"PHASER_RATE\" value=\"" + juce::String(phaserRate) + "\">\n";
        xml += "  <PARAM id=\"PHASER_DEPTH\" value=\"" + juce::String(phaserDepth) + "\">\n";
        xml += "  <PARAM id=\"PHASER_MIX\" value=\"" + juce::String(phaserMix) + "\">\n";
        xml += "  <PARAM id=\"STEREO_WIDTH\" value=\"" + juce::String(stereoWidth) + "\">\n";
        xml += "</PARAMS>";
        return xml;
    }

    // 100 пресетов для Audio Effect
    inline std::vector<PresetData> getEffectPresets()
    {
        std::vector<PresetData> presets;
        
        // Industrial Destruction (10 пресетов)
        presets.push_back({"Heavy Metal Crunch", "Industrial Destruction", makeEffectPresetXml(0.7f, 0.8f, -3.0f, 0.6f, 0.0f, 0.7f, 0.2f, 0)});
        presets.push_back({"Machine Grinder", "Industrial Destruction", makeEffectPresetXml(0.9f, 0.9f, -6.0f, 0.4f, 0.1f, 0.5f, 0.3f, 1)});
        presets.push_back({"Steel Crusher", "Industrial Destruction", makeEffectPresetXml(0.8f, 0.85f, -2.0f, 0.7f, -0.1f, 0.6f, 0.0f, 2)});
        presets.push_back({"Iron Fist", "Industrial Destruction", makeEffectPresetXml(0.95f, 0.95f, -8.0f, 0.3f, 0.2f, 0.4f, 0.5f, 3)});
        presets.push_back({"Concrete Breaker", "Industrial Destruction", makeEffectPresetXml(0.85f, 0.9f, -4.0f, 0.5f, 0.0f, 0.8f, -0.2f, 4)});
        presets.push_back({"Demolition Hammer", "Industrial Destruction", makeEffectPresetXml(1.0f, 1.0f, -10.0f, 0.2f, 0.3f, 0.3f, 0.4f, 5)});
        presets.push_back({"Rusty Chainsaw", "Industrial Destruction", makeEffectPresetXml(0.75f, 0.8f, -5.0f, 0.6f, -0.2f, 0.7f, 0.1f, 6)});
        presets.push_back({"Hydraulic Press", "Industrial Destruction", makeEffectPresetXml(0.9f, 0.85f, -7.0f, 0.4f, 0.1f, 0.5f, 0.0f, 7)});
        presets.push_back({"Wrecking Ball", "Industrial Destruction", makeEffectPresetXml(0.95f, 0.9f, -9.0f, 0.3f, 0.2f, 0.6f, 0.3f, 8)});
        presets.push_back({"Nuclear Meltdown", "Industrial Destruction", makeEffectPresetXml(1.0f, 1.0f, -12.0f, 0.1f, 0.4f, 0.2f, 0.6f, 9)});
        
        // Dark Saturation (10 пресетов)
        presets.push_back({"Warm Tape", "Dark Saturation", makeEffectPresetXml(0.3f, 0.6f, 0.0f, 0.8f, 0.0f, 0.9f, 0.0f, 2)});
        presets.push_back({"Vintage Tube", "Dark Saturation", makeEffectPresetXml(0.4f, 0.7f, -1.0f, 0.7f, 0.05f, 0.85f, 0.1f, 2)});
        presets.push_back({"Analog Warmth", "Dark Saturation", makeEffectPresetXml(0.35f, 0.65f, 0.5f, 0.75f, 0.0f, 0.88f, 0.05f, 2)});
        presets.push_back({"Midnight Glow", "Dark Saturation", makeEffectPresetXml(0.5f, 0.75f, -2.0f, 0.6f, 0.1f, 0.8f, 0.15f, 3)});
        presets.push_back({"Ember", "Dark Saturation", makeEffectPresetXml(0.45f, 0.7f, -1.5f, 0.65f, 0.0f, 0.82f, 0.1f, 3)});
        presets.push_back({"Shadow", "Dark Saturation", makeEffectPresetXml(0.55f, 0.8f, -3.0f, 0.5f, 0.15f, 0.75f, 0.2f, 4)});
        presets.push_back({"Velvet", "Dark Saturation", makeEffectPresetXml(0.4f, 0.68f, 0.0f, 0.72f, 0.0f, 0.86f, 0.08f, 2)});
        presets.push_back({"Obsidian", "Dark Saturation", makeEffectPresetXml(0.6f, 0.85f, -4.0f, 0.4f, 0.2f, 0.7f, 0.25f, 5)});
        presets.push_back({"Charcoal", "Dark Saturation", makeEffectPresetXml(0.5f, 0.78f, -2.5f, 0.55f, 0.1f, 0.78f, 0.18f, 3)});
        presets.push_back({"Abyss", "Dark Saturation", makeEffectPresetXml(0.65f, 0.9f, -5.0f, 0.3f, 0.25f, 0.65f, 0.3f, 6)});
        
        // Metallic Noise (10 пресетов)
        presets.push_back({"Rusty Metal", "Metallic Noise", makeEffectPresetXml(0.6f, 0.7f, -3.0f, 0.9f, 0.0f, 0.5f, 0.0f, 0)});
        presets.push_back({"Chrome", "Metallic Noise", makeEffectPresetXml(0.7f, 0.75f, -2.0f, 0.95f, 0.0f, 0.6f, 0.1f, 1)});
        presets.push_back({"Steel Wire", "Metallic Noise", makeEffectPresetXml(0.65f, 0.72f, -2.5f, 0.92f, 0.05f, 0.55f, 0.05f, 2)});
        presets.push_back({"Aluminum", "Metallic Noise", makeEffectPresetXml(0.75f, 0.78f, -3.5f, 0.88f, 0.1f, 0.65f, 0.15f, 3)});
        presets.push_back({"Copper", "Metallic Noise", makeEffectPresetXml(0.68f, 0.74f, -2.8f, 0.85f, 0.0f, 0.58f, 0.08f, 4)});
        presets.push_back({"Brass", "Metallic Noise", makeEffectPresetXml(0.72f, 0.76f, -3.2f, 0.82f, 0.08f, 0.62f, 0.12f, 5)});
        presets.push_back({"Tin", "Metallic Noise", makeEffectPresetXml(0.8f, 0.82f, -4.0f, 0.78f, 0.15f, 0.7f, 0.2f, 6)});
        presets.push_back({"Silver", "Metallic Noise", makeEffectPresetXml(0.78f, 0.8f, -3.8f, 0.8f, 0.12f, 0.68f, 0.18f, 7)});
        presets.push_back({"Gold", "Metallic Noise", makeEffectPresetXml(0.82f, 0.84f, -4.5f, 0.75f, 0.2f, 0.72f, 0.25f, 8)});
        presets.push_back({"Platinum", "Metallic Noise", makeEffectPresetXml(0.85f, 0.88f, -5.0f, 0.7f, 0.25f, 0.75f, 0.3f, 9)});
        
        // Extreme Fuzz (10 пресетов)
        presets.push_back({"Fuzz Face", "Extreme Fuzz", makeEffectPresetXml(0.9f, 0.95f, -6.0f, 0.5f, 0.0f, 0.3f, 0.0f, 4)});
        presets.push_back({"Big Muff", "Extreme Fuzz", makeEffectPresetXml(0.95f, 1.0f, -8.0f, 0.4f, 0.1f, 0.25f, 0.1f, 4)});
        presets.push_back({"Tone Bender", "Extreme Fuzz", makeEffectPresetXml(0.88f, 0.92f, -5.0f, 0.55f, 0.05f, 0.35f, 0.05f, 4)});
        presets.push_back({"Fuzz War", "Extreme Fuzz", makeEffectPresetXml(1.0f, 1.0f, -10.0f, 0.3f, 0.2f, 0.2f, 0.2f, 4)});
        presets.push_back({"Fuzz Lord", "Extreme Fuzz", makeEffectPresetXml(0.92f, 0.96f, -7.0f, 0.45f, 0.15f, 0.28f, 0.15f, 4)});
        presets.push_back({"Fuzz King", "Extreme Fuzz", makeEffectPresetXml(0.94f, 0.98f, -9.0f, 0.35f, 0.18f, 0.22f, 0.18f, 4)});
        presets.push_back({"Fuzz Emperor", "Extreme Fuzz", makeEffectPresetXml(0.96f, 1.0f, -11.0f, 0.25f, 0.22f, 0.18f, 0.22f, 4)});
        presets.push_back({"Fuzz Titan", "Extreme Fuzz", makeEffectPresetXml(0.98f, 1.0f, -12.0f, 0.2f, 0.25f, 0.15f, 0.25f, 4)});
        presets.push_back({"Fuzz God", "Extreme Fuzz", makeEffectPresetXml(1.0f, 1.0f, -14.0f, 0.15f, 0.3f, 0.12f, 0.3f, 4)});
        presets.push_back({"Fuzz Apocalypse", "Extreme Fuzz", makeEffectPresetXml(1.0f, 1.0f, -16.0f, 0.1f, 0.35f, 0.1f, 0.35f, 4)});
        
        // Cybernetic Distortion (10 пресетов)
        presets.push_back({"Digital Decay", "Cybernetic Distortion", makeEffectPresetXml(0.7f, 0.8f, -4.0f, 0.3f, 0.0f, 0.4f, 0.0f, 8)});
        presets.push_back({"Bit Crusher", "Cybernetic Distortion", makeEffectPresetXml(0.8f, 0.85f, -5.0f, 0.25f, 0.1f, 0.35f, 0.1f, 8)});
        presets.push_back({"Glitch Core", "Cybernetic Distortion", makeEffectPresetXml(0.85f, 0.9f, -6.0f, 0.2f, 0.15f, 0.3f, 0.15f, 8)});
        presets.push_back({"Data Corruption", "Cybernetic Distortion", makeEffectPresetXml(0.9f, 0.95f, -7.0f, 0.15f, 0.2f, 0.25f, 0.2f, 8)});
        presets.push_back({"Circuit Break", "Cybernetic Distortion", makeEffectPresetXml(0.95f, 1.0f, -8.0f, 0.1f, 0.25f, 0.2f, 0.25f, 8)});
        presets.push_back({"Neural Net", "Cybernetic Distortion", makeEffectPresetXml(0.75f, 0.82f, -4.5f, 0.28f, 0.08f, 0.38f, 0.08f, 8)});
        presets.push_back({"Quantum Flux", "Cybernetic Distortion", makeEffectPresetXml(0.82f, 0.88f, -5.5f, 0.22f, 0.12f, 0.32f, 0.12f, 8)});
        presets.push_back({"Binary Storm", "Cybernetic Distortion", makeEffectPresetXml(0.88f, 0.92f, -6.5f, 0.18f, 0.18f, 0.28f, 0.18f, 8)});
        presets.push_back({"Matrix Error", "Cybernetic Distortion", makeEffectPresetXml(0.92f, 0.96f, -7.5f, 0.12f, 0.22f, 0.22f, 0.22f, 8)});
        presets.push_back({"System Crash", "Cybernetic Distortion", makeEffectPresetXml(1.0f, 1.0f, -9.0f, 0.08f, 0.3f, 0.15f, 0.3f, 8)});
        
        // Broken Digital (10 пресетов)
        presets.push_back({"Pixel Dust", "Broken Digital", makeEffectPresetXml(0.6f, 0.7f, -3.0f, 0.15f, 0.0f, 0.6f, 0.0f, 8)});
        presets.push_back({"Screen Tear", "Broken Digital", makeEffectPresetXml(0.7f, 0.75f, -4.0f, 0.12f, 0.1f, 0.55f, 0.1f, 8)});
        presets.push_back({"Buffer Overflow", "Broken Digital", makeEffectPresetXml(0.8f, 0.8f, -5.0f, 0.1f, 0.15f, 0.5f, 0.15f, 8)});
        presets.push_back({"Memory Leak", "Broken Digital", makeEffectPresetXml(0.85f, 0.85f, -6.0f, 0.08f, 0.2f, 0.45f, 0.2f, 8)});
        presets.push_back({"Stack Overflow", "Broken Digital", makeEffectPresetXml(0.9f, 0.9f, -7.0f, 0.05f, 0.25f, 0.4f, 0.25f, 8)});
        presets.push_back({"Kernel Panic", "Broken Digital", makeEffectPresetXml(0.95f, 0.95f, -8.0f, 0.03f, 0.3f, 0.35f, 0.3f, 8)});
        presets.push_back({"Blue Screen", "Broken Digital", makeEffectPresetXml(1.0f, 1.0f, -9.0f, 0.02f, 0.35f, 0.3f, 0.35f, 8)});
        presets.push_back({"Dead Pixel", "Broken Digital", makeEffectPresetXml(0.65f, 0.72f, -3.5f, 0.14f, 0.05f, 0.58f, 0.05f, 8)});
        presets.push_back({"Corrupt File", "Broken Digital", makeEffectPresetXml(0.75f, 0.78f, -4.5f, 0.11f, 0.12f, 0.52f, 0.12f, 8)});
        presets.push_back({"Fatal Error", "Broken Digital", makeEffectPresetXml(0.98f, 0.98f, -10.0f, 0.01f, 0.4f, 0.25f, 0.4f, 8)});
        
        // Industrial Drum (10 пресетов)
        presets.push_back({"Kick Crusher", "Industrial Drum", makeEffectPresetXml(0.5f, 0.6f, 0.0f, 0.9f, 0.0f, 0.7f, 0.0f, 0)});
        presets.push_back({"Snare Smasher", "Industrial Drum", makeEffectPresetXml(0.6f, 0.7f, -1.0f, 0.85f, 0.05f, 0.65f, 0.1f, 1)});
        presets.push_back({"Hi-Hat Shredder", "Industrial Drum", makeEffectPresetXml(0.7f, 0.8f, -2.0f, 0.8f, 0.1f, 0.6f, 0.2f, 2)});
        presets.push_back({"Tom Pulverizer", "Industrial Drum", makeEffectPresetXml(0.65f, 0.75f, -1.5f, 0.82f, 0.08f, 0.62f, 0.15f, 3)});
        presets.push_back({"Cymbal Destroyer", "Industrial Drum", makeEffectPresetXml(0.75f, 0.82f, -2.5f, 0.78f, 0.12f, 0.58f, 0.18f, 4)});
        presets.push_back({"Percussion Grinder", "Industrial Drum", makeEffectPresetXml(0.68f, 0.76f, -1.8f, 0.8f, 0.1f, 0.6f, 0.16f, 5)});
        presets.push_back({"Rhythm Annihilator", "Industrial Drum", makeEffectPresetXml(0.72f, 0.78f, -2.2f, 0.76f, 0.14f, 0.56f, 0.2f, 6)});
        presets.push_back({"Beat Obliterator", "Industrial Drum", makeEffectPresetXml(0.78f, 0.84f, -2.8f, 0.72f, 0.18f, 0.52f, 0.22f, 7)});
        presets.push_back({"Groove Demolisher", "Industrial Drum", makeEffectPresetXml(0.82f, 0.88f, -3.2f, 0.68f, 0.22f, 0.48f, 0.25f, 8)});
        presets.push_back({"Tempo Wrecker", "Industrial Drum", makeEffectPresetXml(0.88f, 0.92f, -3.8f, 0.65f, 0.25f, 0.45f, 0.28f, 9)});
        
        // Dark Ambient (10 пресетов)
        presets.push_back({"Shadow Weaver", "Dark Ambient", makeEffectPresetXml(0.2f, 0.4f, 0.0f, 0.95f, 0.0f, 0.95f, 0.0f, 2)});
        presets.push_back({"Void Walker", "Dark Ambient", makeEffectPresetXml(0.25f, 0.45f, -1.0f, 0.92f, 0.05f, 0.92f, 0.05f, 2)});
        presets.push_back({"Nightmare", "Dark Ambient", makeEffectPresetXml(0.3f, 0.5f, -2.0f, 0.9f, 0.1f, 0.9f, 0.1f, 3)});
        presets.push_back({"Abyssal", "Dark Ambient", makeEffectPresetXml(0.35f, 0.55f, -3.0f, 0.88f, 0.15f, 0.88f, 0.15f, 3)});
        presets.push_back({"Ethereal", "Dark Ambient", makeEffectPresetXml(0.28f, 0.48f, -1.5f, 0.91f, 0.08f, 0.91f, 0.08f, 2)});
        presets.push_back({"Haunting", "Dark Ambient", makeEffectPresetXml(0.32f, 0.52f, -2.5f, 0.89f, 0.12f, 0.89f, 0.12f, 3)});
        presets.push_back({"Spectral", "Dark Ambient", makeEffectPresetXml(0.38f, 0.58f, -3.5f, 0.86f, 0.18f, 0.86f, 0.18f, 4)});
        presets.push_back({"Phantom", "Dark Ambient", makeEffectPresetXml(0.42f, 0.62f, -4.0f, 0.84f, 0.22f, 0.84f, 0.22f, 4)});
        presets.push_back({"Wraith", "Dark Ambient", makeEffectPresetXml(0.45f, 0.65f, -4.5f, 0.82f, 0.25f, 0.82f, 0.25f, 5)});
        presets.push_back({"Demon", "Dark Ambient", makeEffectPresetXml(0.5f, 0.7f, -5.0f, 0.8f, 0.3f, 0.8f, 0.3f, 5)});
        
        // Mechanical Feedback (10 пресетов)
        presets.push_back({"Machine Howl", "Mechanical Feedback", makeEffectPresetXml(0.7f, 0.8f, -4.0f, 0.6f, 0.2f, 0.85f, 0.3f, 5)});
        presets.push_back({"Engine Roar", "Mechanical Feedback", makeEffectPresetXml(0.75f, 0.85f, -5.0f, 0.55f, 0.25f, 0.82f, 0.35f, 5)});
        presets.push_back({"Gear Grind", "Mechanical Feedback", makeEffectPresetXml(0.8f, 0.9f, -6.0f, 0.5f, 0.3f, 0.8f, 0.4f, 6)});
        presets.push_back({"Piston Slam", "Mechanical Feedback", makeEffectPresetXml(0.85f, 0.95f, -7.0f, 0.45f, 0.35f, 0.78f, 0.45f, 6)});
        presets.push_back({"Valve Squeal", "Mechanical Feedback", makeEffectPresetXml(0.9f, 1.0f, -8.0f, 0.4f, 0.4f, 0.75f, 0.5f, 7)});
        presets.push_back({"Turbine Whine", "Mechanical Feedback", makeEffectPresetXml(0.72f, 0.82f, -4.5f, 0.58f, 0.22f, 0.84f, 0.32f, 5)});
        presets.push_back({"Compressor Hiss", "Mechanical Feedback", makeEffectPresetXml(0.78f, 0.88f, -5.5f, 0.52f, 0.28f, 0.81f, 0.38f, 6)});
        presets.push_back({"Drone Hum", "Mechanical Feedback", makeEffectPresetXml(0.82f, 0.92f, -6.5f, 0.48f, 0.32f, 0.79f, 0.42f, 6)});
        presets.push_back({"Motor Buzz", "Mechanical Feedback", makeEffectPresetXml(0.88f, 0.96f, -7.5f, 0.42f, 0.38f, 0.76f, 0.48f, 7)});
        presets.push_back({"Feedback Loop", "Mechanical Feedback", makeEffectPresetXml(0.95f, 1.0f, -9.0f, 0.35f, 0.45f, 0.72f, 0.55f, 7)});
        
        // Gothic Texture (10 пресетов)
        presets.push_back({"Cathedral", "Gothic Texture", makeEffectPresetXml(0.4f, 0.6f, -2.0f, 0.85f, 0.0f, 0.75f, 0.0f, 2)});
        presets.push_back({"Crypt", "Gothic Texture", makeEffectPresetXml(0.45f, 0.65f, -3.0f, 0.82f, 0.05f, 0.72f, 0.05f, 3)});
        presets.push_back({"Dungeon", "Gothic Texture", makeEffectPresetXml(0.5f, 0.7f, -4.0f, 0.8f, 0.1f, 0.7f, 0.1f, 3)});
        presets.push_back({"Castle", "Gothic Texture", makeEffectPresetXml(0.55f, 0.75f, -5.0f, 0.78f, 0.15f, 0.68f, 0.15f, 4)});
        presets.push_back({"Abbey", "Gothic Texture", makeEffectPresetXml(0.6f, 0.8f, -6.0f, 0.75f, 0.2f, 0.65f, 0.2f, 4)});
        presets.push_back({"Chapel", "Gothic Texture", makeEffectPresetXml(0.42f, 0.62f, -2.5f, 0.84f, 0.08f, 0.74f, 0.08f, 2)});
        presets.push_back({"Monastery", "Gothic Texture", makeEffectPresetXml(0.48f, 0.68f, -3.5f, 0.81f, 0.12f, 0.71f, 0.12f, 3)});
        presets.push_back({"Sanctuary", "Gothic Texture", makeEffectPresetXml(0.52f, 0.72f, -4.5f, 0.79f, 0.18f, 0.69f, 0.18f, 4)});
        presets.push_back({"Temple", "Gothic Texture", makeEffectPresetXml(0.58f, 0.78f, -5.5f, 0.76f, 0.22f, 0.66f, 0.22f, 5)});
        presets.push_back({"Ruins", "Gothic Texture", makeEffectPresetXml(0.65f, 0.85f, -7.0f, 0.72f, 0.28f, 0.62f, 0.28f, 5)});
        
        return presets;
    }

    // 100 пресетов для Synthesizer
    inline std::vector<PresetData> getSynthPresets()
    {
        std::vector<PresetData> presets;
        
        // Dark Bass (10 пресетов)
        presets.push_back({"Sub Terror", "Dark Bass", makeSynthPresetXml(
            2, 0.5f, 0.0f, 3, -1, 0.8f, 5.0f, 100.0f, 0.9f, 200.0f,
            10.0f, 200.0f, 0.5f, 300.0f, 0.8f, 0.3f, 2.0f, 0, 5.0f, 0,
            200.0f, 0.6f, 0.3f, 0, 1, 0.0f, false, 50.0f, -6.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Industrial Bass", "Dark Bass", makeSynthPresetXml(
            2, 0.5f, 0.0f, 3, -1, 0.7f, 3.0f, 80.0f, 0.85f, 150.0f,
            8.0f, 150.0f, 0.6f, 250.0f, 0.9f, 0.4f, 3.0f, 1, 6.0f, 1,
            300.0f, 0.7f, 0.4f, 3, 1, 0.0f, false, 50.0f, -3.0f,
            true, 0.5f, 0.6f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Deep Sub", "Dark Bass", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -2, 0.9f, 10.0f, 150.0f, 0.95f, 300.0f,
            15.0f, 250.0f, 0.7f, 400.0f, 0.6f, 0.2f, 1.0f, 0, 3.0f, 0,
            150.0f, 0.4f, 0.2f, 0, 1, 0.0f, false, 50.0f, -9.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Acid Bass", "Dark Bass", makeSynthPresetXml(
            2, 0.3f, 0.0f, 3, -1, 0.6f, 2.0f, 60.0f, 0.8f, 120.0f,
            5.0f, 120.0f, 0.4f, 200.0f, 1.0f, 0.5f, 4.0f, 2, 8.0f, 2,
            500.0f, 0.9f, 0.5f, 4, 1, 0.0f, false, 50.0f, -3.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Wobble Bass", "Dark Bass", makeSynthPresetXml(
            2, 0.5f, 0.0f, 3, -1, 0.75f, 4.0f, 90.0f, 0.88f, 180.0f,
            12.0f, 180.0f, 0.55f, 280.0f, 0.95f, 0.45f, 6.0f, 0, 10.0f, 0,
            400.0f, 0.8f, 0.45f, 0, 1, 0.0f, false, 50.0f, -4.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Reese Bass", "Dark Bass", makeSynthPresetXml(
            2, 0.5f, 0.0f, 2, -1, 0.7f, 8.0f, 120.0f, 0.9f, 250.0f,
            10.0f, 200.0f, 0.65f, 350.0f, 0.7f, 0.35f, 2.5f, 1, 5.0f, 1,
            350.0f, 0.65f, 0.35f, 0, 3, 15.0f, false, 50.0f, -5.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.2f)});
        
        presets.push_back({"FM Bass", "Dark Bass", makeSynthPresetXml(
            7, 0.5f, 0.5f, 3, -1, 0.65f, 3.0f, 70.0f, 0.82f, 140.0f,
            6.0f, 130.0f, 0.5f, 220.0f, 0.85f, 0.4f, 3.5f, 3, 7.0f, 3,
            280.0f, 0.75f, 0.4f, 3, 1, 0.0f, false, 50.0f, -4.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Growl Bass", "Dark Bass", makeSynthPresetXml(
            2, 0.4f, 0.0f, 3, -1, 0.8f, 2.0f, 50.0f, 0.75f, 100.0f,
            4.0f, 100.0f, 0.45f, 180.0f, 1.0f, 0.6f, 5.0f, 4, 9.0f, 4,
            600.0f, 0.95f, 0.6f, 4, 1, 0.0f, false, 50.0f, -2.0f,
            true, 0.7f, 0.8f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"808 Bass", "Dark Bass", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -2, 0.95f, 1.0f, 500.0f, 1.0f, 800.0f,
            5.0f, 500.0f, 0.8f, 600.0f, 0.5f, 0.1f, 1.5f, 0, 3.0f, 0,
            100.0f, 0.3f, 0.1f, 0, 1, 0.0f, false, 50.0f, -6.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Dark Matter", "Dark Bass", makeSynthPresetXml(
            2, 0.5f, 0.0f, 3, -2, 0.85f, 6.0f, 110.0f, 0.92f, 220.0f,
            12.0f, 220.0f, 0.7f, 380.0f, 0.9f, 0.5f, 4.0f, 5, 8.0f, 5,
            250.0f, 0.85f, 0.5f, 3, 2, 20.0f, true, 80.0f, -5.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        // Industrial Lead (10 пресетов)
        presets.push_back({"Cyber Lead", "Industrial Lead", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 5.0f, 100.0f, 0.8f, 200.0f,
            10.0f, 200.0f, 0.6f, 300.0f, 0.7f, 0.4f, 3.0f, 1, 6.0f, 1,
            2000.0f, 0.5f, 0.3f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.6f, 0.7f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Saw Lead", "Industrial Lead", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 3.0f, 80.0f, 0.85f, 150.0f,
            8.0f, 150.0f, 0.65f, 250.0f, 0.8f, 0.45f, 4.0f, 2, 8.0f, 2,
            3000.0f, 0.6f, 0.4f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Square Lead", "Industrial Lead", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 4.0f, 90.0f, 0.82f, 180.0f,
            9.0f, 180.0f, 0.62f, 280.0f, 0.75f, 0.42f, 3.5f, 3, 7.0f, 3,
            2500.0f, 0.55f, 0.35f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Pulse Lead", "Industrial Lead", makeSynthPresetXml(
            4, 0.3f, 0.0f, 0, 0, 0.0f, 3.0f, 70.0f, 0.78f, 140.0f,
            7.0f, 140.0f, 0.58f, 220.0f, 0.85f, 0.48f, 5.0f, 4, 10.0f, 4,
            2800.0f, 0.65f, 0.45f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Unison Lead", "Industrial Lead", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 4.0f, 85.0f, 0.83f, 170.0f,
            8.0f, 170.0f, 0.63f, 260.0f, 0.78f, 0.44f, 3.0f, 1, 6.0f, 1,
            2200.0f, 0.58f, 0.38f, 0, 4, 25.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Detune Lead", "Industrial Lead", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 5.0f, 95.0f, 0.86f, 190.0f,
            9.0f, 190.0f, 0.66f, 290.0f, 0.82f, 0.46f, 3.5f, 2, 7.0f, 2,
            2400.0f, 0.62f, 0.42f, 0, 3, 35.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"PWM Lead", "Industrial Lead", makeSynthPresetXml(
            4, 0.5f, 0.0f, 0, 0, 0.0f, 4.0f, 88.0f, 0.84f, 175.0f,
            8.0f, 175.0f, 0.64f, 270.0f, 0.8f, 0.45f, 4.0f, 6, 8.0f, 6,
            2600.0f, 0.6f, 0.4f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Filter Lead", "Industrial Lead", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 3.0f, 75.0f, 0.8f, 150.0f,
            15.0f, 250.0f, 0.7f, 350.0f, 1.0f, 0.5f, 2.0f, 0, 4.0f, 0,
            1500.0f, 0.7f, 0.5f, 3, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Distorted Lead", "Industrial Lead", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 4.0f, 82.0f, 0.81f, 165.0f,
            8.0f, 165.0f, 0.61f, 255.0f, 0.76f, 0.43f, 3.0f, 1, 6.0f, 1,
            2100.0f, 0.56f, 0.36f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.5f, 0.6f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Lead Machine", "Industrial Lead", makeSynthPresetXml(
            2, 0.5f, 0.0f, 3, -1, 0.4f, 5.0f, 100.0f, 0.85f, 200.0f,
            10.0f, 200.0f, 0.65f, 300.0f, 0.85f, 0.48f, 4.0f, 3, 8.0f, 3,
            2800.0f, 0.68f, 0.48f, 3, 2, 18.0f, false, 50.0f, 0.0f,
            true, 0.65f, 0.75f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        // Metallic Drone (10 пресетов)
        presets.push_back({"Steel Drone", "Metallic Drone", makeSynthPresetXml(
            2, 0.5f, 0.0f, 2, -1, 0.6f, 500.0f, 1000.0f, 1.0f, 2000.0f,
            500.0f, 1000.0f, 0.9f, 2000.0f, 0.3f, 0.2f, 0.5f, 0, 1.0f, 0,
            800.0f, 0.4f, 0.2f, 0, 1, 0.0f, false, 50.0f, -3.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.5f, 0.6f, 0.4f, false, 0.0f, 0.0f, 0.0f, 1.3f)});
        
        presets.push_back({"Iron Drone", "Metallic Drone", makeSynthPresetXml(
            3, 0.5f, 0.0f, 3, -1, 0.55f, 600.0f, 1200.0f, 1.0f, 2500.0f,
            600.0f, 1200.0f, 0.92f, 2500.0f, 0.35f, 0.25f, 0.6f, 1, 1.2f, 1,
            900.0f, 0.45f, 0.25f, 0, 1, 0.0f, false, 50.0f, -4.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.6f, 0.7f, 0.5f, false, 0.0f, 0.0f, 0.0f, 1.4f)});
        
        presets.push_back({"Chrome Drone", "Metallic Drone", makeSynthPresetXml(
            2, 0.5f, 0.0f, 2, -2, 0.65f, 700.0f, 1400.0f, 1.0f, 3000.0f,
            700.0f, 1400.0f, 0.95f, 3000.0f, 0.4f, 0.3f, 0.7f, 2, 1.4f, 2,
            1000.0f, 0.5f, 0.3f, 0, 1, 0.0f, false, 50.0f, -5.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.7f, 0.8f, 0.6f, false, 0.0f, 0.0f, 0.0f, 1.5f)});
        
        presets.push_back({"Aluminum Drone", "Metallic Drone", makeSynthPresetXml(
            2, 0.5f, 0.0f, 2, -1, 0.58f, 550.0f, 1100.0f, 1.0f, 2200.0f,
            550.0f, 1100.0f, 0.91f, 2200.0f, 0.32f, 0.22f, 0.55f, 3, 1.1f, 3,
            850.0f, 0.42f, 0.22f, 0, 1, 0.0f, false, 50.0f, -3.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.55f, 0.65f, 0.45f, false, 0.0f, 0.0f, 0.0f, 1.35f)});
        
        presets.push_back({"Copper Drone", "Metallic Drone", makeSynthPresetXml(
            3, 0.5f, 0.0f, 3, -1, 0.62f, 650.0f, 1300.0f, 1.0f, 2600.0f,
            650.0f, 1300.0f, 0.93f, 2600.0f, 0.38f, 0.28f, 0.65f, 4, 1.3f, 4,
            950.0f, 0.48f, 0.28f, 0, 1, 0.0f, false, 50.0f, -4.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.65f, 0.75f, 0.55f, false, 0.0f, 0.0f, 0.0f, 1.45f)});
        
        presets.push_back({"Brass Drone", "Metallic Drone", makeSynthPresetXml(
            2, 0.5f, 0.0f, 2, -2, 0.68f, 750.0f, 1500.0f, 1.0f, 3200.0f,
            750.0f, 1500.0f, 0.96f, 3200.0f, 0.42f, 0.32f, 0.75f, 5, 1.5f, 5,
            1100.0f, 0.52f, 0.32f, 0, 1, 0.0f, false, 50.0f, -5.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.75f, 0.85f, 0.65f, false, 0.0f, 0.0f, 0.0f, 1.55f)});
        
        presets.push_back({"Tin Drone", "Metallic Drone", makeSynthPresetXml(
            3, 0.5f, 0.0f, 3, -1, 0.56f, 580.0f, 1160.0f, 1.0f, 2320.0f,
            580.0f, 1160.0f, 0.9f, 2320.0f, 0.33f, 0.23f, 0.58f, 6, 1.16f, 6,
            880.0f, 0.43f, 0.23f, 0, 1, 0.0f, false, 50.0f, -3.8f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.58f, 0.68f, 0.48f, false, 0.0f, 0.0f, 0.0f, 1.38f)});
        
        presets.push_back({"Silver Drone", "Metallic Drone", makeSynthPresetXml(
            2, 0.5f, 0.0f, 2, -1, 0.64f, 680.0f, 1360.0f, 1.0f, 2720.0f,
            680.0f, 1360.0f, 0.94f, 2720.0f, 0.39f, 0.29f, 0.68f, 0, 1.36f, 0,
            980.0f, 0.49f, 0.29f, 0, 1, 0.0f, false, 50.0f, -4.8f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.68f, 0.78f, 0.58f, false, 0.0f, 0.0f, 0.0f, 1.48f)});
        
        presets.push_back({"Gold Drone", "Metallic Drone", makeSynthPresetXml(
            3, 0.5f, 0.0f, 3, -2, 0.7f, 800.0f, 1600.0f, 1.0f, 3400.0f,
            800.0f, 1600.0f, 0.97f, 3400.0f, 0.44f, 0.34f, 0.8f, 1, 1.6f, 1,
            1200.0f, 0.54f, 0.34f, 0, 1, 0.0f, false, 50.0f, -6.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.8f, 0.9f, 0.7f, false, 0.0f, 0.0f, 0.0f, 1.6f)});
        
        presets.push_back({"Platinum Drone", "Metallic Drone", makeSynthPresetXml(
            2, 0.5f, 0.0f, 2, -1, 0.72f, 850.0f, 1700.0f, 1.0f, 3600.0f,
            850.0f, 1700.0f, 0.98f, 3600.0f, 0.46f, 0.36f, 0.85f, 2, 1.7f, 2,
            1300.0f, 0.56f, 0.36f, 0, 1, 0.0f, false, 50.0f, -6.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.85f, 0.95f, 0.75f, false, 0.0f, 0.0f, 0.0f, 1.65f)});
        
        // Cybernetic Pluck (10 пресетов)
        presets.push_back({"Digital Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 1.0f, 50.0f, 0.0f, 100.0f,
            2.0f, 80.0f, 0.0f, 150.0f, 0.9f, 0.6f, 8.0f, 0, 16.0f, 0,
            3000.0f, 0.7f, 0.5f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Cyber Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1.5f, 60.0f, 0.0f, 120.0f,
            3.0f, 90.0f, 0.0f, 180.0f, 0.95f, 0.65f, 10.0f, 1, 20.0f, 1,
            3500.0f, 0.75f, 0.55f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Neon Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 2.0f, 70.0f, 0.0f, 140.0f,
            4.0f, 100.0f, 0.0f, 200.0f, 1.0f, 0.7f, 12.0f, 2, 24.0f, 2,
            4000.0f, 0.8f, 0.6f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Laser Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            4, 0.3f, 0.0f, 0, 0, 0.0f, 1.0f, 40.0f, 0.0f, 80.0f,
            2.0f, 70.0f, 0.0f, 120.0f, 0.85f, 0.55f, 15.0f, 3, 30.0f, 3,
            5000.0f, 0.85f, 0.65f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Glitch Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 0.5f, 30.0f, 0.0f, 60.0f,
            1.0f, 50.0f, 0.0f, 100.0f, 0.8f, 0.5f, 20.0f, 4, 40.0f, 4,
            6000.0f, 0.9f, 0.7f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Circuit Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 1.8f, 65.0f, 0.0f, 130.0f,
            3.5f, 95.0f, 0.0f, 190.0f, 0.92f, 0.62f, 11.0f, 5, 22.0f, 5,
            3800.0f, 0.78f, 0.58f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Binary Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1.2f, 55.0f, 0.0f, 110.0f,
            2.5f, 85.0f, 0.0f, 170.0f, 0.88f, 0.58f, 13.0f, 6, 26.0f, 6,
            4200.0f, 0.82f, 0.62f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Quantum Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 2.2f, 75.0f, 0.0f, 150.0f,
            4.5f, 110.0f, 0.0f, 220.0f, 1.0f, 0.75f, 14.0f, 0, 28.0f, 0,
            4500.0f, 0.88f, 0.68f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Matrix Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            4, 0.4f, 0.0f, 0, 0, 0.0f, 1.6f, 68.0f, 0.0f, 136.0f,
            3.2f, 98.0f, 0.0f, 196.0f, 0.94f, 0.64f, 16.0f, 1, 32.0f, 1,
            4800.0f, 0.92f, 0.72f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Neural Pluck", "Cybernetic Pluck", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 2.5f, 80.0f, 0.0f, 160.0f,
            5.0f, 120.0f, 0.0f, 240.0f, 1.0f, 0.8f, 18.0f, 2, 36.0f, 2,
            5200.0f, 0.95f, 0.75f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        // Deep Sub (10 пресетов)
        presets.push_back({"Sub Bass", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -2, 1.0f, 10.0f, 200.0f, 1.0f, 400.0f,
            20.0f, 300.0f, 0.8f, 500.0f, 0.5f, 0.1f, 1.0f, 0, 2.0f, 0,
            120.0f, 0.3f, 0.1f, 0, 1, 0.0f, false, 50.0f, -12.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Deep Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -2, 0.95f, 15.0f, 250.0f, 1.0f, 500.0f,
            25.0f, 350.0f, 0.85f, 600.0f, 0.55f, 0.15f, 1.5f, 0, 3.0f, 0,
            100.0f, 0.25f, 0.08f, 0, 1, 0.0f, false, 50.0f, -15.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Ultra Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -3, 1.0f, 20.0f, 300.0f, 1.0f, 600.0f,
            30.0f, 400.0f, 0.9f, 700.0f, 0.6f, 0.2f, 2.0f, 0, 4.0f, 0,
            80.0f, 0.2f, 0.05f, 0, 1, 0.0f, false, 50.0f, -18.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Mega Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -3, 0.98f, 25.0f, 350.0f, 1.0f, 700.0f,
            35.0f, 450.0f, 0.92f, 800.0f, 0.65f, 0.25f, 2.5f, 0, 5.0f, 0,
            70.0f, 0.18f, 0.04f, 0, 1, 0.0f, false, 50.0f, -20.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Terra Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -2, 0.92f, 12.0f, 220.0f, 1.0f, 450.0f,
            22.0f, 320.0f, 0.82f, 550.0f, 0.52f, 0.12f, 1.2f, 0, 2.4f, 0,
            110.0f, 0.28f, 0.09f, 0, 1, 0.0f, false, 50.0f, -14.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Abyss Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -3, 0.96f, 18.0f, 280.0f, 1.0f, 560.0f,
            28.0f, 380.0f, 0.88f, 660.0f, 0.58f, 0.18f, 1.8f, 0, 3.6f, 0,
            90.0f, 0.22f, 0.06f, 0, 1, 0.0f, false, 50.0f, -17.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Void Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -3, 0.99f, 22.0f, 320.0f, 1.0f, 640.0f,
            32.0f, 420.0f, 0.91f, 740.0f, 0.62f, 0.22f, 2.2f, 0, 4.4f, 0,
            75.0f, 0.19f, 0.05f, 0, 1, 0.0f, false, 50.0f, -19.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Dark Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -2, 0.94f, 14.0f, 240.0f, 1.0f, 480.0f,
            24.0f, 340.0f, 0.84f, 580.0f, 0.54f, 0.14f, 1.4f, 0, 2.8f, 0,
            105.0f, 0.26f, 0.08f, 0, 1, 0.0f, false, 50.0f, -16.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Shadow Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -2, 0.93f, 16.0f, 260.0f, 1.0f, 520.0f,
            26.0f, 360.0f, 0.86f, 620.0f, 0.56f, 0.16f, 1.6f, 0, 3.2f, 0,
            95.0f, 0.24f, 0.07f, 0, 1, 0.0f, false, 50.0f, -17.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Phantom Sub", "Deep Sub", makeSynthPresetXml(
            0, 0.5f, 0.0f, 0, -3, 0.97f, 20.0f, 300.0f, 1.0f, 600.0f,
            30.0f, 400.0f, 0.9f, 700.0f, 0.6f, 0.2f, 2.0f, 0, 4.0f, 0,
            85.0f, 0.21f, 0.06f, 0, 1, 0.0f, false, 50.0f, -18.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        // Broken Digital (10 пресетов)
        presets.push_back({"Glitch Lead", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 2.0f, 60.0f, 0.7f, 120.0f,
            4.0f, 100.0f, 0.5f, 200.0f, 0.9f, 0.6f, 12.0f, 4, 24.0f, 4,
            4000.0f, 0.85f, 0.65f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.8f, 0.9f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Bit Crusher", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 1.5f, 50.0f, 0.65f, 100.0f,
            3.0f, 80.0f, 0.45f, 160.0f, 0.85f, 0.55f, 15.0f, 5, 30.0f, 5,
            5000.0f, 0.9f, 0.7f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.9f, 1.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Data Corrupt", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 1.0f, 40.0f, 0.6f, 80.0f,
            2.0f, 60.0f, 0.4f, 120.0f, 0.8f, 0.5f, 18.0f, 6, 36.0f, 6,
            6000.0f, 0.95f, 0.75f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.85f, 0.95f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Pixel Storm", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 2.5f, 70.0f, 0.75f, 140.0f,
            5.0f, 110.0f, 0.55f, 220.0f, 0.95f, 0.65f, 10.0f, 0, 20.0f, 0,
            3500.0f, 0.82f, 0.62f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Buffer Overflow", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 1.8f, 55.0f, 0.68f, 110.0f,
            3.5f, 90.0f, 0.48f, 180.0f, 0.88f, 0.58f, 14.0f, 1, 28.0f, 1,
            4500.0f, 0.88f, 0.68f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.75f, 0.85f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Memory Leak", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 2.2f, 65.0f, 0.72f, 130.0f,
            4.5f, 105.0f, 0.52f, 210.0f, 0.92f, 0.62f, 16.0f, 2, 32.0f, 2,
            5500.0f, 0.92f, 0.72f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Stack Overflow", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 1.2f, 45.0f, 0.62f, 90.0f,
            2.5f, 70.0f, 0.42f, 140.0f, 0.82f, 0.52f, 20.0f, 3, 40.0f, 3,
            7000.0f, 0.98f, 0.78f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.95f, 1.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Kernel Panic", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 0.8f, 35.0f, 0.58f, 70.0f,
            1.5f, 50.0f, 0.38f, 100.0f, 0.78f, 0.48f, 25.0f, 4, 50.0f, 4,
            8000.0f, 1.0f, 0.8f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 1.0f, 1.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Blue Screen", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 3.0f, 80.0f, 0.8f, 160.0f,
            6.0f, 130.0f, 0.6f, 260.0f, 1.0f, 0.7f, 8.0f, 5, 16.0f, 5,
            3000.0f, 0.8f, 0.6f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Fatal Error", "Broken Digital", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 1.0f, 40.0f, 0.6f, 80.0f,
            2.0f, 60.0f, 0.4f, 120.0f, 0.8f, 0.5f, 22.0f, 6, 44.0f, 6,
            9000.0f, 1.0f, 0.85f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            true, 0.88f, 0.98f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        // Gothic Pad (10 пресетов)
        presets.push_back({"Cathedral Pad", "Gothic Pad", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 800.0f, 1500.0f, 0.9f, 3000.0f,
            800.0f, 1500.0f, 0.85f, 3000.0f, 0.4f, 0.3f, 0.8f, 0, 1.6f, 0,
            1200.0f, 0.5f, 0.3f, 0, 1, 0.0f, false, 50.0f, -3.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.7f, 0.6f, 0.5f, false, 0.0f, 0.0f, 0.0f, 1.5f)});
        
        presets.push_back({"Crypt Pad", "Gothic Pad", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 900.0f, 1700.0f, 0.92f, 3200.0f,
            900.0f, 1700.0f, 0.87f, 3200.0f, 0.42f, 0.32f, 0.9f, 1, 1.8f, 1,
            1300.0f, 0.52f, 0.32f, 0, 1, 0.0f, false, 50.0f, -4.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.75f, 0.65f, 0.55f, false, 0.0f, 0.0f, 0.0f, 1.55f)});
        
        presets.push_back({"Dungeon Pad", "Gothic Pad", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 1000.0f, 1900.0f, 0.94f, 3400.0f,
            1000.0f, 1900.0f, 0.89f, 3400.0f, 0.44f, 0.34f, 1.0f, 2, 2.0f, 2,
            1400.0f, 0.54f, 0.34f, 0, 1, 0.0f, false, 50.0f, -5.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.8f, 0.7f, 0.6f, false, 0.0f, 0.0f, 0.0f, 1.6f)});
        
        presets.push_back({"Castle Pad", "Gothic Pad", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1100.0f, 2100.0f, 0.96f, 3600.0f,
            1100.0f, 2100.0f, 0.91f, 3600.0f, 0.46f, 0.36f, 1.1f, 3, 2.2f, 3,
            1500.0f, 0.56f, 0.36f, 0, 1, 0.0f, false, 50.0f, -6.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.85f, 0.75f, 0.65f, false, 0.0f, 0.0f, 0.0f, 1.65f)});
        
        presets.push_back({"Abbey Pad", "Gothic Pad", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 1200.0f, 2300.0f, 0.98f, 3800.0f,
            1200.0f, 2300.0f, 0.93f, 3800.0f, 0.48f, 0.38f, 1.2f, 4, 2.4f, 4,
            1600.0f, 0.58f, 0.38f, 0, 1, 0.0f, false, 50.0f, -7.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.9f, 0.8f, 0.7f, false, 0.0f, 0.0f, 0.0f, 1.7f)});
        
        presets.push_back({"Chapel Pad", "Gothic Pad", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 850.0f, 1600.0f, 0.91f, 3100.0f,
            850.0f, 1600.0f, 0.86f, 3100.0f, 0.41f, 0.31f, 0.85f, 5, 1.7f, 5,
            1250.0f, 0.51f, 0.31f, 0, 1, 0.0f, false, 50.0f, -3.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.72f, 0.62f, 0.52f, false, 0.0f, 0.0f, 0.0f, 1.52f)});
        
        presets.push_back({"Monastery Pad", "Gothic Pad", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 950.0f, 1800.0f, 0.93f, 3300.0f,
            950.0f, 1800.0f, 0.88f, 3300.0f, 0.43f, 0.33f, 0.95f, 6, 1.9f, 6,
            1350.0f, 0.53f, 0.33f, 0, 1, 0.0f, false, 50.0f, -4.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.78f, 0.68f, 0.58f, false, 0.0f, 0.0f, 0.0f, 1.58f)});
        
        presets.push_back({"Sanctuary Pad", "Gothic Pad", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1050.0f, 2000.0f, 0.95f, 3500.0f,
            1050.0f, 2000.0f, 0.9f, 3500.0f, 0.45f, 0.35f, 1.05f, 0, 2.1f, 0,
            1450.0f, 0.55f, 0.35f, 0, 1, 0.0f, false, 50.0f, -5.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.82f, 0.72f, 0.62f, false, 0.0f, 0.0f, 0.0f, 1.62f)});
        
        presets.push_back({"Temple Pad", "Gothic Pad", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 1150.0f, 2200.0f, 0.97f, 3700.0f,
            1150.0f, 2200.0f, 0.92f, 3700.0f, 0.47f, 0.37f, 1.15f, 1, 2.3f, 1,
            1550.0f, 0.57f, 0.37f, 0, 1, 0.0f, false, 50.0f, -6.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.88f, 0.78f, 0.68f, false, 0.0f, 0.0f, 0.0f, 1.68f)});
        
        presets.push_back({"Ruins Pad", "Gothic Pad", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1250.0f, 2400.0f, 0.99f, 3900.0f,
            1250.0f, 2400.0f, 0.94f, 3900.0f, 0.49f, 0.39f, 1.25f, 2, 2.5f, 2,
            1650.0f, 0.59f, 0.39f, 0, 1, 0.0f, false, 50.0f, -7.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.92f, 0.82f, 0.72f, false, 0.0f, 0.0f, 0.0f, 1.72f)});
        
        // Noise Texture (10 пресетов)
        presets.push_back({"White Noise", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 5.0f, 100.0f, 0.8f, 200.0f,
            10.0f, 200.0f, 0.7f, 300.0f, 0.6f, 0.4f, 3.0f, 0, 6.0f, 0,
            5000.0f, 0.5f, 0.3f, 0, 1, 0.0f, false, 50.0f, -6.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Pink Noise", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 8.0f, 150.0f, 0.85f, 300.0f,
            15.0f, 300.0f, 0.75f, 450.0f, 0.65f, 0.45f, 4.0f, 1, 8.0f, 1,
            3000.0f, 0.4f, 0.25f, 0, 1, 0.0f, false, 50.0f, -7.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Brown Noise", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 12.0f, 200.0f, 0.9f, 400.0f,
            20.0f, 400.0f, 0.8f, 600.0f, 0.7f, 0.5f, 5.0f, 2, 10.0f, 2,
            1500.0f, 0.3f, 0.2f, 0, 1, 0.0f, false, 50.0f, -8.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Wind Texture", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 500.0f, 1000.0f, 0.95f, 2000.0f,
            500.0f, 1000.0f, 0.9f, 2000.0f, 0.5f, 0.3f, 0.5f, 0, 1.0f, 0,
            2000.0f, 0.6f, 0.4f, 0, 1, 0.0f, false, 50.0f, -5.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.6f, 0.5f, 0.4f, false, 0.0f, 0.0f, 0.0f, 1.8f)});
        
        presets.push_back({"Rain Texture", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 300.0f, 600.0f, 0.88f, 1200.0f,
            300.0f, 600.0f, 0.82f, 1200.0f, 0.55f, 0.35f, 2.0f, 3, 4.0f, 3,
            4000.0f, 0.7f, 0.5f, 0, 1, 0.0f, false, 50.0f, -6.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.5f, 0.4f, 0.3f, false, 0.0f, 0.0f, 0.0f, 1.6f)});
        
        presets.push_back({"Fire Texture", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 200.0f, 400.0f, 0.86f, 800.0f,
            200.0f, 400.0f, 0.8f, 800.0f, 0.6f, 0.4f, 3.0f, 4, 6.0f, 4,
            2500.0f, 0.8f, 0.6f, 0, 1, 0.0f, false, 50.0f, -7.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Ocean Texture", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 600.0f, 1200.0f, 0.96f, 2400.0f,
            600.0f, 1200.0f, 0.92f, 2400.0f, 0.52f, 0.32f, 0.6f, 5, 1.2f, 5,
            1800.0f, 0.55f, 0.35f, 0, 1, 0.0f, false, 50.0f, -5.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.65f, 0.55f, 0.45f, false, 0.0f, 0.0f, 0.0f, 1.9f)});
        
        presets.push_back({"Thunder Texture", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 100.0f, 200.0f, 0.82f, 400.0f,
            100.0f, 200.0f, 0.76f, 400.0f, 0.65f, 0.45f, 4.0f, 6, 8.0f, 6,
            1000.0f, 0.9f, 0.7f, 0, 1, 0.0f, false, 50.0f, -9.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Static Texture", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 15.0f, 250.0f, 0.84f, 500.0f,
            25.0f, 350.0f, 0.78f, 500.0f, 0.62f, 0.42f, 6.0f, 0, 12.0f, 0,
            6000.0f, 0.6f, 0.4f, 0, 1, 0.0f, false, 50.0f, -6.8f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"Hiss Texture", "Noise Texture", makeSynthPresetXml(
            5, 0.5f, 0.0f, 0, 0, 0.0f, 25.0f, 400.0f, 0.87f, 700.0f,
            35.0f, 500.0f, 0.81f, 700.0f, 0.58f, 0.38f, 5.0f, 1, 10.0f, 1,
            7000.0f, 0.65f, 0.45f, 0, 1, 0.0f, false, 50.0f, -7.2f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        // Experimental FM (10 пресетов)
        presets.push_back({"FM Bell", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.5f, 0, 0, 0.0f, 2.0f, 500.0f, 0.0f, 1000.0f,
            5.0f, 800.0f, 0.0f, 1200.0f, 1.0f, 0.7f, 8.0f, 0, 16.0f, 0,
            4000.0f, 0.6f, 0.4f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"FM Pluck", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.6f, 0, 0, 0.0f, 1.0f, 300.0f, 0.0f, 600.0f,
            3.0f, 500.0f, 0.0f, 800.0f, 0.95f, 0.65f, 12.0f, 1, 24.0f, 1,
            5000.0f, 0.7f, 0.5f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"FM Lead", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.7f, 0, 0, 0.0f, 5.0f, 400.0f, 0.8f, 800.0f,
            10.0f, 600.0f, 0.6f, 1000.0f, 0.8f, 0.5f, 6.0f, 2, 12.0f, 2,
            3000.0f, 0.5f, 0.3f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"FM Bass", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.4f, 0, 0, 0.0f, 3.0f, 350.0f, 0.85f, 700.0f,
            8.0f, 550.0f, 0.7f, 900.0f, 0.9f, 0.6f, 4.0f, 3, 8.0f, 3,
            2000.0f, 0.4f, 0.25f, 0, 1, 0.0f, false, 50.0f, -3.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"FM Pad", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.8f, 0, 0, 0.0f, 600.0f, 1200.0f, 0.9f, 2400.0f,
            600.0f, 1200.0f, 0.85f, 2400.0f, 0.5f, 0.3f, 2.0f, 4, 4.0f, 4,
            2500.0f, 0.45f, 0.28f, 0, 1, 0.0f, false, 50.0f, -2.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.6f, 0.5f, 0.4f, false, 0.0f, 0.0f, 0.0f, 1.4f)});
        
        presets.push_back({"FM Brass", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.55f, 0, 0, 0.0f, 15.0f, 300.0f, 0.8f, 600.0f,
            20.0f, 400.0f, 0.75f, 700.0f, 0.85f, 0.55f, 5.0f, 5, 10.0f, 5,
            3500.0f, 0.55f, 0.35f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"FM Strings", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.65f, 0, 0, 0.0f, 400.0f, 800.0f, 0.88f, 1600.0f,
            400.0f, 800.0f, 0.82f, 1600.0f, 0.6f, 0.4f, 3.0f, 6, 6.0f, 6,
            2800.0f, 0.5f, 0.32f, 0, 1, 0.0f, false, 50.0f, -1.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.7f, 0.6f, 0.5f, false, 0.0f, 0.0f, 0.0f, 1.5f)});
        
        presets.push_back({"FM Organ", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.5f, 0, 0, 0.0f, 20.0f, 100.0f, 1.0f, 200.0f,
            30.0f, 150.0f, 0.95f, 300.0f, 0.7f, 0.5f, 4.0f, 0, 8.0f, 0,
            4500.0f, 0.65f, 0.45f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"FM Clav", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.45f, 0, 0, 0.0f, 1.5f, 200.0f, 0.0f, 400.0f,
            4.0f, 300.0f, 0.0f, 500.0f, 0.92f, 0.62f, 10.0f, 1, 20.0f, 1,
            5500.0f, 0.75f, 0.55f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        presets.push_back({"FM Harp", "Experimental FM", makeSynthPresetXml(
            7, 0.5f, 0.75f, 0, 0, 0.0f, 3.0f, 450.0f, 0.0f, 900.0f,
            6.0f, 700.0f, 0.0f, 1100.0f, 0.98f, 0.68f, 14.0f, 2, 28.0f, 2,
            4800.0f, 0.68f, 0.48f, 0, 1, 0.0f, false, 50.0f, 0.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            false, 0.0f, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f, 1.0f)});
        
        // Dark Atmosphere (10 пресетов)
        presets.push_back({"Void Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 1500.0f, 3000.0f, 0.95f, 6000.0f,
            1500.0f, 3000.0f, 0.9f, 6000.0f, 0.3f, 0.2f, 0.3f, 0, 0.6f, 0,
            800.0f, 0.4f, 0.2f, 0, 1, 0.0f, false, 50.0f, -6.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.9f, 0.8f, 0.7f, false, 0.0f, 0.0f, 0.0f, 2.0f)});
        
        presets.push_back({"Abyss Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1800.0f, 3500.0f, 0.96f, 7000.0f,
            1800.0f, 3500.0f, 0.92f, 7000.0f, 0.32f, 0.22f, 0.35f, 1, 0.7f, 1,
            900.0f, 0.42f, 0.22f, 0, 1, 0.0f, false, 50.0f, -7.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.92f, 0.82f, 0.72f, false, 0.0f, 0.0f, 0.0f, 2.1f)});
        
        presets.push_back({"Shadow Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 2000.0f, 4000.0f, 0.97f, 8000.0f,
            2000.0f, 4000.0f, 0.94f, 8000.0f, 0.34f, 0.24f, 0.4f, 2, 0.8f, 2,
            1000.0f, 0.44f, 0.24f, 0, 1, 0.0f, false, 50.0f, -8.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.94f, 0.84f, 0.74f, false, 0.0f, 0.0f, 0.0f, 2.2f)});
        
        presets.push_back({"Phantom Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 2200.0f, 4500.0f, 0.98f, 9000.0f,
            2200.0f, 4500.0f, 0.96f, 9000.0f, 0.36f, 0.26f, 0.45f, 3, 0.9f, 3,
            1100.0f, 0.46f, 0.26f, 0, 1, 0.0f, false, 50.0f, -9.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.96f, 0.86f, 0.76f, false, 0.0f, 0.0f, 0.0f, 2.3f)});
        
        presets.push_back({"Wraith Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 2500.0f, 5000.0f, 0.99f, 10000.0f,
            2500.0f, 5000.0f, 0.98f, 10000.0f, 0.38f, 0.28f, 0.5f, 4, 1.0f, 4,
            1200.0f, 0.48f, 0.28f, 0, 1, 0.0f, false, 50.0f, -10.0f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.98f, 0.88f, 0.78f, false, 0.0f, 0.0f, 0.0f, 2.4f)});
        
        presets.push_back({"Spectral Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1600.0f, 3200.0f, 0.955f, 6400.0f,
            1600.0f, 3200.0f, 0.91f, 6400.0f, 0.31f, 0.21f, 0.32f, 5, 0.64f, 5,
            850.0f, 0.41f, 0.21f, 0, 1, 0.0f, false, 50.0f, -6.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.91f, 0.81f, 0.71f, false, 0.0f, 0.0f, 0.0f, 2.05f)});
        
        presets.push_back({"Ethereal Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 1700.0f, 3400.0f, 0.965f, 6800.0f,
            1700.0f, 3400.0f, 0.93f, 6800.0f, 0.33f, 0.23f, 0.38f, 6, 0.76f, 6,
            950.0f, 0.43f, 0.23f, 0, 1, 0.0f, false, 50.0f, -7.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.93f, 0.83f, 0.73f, false, 0.0f, 0.0f, 0.0f, 2.15f)});
        
        presets.push_back({"Haunting Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 1900.0f, 3800.0f, 0.975f, 7600.0f,
            1900.0f, 3800.0f, 0.95f, 7600.0f, 0.35f, 0.25f, 0.42f, 0, 0.84f, 0,
            1050.0f, 0.45f, 0.25f, 0, 1, 0.0f, false, 50.0f, -8.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.95f, 0.85f, 0.75f, false, 0.0f, 0.0f, 0.0f, 2.25f)});
        
        presets.push_back({"Nightmare Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            2, 0.5f, 0.0f, 0, 0, 0.0f, 2100.0f, 4200.0f, 0.985f, 8400.0f,
            2100.0f, 4200.0f, 0.97f, 8400.0f, 0.37f, 0.27f, 0.48f, 1, 0.96f, 1,
            1150.0f, 0.47f, 0.27f, 0, 1, 0.0f, false, 50.0f, -9.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.97f, 0.87f, 0.77f, false, 0.0f, 0.0f, 0.0f, 2.35f)});
        
        presets.push_back({"Demon Atmosphere", "Dark Atmosphere", makeSynthPresetXml(
            3, 0.5f, 0.0f, 0, 0, 0.0f, 2400.0f, 4800.0f, 0.995f, 9600.0f,
            2400.0f, 4800.0f, 0.99f, 9600.0f, 0.39f, 0.29f, 0.52f, 2, 1.04f, 2,
            1250.0f, 0.49f, 0.29f, 0, 1, 0.0f, false, 50.0f, -10.5f,
            false, 0.0f, 0.0f, false, 0.0f, 0.0f, false, 0.0f, 0.0f, 0.0f,
            true, 0.99f, 0.89f, 0.79f, false, 0.0f, 0.0f, 0.0f, 2.45f)});
        
        return presets;
    }
}
