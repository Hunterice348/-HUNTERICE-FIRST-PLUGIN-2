// Preset Browser
presetLabel.setText("Preset:", juce::dontSendNotification);
presetLabel.setColour(juce::Label::textColourId, HuntericeLookAndFeel::coldWhite);
addAndMakeVisible(presetLabel);

presetBox.addItemList(processorRef.getPresetManager()->getAllPresetNames(), 1);
presetBox.setSelectedId(1);
addAndMakeVisible(presetBox);

prevPresetBtn.setButtonText("<");
nextPresetBtn.setButtonText(">");
savePresetBtn.setButtonText("Save");
loadPresetBtn.setButtonText("Load");

addAndMakeVisible(prevPresetBtn);
addAndMakeVisible(nextPresetBtn);
addAndMakeVisible(savePresetBtn);
addAndMakeVisible(loadPresetBtn);

// Callbacks
presetBox.onChange = [this]() {
    processorRef.getPresetManager()->loadPreset(presetBox.getSelectedId() - 1);
};

prevPresetBtn.onClick = [this]() {
    processorRef.getPresetManager()->previousPreset();
    presetBox.setSelectedId(processorRef.getPresetManager()->getCurrentPresetIndex() + 1);
};

nextPresetBtn.onClick = [this]() {
    processorRef.getPresetManager()->nextPreset();
    presetBox.setSelectedId(processorRef.getPresetManager()->getCurrentPresetIndex() + 1);
};

savePresetBtn.onClick = [this]() {
    auto name = juce::String("User Preset ") + juce::String(processorRef.getPresetManager()->getNumPresets());
    processorRef.getPresetManager()->saveUserPreset(name);
    presetBox.addItem(name, processorRef.getPresetManager()->getNumPresets());
};

loadPresetBtn.onClick = [this]() {
    // Показать диалог выбора файла
    auto chooser = std::make_shared<juce::FileChooser>("Load Preset", 
        juce::File::getSpecialLocation(juce::File::userApplicationDataDirectory),
        "*.preset");
    
    chooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this, chooser](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.exists()) {
                auto xml = file.loadFileAsString();
                // Загрузить пресет из XML
            }
        });
};

// Обновление presetBox при изменении пресета
processorRef.getPresetManager()->onPresetChanged = [this](const juce::String& name) {
    auto names = processorRef.getPresetManager()->getAllPresetNames();
    for (int i = 0; i < names.size(); ++i) {
        if (names[i].endsWith(name)) {
            presetBox.setSelectedId(i + 1);
            break;
        }
    }
};
