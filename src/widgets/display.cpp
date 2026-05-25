
#include "display.h"

#include <QPainter>
#include <QPainterPath>
#include "../util/log.h"

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
    device->write_hook = [this](offs_t addr, uint8_t data)
    {
        addr = addr - 0xC100;
        addr = addr & 0b01110111;

        int position = (6 - ((addr & 0xF0) >> 4)) & 0x7;
        int seg = addr & 0x7;
        uint8_t state = data & 1;

        if (state == 1)
        {
            segment[position] |= (1 << seg);
        }
        else
        {
            segment[position] &= ~(1 << seg);
        }
    };

    // device->read_hook = [this](offs_t addr)
    // {
    //     addr = addr - 0xC100;
    //     addr = addr & 0b01110111;
    //     return displaymem[addr];
    // };

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
    QString letters[] = {"H", "I", "N", "Z", "V", "C"};
    // Clear display
    painter.setBrush(QBrush(Qt::black));

    painter.fillRect(this->rect(), painter.brush());

    painter.setPen(Qt::white);

    painter.save();

    // Go over each position
    for (int pos = 0; pos < 6; pos++)
    {
        // Go over each segment
        for (int seg = 0; seg <= 7; seg++)
        {
            uint8_t state = (segment[pos] >> seg) & 1;

            painter.save();
            painter.translate(20 + pos * 45, 10);

            switch (seg)
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

            painter.save();
            painter.translate(20 + pos * 45, 10);
            painter.drawText(15, 65, letters[pos]);
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

void Display::update_display()
{
    action->trigger();
}