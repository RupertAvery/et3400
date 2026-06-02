#ifndef SETTINGS_H
#define SETTINGS_H

#include "../emu/et3400.h"
#include <QApplication>
#include <QDialog>
#include <QWidget>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QComboBox>
#include <QLineEdit>
#include <QString>
#include <QRadioButton>
#include <QCheckBox>
#include <QDialogButtonBox>
#include "../util/settings.h"

static QString format(int i)
{
    if (i >= 1000000)
    {
        return QString("%1 MHz").arg(i / (float)1000000);
    }
    else if (i > 1000)
    {
        return QString("%1 kHz").arg(i / (float)1000);
    }
    return QString("%1 Hz").arg(i);
};

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    SettingsDialog();
    SettingsDialog(QWidget *parent);
    void set_emulator(et3400emu *emu);
    void set_settings(Settings *settings);

signals:
    // void valueChanged(int value);

public slots:
    void setClockRateValue(int value);
    void resetClockRate();

private slots:
    // void onFreqInputChanged();
    // void onUnitChanged(int index);

private:
    QComboBox *unit_combo;
    QLineEdit *freq_input;
    QLabel *clock_rate_label;
    QLabel *warning_label;
    et3400emu *emu_ptr = nullptr;

    Settings *settings = nullptr;

    QDialogButtonBox *buttonBox;

    QRadioButton *radioButton_Hz;
    QRadioButton *radioButton_kHz;
    QRadioButton *radioButton_MHz;
    QSlider *horizontalSlider;
    QPushButton *reset_button;
    QCheckBox *clear_ram_checkbox;
    int multiplier = 1;

    void setHz();
    void setkHz();
    void setMHz();
    void setClockRate(int value);

    QWidget *createTabs();
    QWidget *createClockRateTab();
    QWidget *createGeneralTab();

    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
};

#endif // SETTINGS_H