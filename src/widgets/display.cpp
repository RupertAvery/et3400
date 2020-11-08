
#include "display.h"

#include <QPainter>
#include <QPainterPath>

Display::Display(QWidget *parent)
    : QWidget(parent)
{
    hr[0].load(":/images/hr_off.png");
    hr[1].load(":/images/hr_on.png");
    vt[0].load(":/images/vt_off.png");
    vt[1].load(":/images/vt_on.png");
    dp[0].load(":/images/dp_off.png");
    dp[1].load(":/images/dp_on.png");

    setBackgroundRole(QPalette::Base);
    setAutoFillBackground(true);
    running = true;

    // m_paintTimer = new QTimer(this);
    // m_paintTimer->start(17); // 17ms, or every 1/60th of a second
    // connect(this->m_paintTimer, SIGNAL(timeout()), this, SLOT(redraw()));
    device = new display_io;

    action = new QAction;
    connect(action, &QAction::triggered, this, &Display::redraw);

    this->setFixedSize(QSize(320, 85));
}

Display::~Display()
{
    delete (action);
}

void Display::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);

    // Clear display
    painter.setBrush(QBrush(Qt::black));

    painter.fillRect(this->rect(), painter.brush());

    // // Clear segment area only
    // painter.save();
    // for (int address = 0xC16F; address >= 0xC110; address--)
    // {
    //     if ((address & 0x08) != 0x08)
    //     {
    //         int position = 6 - ((address & 0xF0) >> 4);
    //         painter.save();
    //         painter.translate(20 + position * 45, 10);
    //         painter.fillRect(QRect(0, 0, 38, 54), painter.brush());
    //         painter.restore();
    //     }
    // }
    // painter.restore();

    painter.save();
    for (int address = 0xC16F; address >= 0xC110; address--)
    {
        // prevent writing on all addresses, minor performance increase
        if ((address & 0x08) != 0x08)
        {

            int position = 6 - ((address & 0xF0) >> 4);
            int segment = address & 0x7;
            uint8_t segdata = device->read(address);

            painter.save();
            painter.translate(20 + position * 45, 10);

            uint8_t state = segdata & 1;

            switch (segment)
            {
            case 0:
                painter.drawPixmap(11, 23, hr[state]);
                break;
            case 1:
                painter.drawPixmap(5, 11, vt[state]);
                break;
            case 2:
                painter.drawPixmap(4, 27, vt[state]);
                break;
            case 3:
                painter.drawPixmap(8, 42, hr[state]);
                break;
            case 4:
                painter.drawPixmap(25, 27, vt[state]);
                break;
            case 5:
                painter.drawPixmap(26, 11, vt[state]);
                break;
            case 6:
                painter.drawPixmap(11, 5, hr[state]);
                break;
            case 7:
                painter.drawPixmap(31, 42, dp[state]);
                break;
            }
            painter.restore();
        }
    }
    painter.restore();

    painter.end();
}

void Display::redraw()
{
    this->update();
}

void Display::update_display(){
    action->trigger();
}