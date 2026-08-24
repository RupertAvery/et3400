
#include "dip_array.h"

#include <QPainter>
#include <QPainterPath>
#include "../util/log.h"

DIPArray::DIPArray(QWidget *parent)
    : QWidget(parent)
{
    dip[0].load(":/images/dip_off.png");
    dip[1].load(":/images/dip_on.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    running = true;

    m_font = QFont("Courier", 4);
    // m_paintTimer = new QTimer(this);
    // m_paintTimer->start(17); // 17ms, or every 1/60th of a second
    // connect(this->m_paintTimer, SIGNAL(timeout()), this, SLOT(redraw()));



    action = new QAction;
    connect(action, &QAction::triggered, this, &DIPArray::redraw);

    this->setFixedSize(QSize(128, 32));
}

DIPArray::~DIPArray()
{
    delete (action);
}

void DIPArray::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // painter.setBrush(QBrush(Qt::transparent));

    // painter.fillRect(this->rect(), painter.brush());

	painter.setFont(m_font);
    painter.setPen(Qt::black);

    painter.save();

    for (int bit = 0; bit <= 7; bit++)
    {
        int pos = 7 - bit;
        uint8_t state = (byte >> bit) & 1;

        painter.save();

        painter.translate(pos * 16, 0);
        painter.drawPixmap(0, 0, dip[state]);
        painter.drawText(4, 10, QString("%1").arg(bit));

        painter.restore();
    }

    painter.restore();

    painter.end();
}

void DIPArray::redraw()
{
    this->update();
}

void DIPArray::update_display()
{
    action->trigger();
}

void DIPArray::set_device(io_device *device)
{
    this->device = device;

    if (device == nullptr)
        return;

    // keep the new device in step with the current switch positions
    device->write_direct(0, byte);
}


void DIPArray::mousePressEvent(QMouseEvent *event)
{
    if (device == nullptr)
        return;

    int y = event->y();
    int x = event->x();

    int bit = 7 - (x / 16);

    uint8_t state = (byte >> bit) & 1;

    if (state == 1)
    {
        byte = byte & ~(1 << bit);
    }
    else
    {
        byte = byte | (1 << bit);
    }

    device->write_direct(0, byte);

    update();
}