#ifndef SETTINGS_INI_H
#define SETTINGS_INI_H

struct Settings
{
    bool showTips;
    bool showHeatMap;
    bool showDasmView;
    bool showMemoryView;
    bool autoRefreshDasm;
    bool autoRefreshMemory;
    bool loadDefaultLabels;
    int clockRate;
};

Settings load_settings();
void save_settings(Settings *settings);


#endif // SETTINGS_INI_H
