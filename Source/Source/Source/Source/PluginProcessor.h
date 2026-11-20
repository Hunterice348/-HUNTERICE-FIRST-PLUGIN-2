#include "PresetManager.h"

// В private секции:
std::unique_ptr<PresetManager> presetManager;

// Публичный метод:
PresetManager* getPresetManager() { return presetManager.get(); }
