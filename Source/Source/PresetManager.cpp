#include "PresetManager.h"
#include "FactoryPresets.h"

PresetManager::PresetManager(juce::AudioProcessorValueTreeState& apvts, const juce::String& pluginName)
    : apvts(apvts), pluginName(pluginName)
{
    loadFactoryPresets();
    scanUserPresets();
}

PresetManager::~PresetManager() = default;

void PresetManager::loadFactoryPresets()
{
    presets.clear();
    
    if (pluginName == "HUNTERICE_FIRST_PLUGIN")
    {
        for (const auto& preset : FactoryPresets::getEffectPresets())
        {
            presets.push_back({ preset.name, preset.category, preset.xml, true });
        }
    }
    else if (pluginName == "HUNTERICE_SYNTH")
    {
        for (const auto& preset : FactoryPresets::getSynthPresets())
        {
            presets.push_back({ preset.name, preset.category, preset.xml, true });
        }
    }
    
    currentPresetIndex = 0;
}

void PresetManager::scanUserPresets()
{
    auto userDir = getUserPresetsDirectory();
    if (!userDir.exists())
        userDir.createDirectory();
    
    for (const auto& file : userDir.findChildFiles(juce::File::findFiles, false, "*.preset"))
    {
        auto xml = file.loadFileAsString();
        auto name = file.getFileNameWithoutExtension();
        presets.push_back({ name, "User", xml, false });
    }
}

void PresetManager::nextPreset()
{
    if (presets.empty()) return;
    currentPresetIndex = (currentPresetIndex + 1) % presets.size();
    loadPreset(currentPresetIndex);
}

void PresetManager::previousPreset()
{
    if (presets.empty()) return;
    currentPresetIndex = (currentPresetIndex - 1 + presets.size()) % presets.size();
    loadPreset(currentPresetIndex);
}

void PresetManager::loadPreset(int index)
{
    if (index < 0 || index >= static_cast<int>(presets.size())) return;
    
    currentPresetIndex = index;
    loadStateFromXml(presets[index].xml);
    
    if (onPresetChanged)
        onPresetChanged(presets[index].name);
}

void PresetManager::saveUserPreset(const juce::String& name)
{
    auto userDir = getUserPresetsDirectory();
    if (!userDir.exists())
        userDir.createDirectory();
    
    auto file = userDir.getChildFile(name + ".preset");
    auto xml = getCurrentStateAsXml();
    file.replaceWithText(xml);
    
    // Обновляем список пресетов
    presets.push_back({ name, "User", xml, false });
    currentPresetIndex = static_cast<int>(presets.size()) - 1;
    
    if (onPresetChanged)
        onPresetChanged(name);
}

void PresetManager::loadUserPreset(const juce::String& name)
{
    for (int i = 0; i < static_cast<int>(presets.size()); ++i)
    {
        if (presets[i].name == name && !presets[i].isFactory)
        {
            loadPreset(i);
            return;
        }
    }
}

void PresetManager::deleteUserPreset(const juce::String& name)
{
    auto userDir = getUserPresetsDirectory();
    auto file = userDir.getChildFile(name + ".preset");
    if (file.exists())
        file.deleteFile();
    
    // Удаляем из списка
    for (int i = 0; i < static_cast<int>(presets.size()); ++i)
    {
        if (presets[i].name == name && !presets[i].isFactory)
        {
            presets.erase(presets.begin() + i);
            break;
        }
    }
}

juce::StringArray PresetManager::getAllPresetNames() const
{
    juce::StringArray names;
    for (const auto& preset : presets)
    {
        names.add(preset.category + ": " + preset.name);
    }
    return names;
}

juce::String PresetManager::getCurrentPresetName() const
{
    if (currentPresetIndex >= 0 && currentPresetIndex < static_cast<int>(presets.size()))
        return presets[currentPresetIndex].name;
    return "Init";
}

int PresetManager::getCurrentPresetIndex() const
{
    return currentPresetIndex;
}

int PresetManager::getNumPresets() const
{
    return static_cast<int>(presets.size());
}

juce::File PresetManager::getUserPresetsDirectory() const
{
    auto appData = juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory);
    return appData.getChildFile("HunterIce").getChildFile(pluginName).getChildFile("Presets");
}

juce::String PresetManager::getCurrentStateAsXml() const
{
    auto state = apvts.copyState();
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    return xml->toString();
}

void PresetManager::loadStateFromXml(const juce::String& xmlString)
{
    if (auto xml = juce::XmlDocument::parse(xmlString))
    {
        if (xml->hasTagName(apvts.state.getType()))
        {
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
        }
    }
}
