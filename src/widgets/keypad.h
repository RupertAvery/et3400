#ifndef KEYPAD_H
#define KEYPAD_H

#include <thread>
#include <QPushButton>
#include <QSize>
#include <QGridLayout>
#include <QMap>
#include <QPair>
#include <QWidget>
#include <QIcon>
#include "../dev/keypad_dev.h"


class Keypad : public QWidget
{
    Q_OBJECT

public:
    explicit Keypad(QWidget *parent = nullptr);
    ~Keypad();
    keypad_io *device;

public slots:
    void press_key(keypad_io::Keys key);
    void release_key(keypad_io::Keys key);

private:
    QMap<keypad_io::Keys, QPushButton*> _buttons;
    QMap<keypad_io::Keys, QPair<QIcon, QIcon>> _icons;
    QPushButton *create_button(QString name, QString icon_src, QString icon_pressed_src, keypad_io::Keys key);


};

#endif // KEYPAD_H
