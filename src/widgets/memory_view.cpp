#include <QDebug>
#include "memory_view.h"

MemoryView::MemoryView(QWidget *parent)
    : QFrame(parent)
{
    //setMidLineWidth(0);
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(3);

    // setBackgroundRole(QPalette::Base);
    // setAutoFillBackground(true);
    running = true;
    start = 0;
    end = 0x100;
    offset = 0;
    is_memory_set = false;;

    m_paintTimer = new QTimer(this);
    m_paintTimer->start(36); // 38ms, or every 1/30th of a second
    connect(this->m_paintTimer, &QTimer::timeout, this, &MemoryView::redraw);

    buffer = new QPixmap;
    // action = new QAction;
    // connect(action, &QAction::triggered, this, &Display::redraw);

    // this->setFixedSize(QSize(320, 85));
    setLineWidth(3);
}

MemoryView::~MemoryView()
{
    qDebug() << "Memory view destroy";
    m_paintTimer->stop();
    delete m_paintTimer;
    delete buffer;
    qDebug() << "Memory view destroy done";
}

void MemoryView::wheelEvent(QWheelEvent *event)
{
    int degrees = event->delta() / 8;
    int steps = degrees / 15;

    if (event->orientation() == Qt::Vertical)
    {
        scroll(steps);
        emit on_scroll(steps);
    }
    event->accept();
}

void MemoryView::scroll(int steps)
{
    offset -= steps;
    if (offset < 0)
        offset = 0;
    if (offset > max_vscroll)
        offset = max_vscroll;
    this->update();
}

void MemoryView::scrollTo(int value)
{
    offset = value;
    if (offset < 0)
        offset = 0;
    if (offset > max_vscroll)
        offset = max_vscroll;
    this->update();
}

void MemoryView::bufferDraw()
{
    QPainter painter(buffer);
    painter.setRenderHint(QPainter::TextAntialiasing);
    // Clear display
    painter.setBrush(QBrush(Qt::white));

    painter.fillRect(contentsRect(), painter.brush());

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
    // painter.restore(); *
    /* set the modified font to the painter */
    QFont font("Courier", 12);

    font.setStyleStrategy(QFont::NoAntialias);
    ;

    painter.setFont(font);

    int s = start + offset * 8;
    int e = s + visible_items * 8;
    e = e > end ? end : e;

    int y = 20;

    // QString addr = QString("$%1:");
    // QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");

    QChar filler = QLatin1Char('0');
    painter.save();
    for (int address = s; address <= e; address += 8)
    {
        painter.setPen(Qt::blue);
        painter.drawText(0, y, QString("$%1:").arg(address, 4, 16, filler));
        painter.setPen(Qt::blue);

        QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
        int i = 0;
        while (address + i <= e && i < 8)
        {
            data = data.arg(memory[address + i - start], 2, 16, filler);
            i++;
        }
        while (i < 8)
        {
            data = data.arg(0, 2, 16, filler);
            i++;
        }
        //    .arg(memory[address + 1], 2, 16, QLatin1Char('0'))
        //    .arg(memory[address + 2], 2, 16, QLatin1Char('0'))
        //    .arg(memory[address + 3], 2, 16, QLatin1Char('0'))
        //    .arg(memory[address + 4], 2, 16, QLatin1Char('0'))
        //    .arg(memory[address + 5], 2, 16, QLatin1Char('0'))
        //    .arg(memory[address + 6], 2, 16, QLatin1Char('0'))
        //    .arg(memory[address + 7], 2, 16, QLatin1Char('0'));

        painter.drawText(80, y, data);
        y += item_height;
    }
    painter.restore();
}

void MemoryView::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    buffer = new QPixmap(size);
    visible_items = size.height() / item_height;
    int x = (end - start) / 8 - visible_items + 1;
    max_vscroll = x > 0 ? x : 0;
    emit on_size(max_vscroll);
}

void MemoryView::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    if (is_memory_set) {
        bufferDraw();
        painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
        painter.end();
    }
}

// void MemoryView::set_range()
// {
// }

void MemoryView::redraw()
{
    this->update();
}

void MemoryView::update_display()
{
    //action->trigger();
}

void MemoryView::set_range(offs_t start, offs_t end, uint8_t *memory)
{
    this->start = start;
    this->end = end;
    this->memory = memory;

    visible_items = height() / item_height;
    int x = (end - start) / 8 - visible_items + 1;
    max_vscroll = x > 0 ? x : 0;
    emit on_size(max_vscroll);
    is_memory_set = true;
}

void MemoryView::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;

}