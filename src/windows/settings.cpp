#include "settings.h"
#include "../common/default.h"
#include "../util/log.h"

SettingsDialog::SettingsDialog()
{
}

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    // setFixedSize(QSize(350, 250));

    setupUi(this);
}

QWidget *SettingsDialog::createTabs()
{
    QTabWidget *tabs = new QTabWidget(this);
    tabs->addTab(createClockRateTab(), "Clock Rate");
    tabs->addTab(createGeneralTab(), "General");
    tabs->addTab(createDisplayTab(), "Display");
    return tabs;
}

QWidget *SettingsDialog::createClockRateTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *verticalLayout = new QVBoxLayout(tab);

    QHBoxLayout *horizontalLayout = new QHBoxLayout((QWidget *)verticalLayout);
    horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

    radioButton_Hz = new QRadioButton(tab);
    radioButton_Hz->setObjectName(QStringLiteral("radioButton"));
    radioButton_Hz->setFixedHeight(24);

    radioButton_kHz = new QRadioButton(tab);
    radioButton_kHz->setObjectName(QStringLiteral("radioButton_2"));
    radioButton_kHz->setFixedHeight(24);

    radioButton_MHz = new QRadioButton(tab);
    radioButton_MHz->setObjectName(QStringLiteral("radioButton_3"));
    radioButton_MHz->setFixedHeight(24);

    horizontalLayout->addWidget(radioButton_Hz);
    horizontalLayout->addWidget(radioButton_kHz);
    horizontalLayout->addWidget(radioButton_MHz);

    horizontalSlider = new QSlider(tab);
    horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
    horizontalSlider->setOrientation(Qt::Horizontal);
    horizontalSlider->setFocusPolicy(Qt::StrongFocus);
    horizontalSlider->setTickPosition(QSlider::TicksBothSides);
    horizontalSlider->setTickInterval(100);
    horizontalSlider->setSingleStep(1);
    horizontalSlider->setMinimum(1);
    horizontalSlider->setMaximum(999);
    horizontalSlider->setFixedHeight(24);

    clock_rate_label = new QLabel("Clock Rate", tab);
    clock_rate_label->setAlignment(Qt::AlignCenter);
    clock_rate_label->setFixedHeight(24);

    warning_label = new QLabel("The ROM key press routine scans the keypad and waits to eliminate contact bouncing on a real ET-3400. Setting the clock rate below 200kHz will affect keypad response, requiring you to hold down (and release between key presses) the buttons for slightly longer.", tab);
    warning_label->setWordWrap(true);
    warning_label->hide();

    reset_button = new QPushButton("Reset Clock", tab);
    reset_button->setFixedWidth(120);

    QHBoxLayout *resetLayout = new QHBoxLayout();
    resetLayout->addStretch();
    resetLayout->addWidget(reset_button);
    resetLayout->addStretch();

    verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addWidget(horizontalSlider);
    verticalLayout->addWidget(clock_rate_label);
    verticalLayout->addWidget(warning_label);
    verticalLayout->addLayout(resetLayout);

    tab->setLayout(verticalLayout);

    connect(radioButton_Hz, &QRadioButton::clicked, this, &SettingsDialog::setHz);
    connect(radioButton_kHz, &QRadioButton::clicked, this, &SettingsDialog::setkHz);
    connect(radioButton_MHz, &QRadioButton::clicked, this, &SettingsDialog::setMHz);
    connect(horizontalSlider, &QSlider::valueChanged, this, &SettingsDialog::setClockRateValue);
    connect(reset_button, &QPushButton::clicked, this, &SettingsDialog::resetClockRate);

    return tab;
}

QWidget *SettingsDialog::createGeneralTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *verticalLayout = new QVBoxLayout(tab);

    QHBoxLayout *horizontalLayout = new QHBoxLayout((QWidget *)verticalLayout);
    clear_ram_checkbox = new QCheckBox("Clear before loading RAM", tab);
    QLabel *clear_ram_label = new QLabel("Sets RAM to zero before loading new content", tab);
    clear_ram_label->setWordWrap(true);

    horizontalLayout->addWidget(clear_ram_checkbox);

    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addWidget(clear_ram_label);

    connect(clear_ram_checkbox, &QCheckBox::toggled, [this](bool checked)
            { settings->clearRamOnLoad = checked; });

    return tab;
}

QWidget *SettingsDialog::createDisplayTab()
{
    QWidget *tab = new QWidget();
    QVBoxLayout *verticalLayout = new QVBoxLayout(tab);

    QHBoxLayout *horizontalLayout = new QHBoxLayout((QWidget *)verticalLayout);
    show_bit0_display_writes_checkbox = new QCheckBox("Only show writes to bit 0 for display addresses in the memory pane", tab);
    QLabel *show_bit0_label = new QLabel("Display latches are only connected to bit 0 of the data bus. Enabling this option will cause the memory pane to show only writes to bit 0", tab);
    show_bit0_label->setWordWrap(true);

    horizontalLayout->addWidget(show_bit0_display_writes_checkbox);

    verticalLayout->addLayout(horizontalLayout);
    verticalLayout->addWidget(show_bit0_label);

    connect(show_bit0_display_writes_checkbox, &QCheckBox::toggled, [this](bool checked)
            { settings->showBit0DisplayWrites = checked; });

    return tab;
}

void SettingsDialog::setupUi(QDialog *Dialog)
{
    if (Dialog->objectName().isEmpty())
        Dialog->setObjectName(QStringLiteral("Settings"));

    Dialog->resize(408, 280);

    QVBoxLayout *mainLayout = new QVBoxLayout(Dialog);
    mainLayout->setObjectName(QStringLiteral("mainLayout"));

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setObjectName(QStringLiteral("buttonBox"));
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);
    QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

    mainLayout->addWidget(createTabs());
    mainLayout->addStretch();
    mainLayout->addWidget(buttonBox);

    setLayout(mainLayout);

    retranslateUi(Dialog);

    QMetaObject::connectSlotsByName(Dialog);

    setWindowTitle("Settings");

} // setupUi

void SettingsDialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
    radioButton_Hz->setText(QApplication::translate("Dialog", "Hz", nullptr));
    radioButton_kHz->setText(QApplication::translate("Dialog", "kHz", nullptr));
    radioButton_MHz->setText(QApplication::translate("Dialog", "MHz", nullptr));
} // retranslateUi

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

    settings->clockRate = clock_rate;
}

void SettingsDialog::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;

    int clock_rate = emu_ptr->get_clock_rate();

    setClockRate(clock_rate);
}

void SettingsDialog::set_settings(Settings *settings)
{
    this->settings = settings;
    clear_ram_checkbox->setChecked(settings->clearRamOnLoad);
    show_bit0_display_writes_checkbox->setChecked(settings->showBit0DisplayWrites);
}
