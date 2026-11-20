#pragma once

#include <juce_core/juce_core.h>
#include <juce_audio_processors/juce_audio_processors.h>

class PresetManager
{
public:
    PresetManager(juce::AudioProcessorValueTreeState& apvts, const juce::String& pluginName);
    ~PresetManager();

    // Загрузка заводских пресетов
    void loadFactoryPresets();
    
    // Навигация
    void nextPreset();
    void previousPreset();
    void loadPreset(int index);
    
    // Сохранение/загрузка пользовательских пресетов
    void saveUserPreset(const juce::String& name);
    void loadUserPreset(const juce::String& name);
    void deleteUserPreset(const juce::String& name);
    
    // Получение информации
    juce::StringArray getAllPresetNames() const;
    juce::String getCurrentPresetName() const;
    int getCurrentPresetIndex() const;
    int getNumPresets() const;
    
    // Callback для уведомления UI об изменении пресета
    std::function<void(const juce::String&)> onPresetChanged;

private:
    juce::AudioProcessorValueTreeState& apvts;
    juce::String pluginName;
    
    struct Preset
    {
        juce::String name;
        juce::String category;
        juce::String xml;
        bool isFactory;
    };
    
    std::vector<Preset> presets;
    int currentPresetIndex = 0;
    
    juce::File getUserPresetsDirectory() const;
    void scanUserPresets();
    juce::String getCurrentStateAsXml() const;
    void loadStateFromXml(const juce::String& xml);
    
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetManager)
};
