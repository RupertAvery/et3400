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
#include <QDialogButtonBox>

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
    SettingsDialog(QWidget *parent);
    void set_emulator(et3400emu *emu);

signals:
    // void valueChanged(int value);

public slots:
    void setClockRate(int value);

private slots:
    // void onFreqInputChanged();
    // void onUnitChanged(int index);

private:
    QComboBox *unit_combo;
    QLineEdit *freq_input;
    QLabel *clock_rate_label;
    QLabel *warning_label;
    et3400emu *emu_ptr;

    QDialogButtonBox *buttonBox;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QRadioButton *radioButton_Hz;
    QRadioButton *radioButton_kHz;
    QRadioButton *radioButton_MHz;
    QSlider *horizontalSlider;
    int multiplier = 1;

    void setHz();
    void setkHz();
    void setMHz();

    void setupUi(QDialog *Dialog)
    {
        if (Dialog->objectName().isEmpty())
            Dialog->setObjectName(QStringLiteral("Dialog"));
        Dialog->resize(408, 280);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));

        radioButton_Hz = new QRadioButton(Dialog);
        radioButton_Hz->setObjectName(QStringLiteral("radioButton"));
        horizontalLayout->addWidget(radioButton_Hz);

        radioButton_kHz = new QRadioButton(Dialog);
        radioButton_kHz->setObjectName(QStringLiteral("radioButton_2"));
        horizontalLayout->addWidget(radioButton_kHz);

        radioButton_MHz = new QRadioButton(Dialog);
        radioButton_MHz->setObjectName(QStringLiteral("radioButton_3"));
        horizontalLayout->addWidget(radioButton_MHz);

        horizontalSlider = new QSlider(Dialog);
        horizontalSlider->setObjectName(QStringLiteral("horizontalSlider"));
        horizontalSlider->setOrientation(Qt::Horizontal);
        horizontalSlider->setFocusPolicy(Qt::StrongFocus);
        horizontalSlider->setTickPosition(QSlider::TicksBothSides);
        horizontalSlider->setTickInterval(100);
        horizontalSlider->setSingleStep(1);
        horizontalSlider->setMinimum(1);
        horizontalSlider->setMaximum(999);

        clock_rate_label = new QLabel("Clock Rate", Dialog);
        clock_rate_label->setStyleSheet("font-size:12px;");
        clock_rate_label->setAlignment(Qt::AlignCenter);

        warning_label = new QLabel("The ROM key press routine scans the keypad and waits to eliminate contact bouncing on a real ET-3400. Setting the clock rate below 200kHz will affect keypad response, requiring you to hold down (and release between key presses) the buttons for slightly longer.", Dialog);
        warning_label->setStyleSheet("font-size:12px;");
        warning_label->setWordWrap(true);
        warning_label->hide();

        buttonBox = new QDialogButtonBox(Dialog);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

        verticalLayout = new QVBoxLayout(Dialog);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        verticalLayout->addLayout(horizontalLayout);
        verticalLayout->addWidget(horizontalSlider);
        verticalLayout->addWidget(clock_rate_label);
        verticalLayout->addWidget(warning_label);
        verticalLayout->addStretch();
        verticalLayout->addWidget(buttonBox);

        retranslateUi(Dialog);
        QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

        QMetaObject::connectSlotsByName(Dialog);
    } // setupUi

    void retranslateUi(QDialog *Dialog)
    {
        Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
        radioButton_Hz->setText(QApplication::translate("Dialog", "Hz", nullptr));
        radioButton_kHz->setText(QApplication::translate("Dialog", "kHz", nullptr));
        radioButton_MHz->setText(QApplication::translate("Dialog", "MHz", nullptr));
    } // retranslateUi
};

#endif // SETTINGS_H