
#include "led_array.h"

#include <QPainter>
#include <QPainterPath>
#include "../util/log.h"

LEDArray::LEDArray(QWidget *parent)
    : QWidget(parent)
{
    led[0].load(":/images/led_off.png");
    led[1].load(":/images/led_on.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    running = true;

    // m_paintTimer = new QTimer(this);
    // m_paintTimer->start(17); // 17ms, or every 1/60th of a second
    // connect(this->m_paintTimer, SIGNAL(timeout()), this, SLOT(redraw()));

    action = new QAction;
    connect(action, &QAction::triggered, this, &LEDArray::redraw);

    this->setFixedSize(QSize(128, 16));
}

LEDArray::~LEDArray()
{
    delete action;
}

void LEDArray::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // painter.setBrush(QBrush(Qt::transparent));

    // painter.fillRect(this->rect(), painter.brush());

    painter.setPen(Qt::white);

    painter.save();

    for (int bit = 0; bit <= 7; bit++)
    {
        uint8_t state = (byte >> bit) & 1;

        painter.save();

        painter.translate((7 - bit) * 16, 0);
        painter.drawPixmap(0, 0, led[state]);

        painter.restore();
    }

    painter.restore();

    painter.end();
}

void LEDArray::set_device(io_device *device)
{
    this->device = device;

    if (device == nullptr)
        return;

    device->write_hook = [this](offs_t addr, uint8_t data)
    {
        byte = data;
    };

    // keep the new device in step with what is currently on screen
    device->write_direct(0, byte);
}

void LEDArray::redraw()
{
    this->update();
}

void LEDArray::update_display()
{
    action->trigger();
}
