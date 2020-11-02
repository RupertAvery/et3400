#ifndef SETTINGS_H
#define SETTINGS_H

#include "../emu/et3400.h"
#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QSlider>
#include <QString>

static QString format(int i)
{
    if (i >= 1000000)
    {
        return QString("%1 MHz").arg(i / (float)1000000);
    }
    else if (i > 1000)
    {
        return QString("%1 klHz").arg(i / (float)1000);
    }
    return QString("%1 Hz").arg(i);
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog();
    void set_emulator(et3400emu *emu);

signals:
    void valueChanged(int value);

public slots:
    void setClockRate(int value);

private:
    QSlider *slider;
    QLabel *label;
    et3400emu *emu_ptr;
};

#endif // SETTINGS_H