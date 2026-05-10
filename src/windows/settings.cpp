#include "settings.h"

SettingsDialog::SettingsDialog()
{
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    // setFixedSize(QSize(350, 250));
    setWindowTitle("Settings");

    setupUi(this);

    // QPushButton *reset_button = new QPushButton("Reset Clock", this);
    // reset_button->setGeometry(250, 15, 80, 30);
    // unit_combo->setGeometry(240, 100, 90, 30);

    connect(horizontalSlider, &QSlider::valueChanged, this, &SettingsDialog::setClockRate);
    connect(radioButton_Hz, &QRadioButton::clicked, this, &SettingsDialog::setHz);
    connect(radioButton_kHz, &QRadioButton::clicked, this, &SettingsDialog::setkHz);
    connect(radioButton_MHz, &QRadioButton::clicked, this, &SettingsDialog::setMHz);
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

void SettingsDialog::setClockRate(int value)
{
    int clock_rate = multiplier * value;

    clock_rate_label->setText(QString("Clock Rate (%1)").arg(format(clock_rate)));

    if (clock_rate < 200)
    {
        warning_label->show();
    }
    else
    {
        warning_label->hide();
    }

    emu_ptr->set_clock_rate(clock_rate);
}

// void SettingsDialog::onFreqInputChanged()
// {
//     bool ok;
//     double val = freq_input->text().toDouble(&ok);
//     if (!ok) return;

//     double hz;
//     switch (unit_combo->currentIndex()) {
//         case 2:  hz = val * 1000000.0; break;
//         case 1:  hz = val * 1000.0;    break;
//         default: hz = val;             break;
//     }

//     int new_rate = (int)(hz / 10000.0);
//     new_rate = qBound(slider->minimum(), new_rate, slider->maximum());
//     slider->setValue(new_rate);
// }

// void SettingsDialog::onUnitChanged(int index)
// {
//     double hz = (double)emu_ptr->get_clock_rate();
//     double display_val;
//     switch (index) {
//         case 2:  display_val = hz / 1000000.0; break;
//         case 1:  display_val = hz / 1000.0;    break;
//         default: display_val = hz;             break;
//     }
//     freq_input->blockSignals(true);
//     freq_input->setText(QString::number(display_val, 'g', 6));
//     freq_input->blockSignals(false);
// }

void SettingsDialog::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;

    int clock_rate = emu_ptr->get_clock_rate();

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

    if (clock_rate < 20)
    {
        warning_label->show();
    }
    else
    {
        warning_label->hide();
    }
}
