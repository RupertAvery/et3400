#ifndef SETTINGS_INI_H
#define SETTINGS_INI_H

#include <QString>

struct Settings
{
    bool showTips;
    bool showHeatMap;
    bool showDasmView;
    bool showMemoryView;
    bool autoRefreshDasm;
    bool autoRefreshMemory;
    int clockRate;
    int mainWindowX;
    int mainWindowY;
    bool debuggerVisible;
    int debuggerX;
    int debuggerY;
    int debuggerWidth;
    int debuggerHeight;
    QString romDir;
    QString ramDir;
    QString labelsDir;
    QString breakpointsDir;
    bool clearRamOnLoad;
};

Settings load_settings();
void save_settings(Settings *settings);


#endif // SETTINGS_INI_H
