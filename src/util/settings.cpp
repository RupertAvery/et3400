#include "settings.h"
#include "log.h"
#include <QString>
#include <QDir>
#include <QStringList>
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
        {
            // QMessageBox::information(0, "error", file.errorString());
            return settings;
        }

        QTextStream in(&file);

        while (!in.atEnd())
        {
            QString line = in.readLine();

            if (line.length() > 0)
            {
                QStringList list1 = line.split(QLatin1Char('='));
                if (list1.at(0) == "ShowTips")
                {
                    settings.showTips = list1.at(1) == "true";
                }
                else if (list1.at(0) == "ShowDisassemblerView")
                {
                    settings.showDasmView = list1.at(1) == "true";
                }
                else if (list1.at(0) == "ShowMemoryView")
                {
                    settings.showMemoryView = list1.at(1) == "true";
                }
                else if (list1.at(0) == "ShowHeatMap")
                {
                    settings.showHeatMap = list1.at(1) == "true";
                }
                else if (list1.at(0) == "ClockRate")
                {
                    settings.clockRate = list1.at(1).toInt();
                }
                else if (list1.at(0) == "MainWindowX")
                {
                    settings.mainWindowX = list1.at(1).toInt();
                }
                else if (list1.at(0) == "MainWindowY")
                {
                    settings.mainWindowY = list1.at(1).toInt();
                }
                else if (list1.at(0) == "DebuggerVisible")
                {
                    settings.debuggerVisible = list1.at(1) == "true";
                }
                else if (list1.at(0) == "DebuggerX")
                {
                    settings.debuggerX = list1.at(1).toInt();
                }
                else if (list1.at(0) == "DebuggerY")
                {
                    settings.debuggerY = list1.at(1).toInt();
                }
                else if (list1.at(0) == "DebuggerWidth")
                {
                    settings.debuggerWidth = list1.at(1).toInt();
                }
                else if (list1.at(0) == "DebuggerHeight")
                {
                    settings.debuggerHeight = list1.at(1).toInt();
                }
                LOG_DEBUG << "Settings entry:" << list1;
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
    LOG_DEBUG << "Settings path found:" << success;

    if (success)
    {
        LOG_DEBUG << "Saving settings to" << settingsFile;

        QFileInfo fi(settingsFile);
        QString settingsPath = fi.absolutePath();

        QDir dir; // Initialize to the desired dir if 'path' is relative
                  // By default the program's working directory "." is used.

        // We create the directory if needed
        if (!dir.exists(settingsPath))
            dir.mkpath(settingsPath); // You can check the success if needed

        QFile file(settingsFile);
        if (!file.open(QIODevice::WriteOnly | QFile::Truncate))
        {
            // QMessageBox::information(0, "error", file.errorString());
            return;
        }

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
        out << "DebuggerHeight=" << settings->debuggerHeight;

        out.flush();

        file.close();

        LOG_DEBUG << "Saved settings";
    }
};
