#include "settings.h"

#include <QDebug>
#include <QString>
#include <QDir>
#include <QStringList>

#ifdef _WIN32
#include <Windows.h>
#endif //_WIN32

#ifdef _WINDOWS_
#include <windows.h>
#include <wchar.h>
#include <KnownFolders.h>
#include <shlobj.h>

QString getSettingsPath(bool &success)
{
    QString spath;
    PWSTR path = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_RoamingAppData, 0, NULL, &path);
    if (SUCCEEDED(hr))
    {
        spath = QString("%1").arg(path) + QDir::separator() + QString("ET3400") + QDir::separator() + QString("settings.ini");
    }
    CoTaskMemFree(path);
    return spath;
}

#endif

Settings load_settings()
{

    bool showTips = false;

    bool success;
    QString settingsFile = getSettingsPath(success);

    if (success)
    {
        qDebug() << settingsFile;

        QFile file(settingsFile);
        if (!file.open(QIODevice::ReadOnly))
        {
            //QMessageBox::information(0, "error", file.errorString());
            return Settings{true};
        }

        QTextStream in(&file);

        while (!in.atEnd())
        {
            QString line = in.readLine();

            if (line.length() > 0)
            {
                QStringList list1 = line.split(QLatin1Char('='));
                if (list1.at(0) == "ShowTips" && list1.at(1) == "true")
                {
                    showTips = true;
                }
                qDebug() << list1;
            }
        }

        file.close();
    }

    return Settings{showTips};
};

void save_settings(Settings *settings)
{
    bool success;
    QString settingsFile = getSettingsPath(success);
    qDebug() << "Get path: " << success;

    if (success)
    {
        qDebug() << "Saving settings to " << settingsFile;

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
            //QMessageBox::information(0, "error", file.errorString());
            return;
        }

        QTextStream out(&file);

        out << "ShowTips=" << (settings->showTips ? "true" : "false");

        out.flush();

        file.close();

        qDebug() << "Saved settings";
    }
};
