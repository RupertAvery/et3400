#ifndef KEYPAD_H
#define KEYPAD_H

#include "../emu/et3400.h"
#include "../util/common.h"
#include <thread>
#include <QPushButton>
#include <QSize>
#include <QGridLayout>
#include <QWidget>
#include <QIcon>

class Keypad : public QWidget
{
    Q_OBJECT

public:
    explicit Keypad(QWidget *parent = nullptr);
    void set_emulator(et3400emu *emu);

public slots:
    void press_key(et3400emu::Keys key);
    void release_key(et3400emu::Keys key);

private:
    et3400emu *emu_ptr;
    QPushButton *create_button(QString name, QString icon, et3400emu::Keys key);
};

#endif // KEYPAD_H
