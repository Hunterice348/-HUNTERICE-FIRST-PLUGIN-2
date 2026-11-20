// ... [весь предыдущий код SynthVoice.h остаётся без изменений] ...

// ЗАМЕНИТЬ метод setFilterParameters на новый:
void setFilterParameters(float cutoff, float resonance, float drive, int type);

// ... [остальной код без изменений] ...

private:
    // ... [предыдущие поля] ...
    
    // Filter (обновлённый)
    FilterEngine filter;
    float filterCutoff = 1000.0f;
    float filterResonance = 0.5f;
    float filterDrive = 0.0f;        // НОВОЕ: Drive (насыщение)
    int filterType = 0;
    
    // ... [остальные поля] ...
