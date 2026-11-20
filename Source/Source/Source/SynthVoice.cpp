// ... [весь предыдущий код SynthVoice.cpp остаётся без изменений] ...

// ЗАМЕНИТЬ метод setFilterParameters на новый:
void SynthVoice::setFilterParameters(float cutoff, float resonance, float drive, int type)
{
    filterCutoff = cutoff;
    filterResonance = resonance;
    filterDrive = drive;
    filterType = type;
}

// ... [остальные методы без изменений] ...
