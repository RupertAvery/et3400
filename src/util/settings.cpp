#include "settings.h"
#include "log.h"
#include <QString>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QTextStream>
#include <QCoreApplication>

QString getSettingsPath(bool &success)
{
    success = true;
    return QCoreApplication::applicationDirPath() + "/settings.ini";
}

Settings load_settings()
{
    Settings settings{false, false, true, true, false, true, 471000, -1, -1, false, -1, -1, -1, -1};

    bool success;
    QString settingsFile = getSettingsPath(success);

    if (success)
    {
        LOG_DEBUG << "Settings file:" << settingsFile;

        QFile file(settingsFile);
        if (!file.open(QIODevice::ReadOnly))
            return settings;

        QTextStream in(&file);

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

    out << "ShowTips=" << (settings->showTips ? "true" : "false") << "\r\n";
    out << "ShowDisassemblerView=" << (settings->showDasmView ? "true" : "false") << "\r\n";
    out << "ShowMemoryView=" << (settings->showMemoryView ? "true" : "false") << "\r\n";
    out << "ShowHeatMap=" << (settings->showHeatMap ? "true" : "false") << "\r\n";
    out << "ClockRate=" << settings->clockRate << "\r\n";
    out << "MainWindowX=" << settings->mainWindowX << "\r\n";
    out << "MainWindowY=" << settings->mainWindowY << "\r\n";
    out << "DebuggerVisible=" << (settings->debuggerVisible ? "true" : "false") << "\r\n";
    out << "DebuggerX=" << settings->debuggerX << "\r\n";
    out << "DebuggerY=" << settings->debuggerY << "\r\n";
    out << "DebuggerWidth=" << settings->debuggerWidth << "\r\n";
    out << "DebuggerHeight=" << settings->debuggerHeight << "\r\n";
    out << "RomDir=" << settings->romDir << "\r\n";
    out << "RamDir=" << settings->ramDir << "\r\n";
    out << "LabelsDir=" << settings->labelsDir << "\r\n";
    out << "BreakpointsDir=" << settings->breakpointsDir;

    out.flush();
    file.close();

    LOG_DEBUG << "Saved settings";
};
