#include <QDebug>
#include "disassembly_view.h"

DisassemblyView::DisassemblyView(QWidget *parent)
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
    connect(this->m_paintTimer, &QTimer::timeout, this, &DisassemblyView::redraw);

    buffer = new QPixmap;
    // action = new QAction;
    // connect(action, &QAction::triggered, this, &Display::redraw);

    // this->setFixedSize(QSize(320, 85));
    setLineWidth(3);
}

DisassemblyView::~DisassemblyView()
{
    qDebug() << "DisassemblyView view destroy";
    m_paintTimer->stop();
    delete m_paintTimer;
    delete buffer;
    qDebug() << "DisassemblyView view destroy done";
}

void DisassemblyView::wheelEvent(QWheelEvent *event)
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

void DisassemblyView::scroll(int steps)
{
    offset -= steps;
    if (offset < 0)
        offset = 0;
    if (offset > max_vscroll)
        offset = max_vscroll;
    this->update();
}

void DisassemblyView::scrollTo(int value)
{
    offset = value;
    if (offset < 0)
        offset = 0;
    if (offset > max_vscroll)
        offset = max_vscroll;
    this->update();
}

void DisassemblyView::bufferDraw()
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
    int address = s;
    int ptr = 0;
    while(address <= e)
    {
        painter.setPen(Qt::blue);
        painter.drawText(0, y, QString("$%1:").arg(address, 4, 16, filler));
        painter.setPen(Qt::blue);

        DasmResult result = Disassembler::disassemble(&memory[ptr], start + ptr);
        ptr += result.byteLength;
        address += result.byteLength;
        QString data = QString("%1 %2").arg(result.instruction).arg(result.operand);

        painter.drawText(80, y, data);
        y += item_height;
    }
    painter.restore();
}

void DisassemblyView::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    buffer = new QPixmap(size);
    visible_items = size.height() / item_height;
    int x = (end - start) / 8 - visible_items + 1;
    max_vscroll = x > 0 ? x : 0;
    emit on_size(max_vscroll);
}

void DisassemblyView::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    if (is_memory_set) {
        bufferDraw();
        painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
        painter.end();
    }
}

// void DisassemblyView::set_range()
// {
// }

void DisassemblyView::redraw()
{
    this->update();
}

void DisassemblyView::update_display()
{
    //action->trigger();
}

void DisassemblyView::set_range(offs_t start, offs_t end, uint8_t *memory)
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

void DisassemblyView::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;

}