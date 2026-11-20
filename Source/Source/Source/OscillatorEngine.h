// ... [предыдущий код OscillatorEngine остаётся без изменений] ...

// Добавляем метод для установки модулированной pulse width
void setModulatedPulseWidth(float modulatedWidth)
{
    // modulatedWidth: -1.0 ... 1.0 (модуляция от LFO/envelope)
    // Применяем к базовой pulse width
    float finalWidth = currentPulseWidth + modulatedWidth * 0.49f;
    finalWidth = juce::jlimit(0.01f, 0.99f, finalWidth);
    setPulseWidth(finalWidth);
}

// ... [остальной код без изменений] ...
