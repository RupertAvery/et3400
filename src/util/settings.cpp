#include "settings.h"
#include "log.h"
#include "../emu/et3400.h"
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>
#include "../common/newline.h"

QString getSettingsPath(bool &success)
{
    success = true;
    return QCoreApplication::applicationDirPath() + "/settings.ini";
}

Settings load_settings()
{
    Settings settings{false, false, true, true, false, true, 471000, -1, -1, false, -1, -1, -1, -1};

    settings.ioLEDAddress = 0x4000;
    settings.ioDIPAddress = 0x4001;

    bool success;
    QString settingsFile = getSettingsPath(success);

    if (success)
    {
        LOG_DEBUG << "Settings file:" << settingsFile;

        QFile file(settingsFile);
        if (!file.open(QIODevice::ReadOnly))
            return settings;

        QTextStream in(&file);

        int device_state = 0;
        int deviceIndex = -1;
        int lastDeviceNo = -1;

        while (!in.atEnd())
        {
            QString line = in.readLine();
            int sep = line.indexOf(QLatin1Char('='));
            if (sep < 0)
                continue;

            QString key = line.left(sep);
            QString value = line.mid(sep + 1);

            if (key == "ShowTips")
                settings.showTips = value == "true";
            else if (key == "ShowDisassemblerView")
                settings.showDasmView = value == "true";
            else if (key == "ShowMemoryView")
                settings.showMemoryView = value == "true";
            else if (key == "AutoRefreshDasm")
                settings.autoRefreshDasm = value == "true";
            else if (key == "ShowHeatMap")
                settings.showHeatMap = value == "true";
            else if (key == "ClockRate")
                settings.clockRate = value.toInt();
            else if (key == "MainWindowX")
                settings.mainWindowX = value.toInt();
            else if (key == "MainWindowY")
                settings.mainWindowY = value.toInt();
            else if (key == "DebuggerVisible")
                settings.debuggerVisible = value == "true";
            else if (key == "DebuggerX")
                settings.debuggerX = value.toInt();
            else if (key == "DebuggerY")
                settings.debuggerY = value.toInt();
            else if (key == "DebuggerWidth")
                settings.debuggerWidth = value.toInt();
            else if (key == "DebuggerHeight")
                settings.debuggerHeight = value.toInt();
            else if (key == "RomDir")
                settings.romDir = value;
            else if (key == "RamDir")
                settings.ramDir = value;
            else if (key == "LabelsDir")
                settings.labelsDir = value;
            else if (key == "BreakpointsDir")
                settings.breakpointsDir = value;
            else if (key == "ClearRamOnLoad")
                settings.clearRamOnLoad = value == "true";
            else if (key == "ShowBit0DisplayWrites")
                settings.showBit0DisplayWrites = value == "true";
            else if (key == "HeatMapDecay")
                settings.heatMapDecay = value.toInt();
            else if (key == "FirstDebuggerOpen")
                settings.firstDebuggerOpen = value == "true";
            else if (key == "IOVisible")
                settings.ioVisible = value == "true";
            else if (key == "IOLEDAddress")
                settings.ioLEDAddress = value.toInt();
            else if (key == "IODIPAddress")
                settings.ioDIPAddress = value.toInt();
            else if (key == "DeviceCount")
            {
                settings.devices.resize(value.toInt());
            }
            else if (key.startsWith("Device["))
            {
                int indexer = key.indexOf(QLatin1Char(']'));
                int deviceNo = key.mid(7, indexer - 7).toInt();
                if (deviceNo != lastDeviceNo)
                {
                    deviceIndex++;
                }
                QString subKey = key.mid(indexer + 2);

                if (subKey == "Name")
                    settings.devices[deviceIndex].name = value;
                else if (subKey == "BitPattern")
                    settings.devices[deviceIndex].bit_pattern = value;

                lastDeviceNo = deviceNo;
            }
        }

        file.close();
    }

    return settings;
};

void save_settings(Settings *settings)
{
    bool success;
    QString settingsFile = getSettingsPath(success);

    if (!success)
        return;

    LOG_DEBUG << "Saving settings to" << settingsFile;

    QFileInfo fi(settingsFile);
    QString settingsPath = fi.absolutePath();

    QDir dir;
    if (!dir.exists(settingsPath))
        dir.mkpath(settingsPath);

    QFile file(settingsFile);
    if (!file.open(QIODevice::WriteOnly | QFile::Truncate))
        return;

    QTextStream out(&file);

    out << "ShowTips=" << (settings->showTips ? "true" : "false") << NEWLINE;
    out << "ShowDisassemblerView=" << (settings->showDasmView ? "true" : "false") << NEWLINE;
    out << "ShowMemoryView=" << (settings->showMemoryView ? "true" : "false") << NEWLINE;
    out << "AutoRefreshDasm=" << (settings->autoRefreshDasm ? "true" : "false") << NEWLINE;
    out << "ShowHeatMap=" << (settings->showHeatMap ? "true" : "false") << NEWLINE;
    out << "ClockRate=" << settings->clockRate << NEWLINE;
    out << "MainWindowX=" << settings->mainWindowX << NEWLINE;
    out << "MainWindowY=" << settings->mainWindowY << NEWLINE;
    out << "DebuggerVisible=" << (settings->debuggerVisible ? "true" : "false") << NEWLINE;
    out << "DebuggerX=" << settings->debuggerX << NEWLINE;
    out << "DebuggerY=" << settings->debuggerY << NEWLINE;
    out << "DebuggerWidth=" << settings->debuggerWidth << NEWLINE;
    out << "DebuggerHeight=" << settings->debuggerHeight << NEWLINE;
    out << "RomDir=" << settings->romDir << NEWLINE;
    out << "RamDir=" << settings->ramDir << NEWLINE;
    out << "LabelsDir=" << settings->labelsDir << NEWLINE;
    out << "BreakpointsDir=" << settings->breakpointsDir << NEWLINE;
    out << "ClearRamOnLoad=" << (settings->clearRamOnLoad ? "true" : "false") << NEWLINE;
    out << "ShowBit0DisplayWrites=" << (settings->showBit0DisplayWrites ? "true" : "false") << NEWLINE;
    out << "HeatMapDecay=" << settings->heatMapDecay << NEWLINE;
    out << "FirstDebuggerOpen=" << (settings->firstDebuggerOpen ? "true" : "false") << NEWLINE;
    out << "IOVisible=" << (settings->ioVisible ? "true" : "false") << NEWLINE;
    out << "IOLEDAddress=" << settings->ioLEDAddress << NEWLINE;
    out << "IODIPAddress=" << settings->ioDIPAddress << NEWLINE;
    out << "DeviceCount=" << settings->devices.size() << NEWLINE;

    for (size_t i = 0; i < settings->devices.size(); i++)
    {
        out << "Device[" << i << "].Name=" << settings->devices[i].name << NEWLINE;
        out << "Device[" << i << "].BitPattern=" << settings->devices[i].bit_pattern << NEWLINE;
    }

    out.flush();
    file.close();

    LOG_DEBUG << "Saved settings";
};

void build_and_save_settings(Settings *settings, et3400emu *emu_ptr)
{
    auto devices = emu_ptr->memory_map->get_custom_devices();

    settings->devices.clear();
    settings->devices.reserve(devices.size());

    for (const auto &device : devices)
    {
        DeviceSetting setting;
        setting.name = QString::fromStdString(device->name);
        setting.bit_pattern = device->get_pattern();
        settings->devices.push_back(setting);
    }

    save_settings(settings);
}