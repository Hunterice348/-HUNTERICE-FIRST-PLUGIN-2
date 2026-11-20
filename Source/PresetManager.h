void PresetManager::loadStateFromXml(const juce::String& xmlString)
{
    if (auto xml = juce::XmlDocument::parse(xmlString))
    {
        // ИСПРАВЛЕНИЕ: Правильная загрузка параметров
        for (auto* paramXml : xml->getChildIterator())
        {
            auto paramId = paramXml->getStringAttribute("id");
            auto value = paramXml->getDoubleAttribute("value");
            
            if (auto* param = apvts.getParameter(paramId))
            {
                param->setValueNotifyingHost(
                    param->convertTo0to1(static_cast<float>(value))
                );
            }
        }
    }
}
