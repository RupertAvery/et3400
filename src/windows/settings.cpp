#include "settings.h"

SettingsDialog::SettingsDialog()
{
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setFixedSize(QSize(350, 250));
    setWindowTitle("Settings");

    QPushButton *reset_button = new QPushButton("Reset Clock", this);
    reset_button->setGeometry(250, 15, 80, 30);

    clock_rate_label = new QLabel("Clock Rate", this);
    clock_rate_label->setStyleSheet("font-size:12px;");
    clock_rate_label->setGeometry(10, 10, 150, 40);

    warning_label = new QLabel("The ROM key press routine scans the keypad and waits to eliminate contact bouncing on a real ET-3400. Setting the clock rate below 200kHz will affect keypad response, requiring you to hold down (and release between key presses) the buttons for slightly longer.", this);
    warning_label->setStyleSheet("font-size:12px;");
    warning_label->hide();
    warning_label->setWordWrap(true);
    warning_label->setGeometry(10, 150, 320, 100);
    // QWidget *mainwidget = new QWidget;
    // QVBoxLayout *layout = new QVBoxLayout(mainwidget);

    slider = new QSlider(Qt::Horizontal, this);
    slider->setGeometry(10, 50, 320, 40);
    slider->setFocusPolicy(Qt::StrongFocus);
    slider->setTickPosition(QSlider::TicksBothSides);
    slider->setTickInterval(10);
    slider->setSingleStep(1);
    slider->setMinimum(1);
    slider->setMaximum(400);

    QLabel *freq_label = new QLabel("Frequency:", this);
    freq_label->setGeometry(10, 100, 80, 30);

    freq_input = new QLineEdit(this);
    freq_input->setGeometry(90, 100, 140, 30);

    unit_combo = new QComboBox(this);
    unit_combo->addItems({"Hz", "kHz", "MHz"});
    unit_combo->setCurrentIndex(1);
    unit_combo->setGeometry(240, 100, 90, 30);

    connect(slider, &QSlider::valueChanged, this, &SettingsDialog::setClockRate);
    connect(reset_button, &QPushButton::clicked, this, [this] { slider->setValue(100); });
    connect(freq_input, &QLineEdit::editingFinished, this, &SettingsDialog::onFreqInputChanged);
    connect(unit_combo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &SettingsDialog::onUnitChanged);

    // addWidget(clock_rate_label);
    // addWidget(warning_label);
    // addWidget(slider);

    // layout->addWidget(clock_rate_label, 1, Qt::AlignTop);
    // layout->addWidget(slider, 1, Qt::AlignTop);
    // layout->addWidget(warning_label, 1, Qt::AlignTop);

    // mainwidget->setFixedHeight(150);

    // QVBoxLayout *mainLayout = new QVBoxLayout;
    // mainLayout->addWidget(mainwidget, 1, Qt::AlignTop);
    // mainLayout->setMargin(10);

    // setLayout(mainLayout);
}

void SettingsDialog::setClockRate(int clock_rate)
{
    clock_rate_label->setText(QString("Clock Rate (%1)").arg(format((float)1000000 * clock_rate / 100)));
    if (clock_rate < 20)
    {
        warning_label->show();
    }
    else
    {
        warning_label->hide();
    }

    double hz = (double)clock_rate * 10000.0;
    double display_val;
    switch (unit_combo->currentIndex()) {
        case 2:  display_val = hz / 1000000.0; break;
        case 1:  display_val = hz / 1000.0;    break;
        default: display_val = hz;             break;
    }
    freq_input->blockSignals(true);
    freq_input->setText(QString::number(display_val, 'g', 6));
    freq_input->blockSignals(false);

    emu_ptr->set_clock_rate(clock_rate);
}

void SettingsDialog::onFreqInputChanged()
{
    bool ok;
    double val = freq_input->text().toDouble(&ok);
    if (!ok) return;

    double hz;
    switch (unit_combo->currentIndex()) {
        case 2:  hz = val * 1000000.0; break;
        case 1:  hz = val * 1000.0;    break;
        default: hz = val;             break;
    }

    int new_rate = (int)(hz / 10000.0);
    new_rate = qBound(slider->minimum(), new_rate, slider->maximum());
    slider->setValue(new_rate);
}

void SettingsDialog::onUnitChanged(int index)
{
    double hz = (double)emu_ptr->get_clock_rate() * 10000.0;
    double display_val;
    switch (index) {
        case 2:  display_val = hz / 1000000.0; break;
        case 1:  display_val = hz / 1000.0;    break;
        default: display_val = hz;             break;
    }
    freq_input->blockSignals(true);
    freq_input->setText(QString::number(display_val, 'g', 6));
    freq_input->blockSignals(false);
}

void SettingsDialog::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;
    int clock_rate = emu_ptr->get_clock_rate();
    slider->setValue(clock_rate);
    if (clock_rate < 20)
    {
        warning_label->show();
    }
    else
    {
        warning_label->hide();
    }
}

