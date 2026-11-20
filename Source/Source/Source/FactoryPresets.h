namespace FactoryPresets
{
    struct PresetData
    {
        juce::String name;
        juce::String category;
        std::map<juce::String, float> values;  // ИСПРАВЛЕНИЕ: используем map вместо XML-строки
    };

    // Helper для Effect пресетов
    inline PresetData makeEffectPreset(const juce::String& name, const juce::String& category,
        float drive, float mix, float output, float tone,
        float bias, float threshold, float symmetry, int mode)
    {
        return { name, category, {
            {"DRIVE", drive}, {"MIX", mix}, {"OUTPUT", output}, {"TONE", tone},
            {"BIAS", bias}, {"THRESHOLD", threshold}, {"SYMMETRY", symmetry},
            {"DIST_MODE", static_cast<float>(mode)}
        }};
    }

    // Конвертация в правильный XML для APVTS
    inline juce::String toApvtsXml(const PresetData& preset, const juce::String& stateType)
    {
        juce::XmlElement xml(stateType);
        for (const auto& [paramId, value] : preset.values)
        {
            auto* paramXml = xml.createNewChildElement("PARAM");
            paramXml->setAttribute("id", paramId);
            paramXml->setAttribute("value", value);
        }
        return xml.toString();
    }

    // ... остальные пресеты используют makeEffectPreset() ...
}
