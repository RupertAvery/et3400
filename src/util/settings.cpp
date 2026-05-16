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
    Settings settings{false, false, true, true, false, true, 471000};

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
        out << "ClockRate=" << settings->clockRate;

        out.flush();

        file.close();

        LOG_DEBUG << "Saved settings";
    }
};
