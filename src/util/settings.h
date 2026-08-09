#ifndef SETTINGS_INI_H
#define SETTINGS_INI_H

#include <QString>
#include <vector>

class et3400emu;

struct DeviceSetting
{
    QString name;
    QString bit_pattern;
};

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
    bool showBit0DisplayWrites;
    int heatMapDecay;
    bool firstDebuggerOpen;
    std::vector<DeviceSetting> devices;
};

Settings load_settings();
void save_settings(Settings *settings);
void build_and_save_settings(Settings *settings, et3400emu *emu_ptr);

#endif // SETTINGS_INI_H
