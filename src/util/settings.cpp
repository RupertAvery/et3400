#include "settings.h"
#include "log.h"
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

    out << "ShowTips=" << (settings->showTips ? "true" : "false") << NEWLINE;
    out << "ShowDisassemblerView=" << (settings->showDasmView ? "true" : "false") << NEWLINE;
    out << "ShowMemoryView=" << (settings->showMemoryView ? "true" : "false") << NEWLINE;
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
    out << "BreakpointsDir=" << settings->breakpointsDir;

    out.flush();
    file.close();

    LOG_DEBUG << "Saved settings";
};
