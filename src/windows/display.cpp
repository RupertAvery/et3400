
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

    m_paintTimer = new QTimer(this);
    //thread = std::thread(&Display::update, this);
    m_paintTimer->start(17);
    connect(this->m_paintTimer, SIGNAL(timeout()), this, SLOT(redraw()));

    this->setFixedSize(QSize(320, 85));
}

void Display::set_memory(uint8_t *ptr)
{
    memptr = ptr;
}

Display::~Display()
{
    m_paintTimer->stop();
}

void Display::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    // painter.setPen(pen);
    // painter.setBrush(brush);
    // if (antialiased)
    //     painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(QBrush(Qt::black));
    painter.fillRect(this->rect(), painter.brush());

    for (int address = 0xC16F; address >= 0xC110; address--)
    {
        if ((address & 0x08) != 0x08)
        {
            int position = 6 - ((address & 0xF0) >> 4);
            int segment = address & 0x7;
            uint8_t segdata = memptr[address];
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

    painter.end();
}

QSize Display::minimumSizeHint() const
{
    return QSize(100, 100);
}

QSize Display::sizeHint() const
{
    return QSize(400, 200);
}

void Display::redraw()
{
    this->update();
}