// Preset Browser
juce::ComboBox presetBox;
juce::TextButton prevPresetBtn, nextPresetBtn;
juce::TextButton savePresetBtn, loadPresetBtn;
juce::Label presetLabel;

std::unique_ptr<juce::Timer> presetUpdateTimer;
