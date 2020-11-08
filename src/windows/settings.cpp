#include "settings.h"

SettingsDialog::SettingsDialog()
{
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    label = new QLabel("Clock Rate");

    QWidget *mainwidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(mainwidget);

    slider = new QSlider(Qt::Horizontal);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(1);
    slider->setMinimum(1);
    slider->setMaximum(400);

    connect(slider, &QSlider::valueChanged, this, &SettingsDialog::setClockRate);

    layout->addWidget(label);
    layout->addWidget(slider);
    mainwidget->setFixedHeight(150);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch(1);
    mainLayout->addWidget(mainwidget);
    mainLayout->addStretch(1);
    mainLayout->setMargin(10);

    setLayout(mainLayout);

    this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

    setFixedSize(QSize(350, 250));
    setWindowTitle("Settings");
}

void SettingsDialog::setClockRate(int value)
{
    label->setText(QString("Clock Rate (%1)").arg(format((float)1000000 * value / 100)));
    emu_ptr->set_clock_rate(value);
}

void SettingsDialog::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;
    int clock_rate = emu_ptr->get_clock_rate();
    slider->setValue(clock_rate);
    label->setText(QString("Clock Rate (%1)").arg(format((float)1000000 * clock_rate / 100)));
}
