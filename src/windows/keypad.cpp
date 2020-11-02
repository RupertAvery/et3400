
#include "keypad.h"

Keypad::Keypad(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setColumnStretch(2, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("0"), QString::fromUtf8(":/buttons/0.bmp"), et3400emu::Key0), 5, 0, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("Reset"), QString::fromUtf8(":/buttons/Reset.bmp"), et3400emu::KeyReset), 5, 1, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("1"), QString::fromUtf8(":/buttons/1.bmp"), et3400emu::Key1), 4, 0, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("2"), QString::fromUtf8(":/buttons/2.bmp"), et3400emu::Key2), 4, 1, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("3"), QString::fromUtf8(":/buttons/3.bmp"), et3400emu::Key3), 4, 2, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("4"), QString::fromUtf8(":/buttons/4.bmp"), et3400emu::Key4), 3, 0, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("5"), QString::fromUtf8(":/buttons/5.bmp"), et3400emu::Key5), 3, 1, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("6"), QString::fromUtf8(":/buttons/6.bmp"), et3400emu::Key6), 3, 2, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("7"), QString::fromUtf8(":/buttons/7.bmp"), et3400emu::Key7), 2, 0, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("8"), QString::fromUtf8(":/buttons/8.bmp"), et3400emu::Key8), 2, 1, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("9"), QString::fromUtf8(":/buttons/9.bmp"), et3400emu::Key9), 2, 2, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("A"), QString::fromUtf8(":/buttons/key A.bmp"), et3400emu::KeyA), 1, 0, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("B"), QString::fromUtf8(":/buttons/key B.bmp"), et3400emu::KeyB), 1, 1, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("C"), QString::fromUtf8(":/buttons/key C.bmp"), et3400emu::KeyC), 1, 2, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("D"), QString::fromUtf8(":/buttons/key D.bmp"), et3400emu::KeyD), 0, 0, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("E"), QString::fromUtf8(":/buttons/key E.bmp"), et3400emu::KeyE), 0, 1, 1, 1);
    mainLayout->addWidget(create_button(QString::fromUtf8("F"), QString::fromUtf8(":/buttons/key F.bmp"), et3400emu::KeyF), 0, 2, 1, 1);

    this->setStyleSheet("QPushButton { margin: 0 0 0 0; padding 0 0 0 0 }");
    this->setFixedSize(QSize(175, 300));
    setLayout(mainLayout);
}

QPushButton *Keypad::create_button(QString name, QString icon, et3400emu::Keys key)
{
    QPushButton *button = new QPushButton(this);
    button->setObjectName(name);
    button->setFixedSize(QSize(37, 37));
    //button->setGeometry(QRect(0, 0, 37, 37));
    QIcon icon2;
    icon2.addFile(icon, QSize(), QIcon::Normal, QIcon::Off);
    button->setIcon(icon2);
    button->setIconSize(QSize(37, 37));

    connect(button, &QPushButton::pressed, this, [this, key] { press_key(key); });
    connect(button, &QPushButton::released, this, [this, key] { release_key(key); });

    return button;
}

void Keypad::press_key(et3400emu::Keys key)
{
    emu_ptr->press_key(key);
}

void Keypad::release_key(et3400emu::Keys key)
{
    emu_ptr->release_key(key);
}

void Keypad::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;
}

