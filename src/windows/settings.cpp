#include "settings.h"
#include "../common/default.h"

SettingsDialog::SettingsDialog()
{
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    // setFixedSize(QSize(350, 250));
    setWindowTitle("Settings");

    setupUi(this);

    connect(radioButton_Hz, &QRadioButton::clicked, this, &SettingsDialog::setHz);
    connect(radioButton_kHz, &QRadioButton::clicked, this, &SettingsDialog::setkHz);
    connect(radioButton_MHz, &QRadioButton::clicked, this, &SettingsDialog::setMHz);
    connect(horizontalSlider, &QSlider::valueChanged, this, &SettingsDialog::setClockRateValue);
    connect(reset_button, &QPushButton::clicked, this, &SettingsDialog::resetClockRate);
}

void SettingsDialog::setHz()
{
    multiplier = 1;

    horizontalSlider->setTickInterval(50);
    horizontalSlider->setSingleStep(1);
    horizontalSlider->setMinimum(1);
    horizontalSlider->setMaximum(999);
}

void SettingsDialog::setkHz()
{
    multiplier = 1000;

    horizontalSlider->setTickInterval(50);
    horizontalSlider->setSingleStep(1);
    horizontalSlider->setMinimum(1);
    horizontalSlider->setMaximum(999);
}

void SettingsDialog::setMHz()
{
    multiplier = 1000000;

    horizontalSlider->setTickInterval(1);
    horizontalSlider->setSingleStep(1);
    horizontalSlider->setMinimum(1);
    horizontalSlider->setMaximum(4);
}

void SettingsDialog::resetClockRate()
{
    setClockRate(DEFAULT_CLOCK_RATE);
}

void SettingsDialog::setClockRateValue(int value)
{
    int clock_rate = multiplier * value;

    setClockRate(clock_rate);
}

void SettingsDialog::setClockRate(int clock_rate)
{
    clock_rate_label->setText(QString("Clock Rate (%1)").arg(format(clock_rate)));

    if (clock_rate < 1000)
    {
        multiplier = 1;
        radioButton_Hz->setChecked(true);

        horizontalSlider->setTickInterval(50);
        horizontalSlider->setSingleStep(1);
        horizontalSlider->setMinimum(1);
        horizontalSlider->setMaximum(999);
    }
    else if (clock_rate < 1000000)
    {
        multiplier = 1000;
        radioButton_kHz->setChecked(true);

        horizontalSlider->setTickInterval(50);
        horizontalSlider->setSingleStep(1);
        horizontalSlider->setMinimum(1);
        horizontalSlider->setMaximum(999);
    }
    else
    {
        multiplier = 1000000;
        radioButton_MHz->setChecked(true);

        horizontalSlider->setTickInterval(1);
        horizontalSlider->setSingleStep(1);
        horizontalSlider->setMinimum(1);
        horizontalSlider->setMaximum(4);
    }

    horizontalSlider->setValue(clock_rate / multiplier);

    if (clock_rate < 200000)
    {
        warning_label->show();
    }
    else
    {
        warning_label->hide();
    }

    emu_ptr->set_clock_rate(clock_rate);
}

void SettingsDialog::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;

    int clock_rate = emu_ptr->get_clock_rate();

    setClockRate(clock_rate);
}
