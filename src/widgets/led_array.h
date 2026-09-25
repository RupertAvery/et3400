#ifndef LED_ARRAY_H
#define LED_ARRAY_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
// #include <QTimer>
#include <QAction>
#include "../dev/io_dev.h"
#include "../util/settings.h"

class LEDArray : public QWidget
{
    Q_OBJECT

public:
    explicit LEDArray(QWidget *parent = nullptr);
    ~LEDArray();
    io_device *device = nullptr;
    void update_display();
    void set_device(io_device *device);

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap led[2];
    QAction *action;
    bool running;
    uint8_t byte = 0;

};

#endif // LED_ARRAY_H
