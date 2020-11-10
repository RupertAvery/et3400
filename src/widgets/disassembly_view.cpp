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
    is_memory_set = false;
    selected = -1;
    current = -1;
    breakpoint_icon = QPixmap(":/buttons/BreakpointEnable_16x.png");

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
    if (!is_memory_set)
        return;
    QPainter painter(buffer);
    //painter.setRenderHint(QPainter::TextAntialiasing);
    // Clear display
    painter.setBrush(QBrush(Qt::white));

    painter.fillRect(contentsRect(), painter.brush());

    QFont font("Courier", 12);
    font.setWeight(QFont::Medium);
    //font.setStyleStrategy(QFont::NoAntialias);

    painter.setFont(font);

    int y = 15;
    int ctr = offset;
    int x = 5;
    painter.save();

    std::vector<DisassemblyLine>::iterator line = lines->begin();

    QColor darkblue = QColor("#00018B");
    QColor black = QColor("#000000");
    QColor darkred = QColor("#8B0000");
    QColor green = QColor("#38761D");

    QBrush selected_brush = QBrush(QColor("#0000FF"));
    QBrush breakpoint_brush = QBrush(QColor("#8B0000"));
    QBrush current_brush = QBrush(QColor("#fff181"));
    QBrush current_selected_brush = QBrush(QColor("#81ffaf"));

    QColor white = QColor("#FFFFFF");

    while (ctr < offset + visible_items && ctr - visible_items + 1 < max_vscroll)
    {
        // Default colors
        QColor address_color = darkblue;
        QColor opcode_color = black;
        QColor insgtruction_color = darkblue;
        QColor operand_color = darkred;

        bool has_breakpoint = line[ctr].has_breakpoint;
        bool is_comment = line[ctr].type == DisassemblyType::Comment;
        bool is_selected = ctr == selected;
        bool is_current = ctr == current;

        painter.save();

        if (has_breakpoint)
        {
            painter.drawPixmap(2, y - 13, 16, 16, breakpoint_icon);
        }

        if (is_current && is_selected)
        {
            painter.fillRect(20, y - 14, width() - 22, item_height - 2, current_selected_brush);
        }
        else if (is_current)
        {
            painter.fillRect(20, y - 14, width() - 22, item_height - 2, current_brush);
        }
        else if (is_selected)
        {
            painter.fillRect(20, y - 14, width() - 22, item_height - 2, selected_brush);
        }
        else if (has_breakpoint)
        {
            painter.fillRect(20, y - 14, width() - 22, item_height - 2, breakpoint_brush);
        }

        painter.restore();

        if (is_comment)
        {
            opcode_color = green;
        }

        if (is_current)
        {
            address_color = black;
            opcode_color = black;
            insgtruction_color = black;
            operand_color = black;
        }
        else if (is_selected)
        {
            address_color = white;
            opcode_color = white;
            insgtruction_color = white;
            operand_color = white;
        }
        else if (has_breakpoint)
        {
            address_color = white;
            opcode_color = white;
            insgtruction_color = white;
            operand_color = white;
        }

        painter.setPen(address_color);
        painter.drawText(20, y, QString("$%1:").arg(line[ctr].address, 4, 16, QChar('0').toUpper()));

        if (is_comment)
        {
            painter.setPen(opcode_color);
            painter.drawText(90, y, line[ctr].opcodes);
        }
        else
        {
            painter.setPen(opcode_color);
            painter.drawText(90, y, line[ctr].opcodes);

            if (line[ctr].type == DisassemblyType::Assembly)
            {
                painter.setPen(insgtruction_color);
                painter.drawText(200, y, line[ctr].instruction);
                painter.setPen(operand_color);
                painter.drawText(260, y, line[ctr].operand);
            }
        }

        ctr++;
        y += item_height;
    }
    painter.restore();
}

void DisassemblyView::resizeEvent(QResizeEvent *event)
{
    QSize size = event->size();
    buffer = new QPixmap(size);

    if (is_memory_set)
    {
        visible_items = size.height() / item_height;
        int x = lines->size() - visible_items + 1;
        max_vscroll = x > 0 ? x : 0;
        emit on_size(max_vscroll);
    }
}

void DisassemblyView::load_breakpoints(std::vector<Breakpoint> *breakpoints)
{
    std::vector<Breakpoint>::iterator it = breakpoints->begin();
    while (it != breakpoints->end())
    {
        DisassemblyLine line = find_line((*it).address);
        if (line.type != DisassemblyType::Empty)
        {
            line.has_breakpoint = true;
        }
        it++;
    }
}

DisassemblyLine DisassemblyView::find_line(offs_t address)
{
    std::vector<DisassemblyLine>::iterator it = lines->begin();
    while (it != lines->end())
    {
        if ((*it).address == address)
        {
            return (*it);
        }
        it++;
    }
    return DisassemblyLine{0, DisassemblyType::Empty, NULL, NULL, NULL, false, false, false};
}

void DisassemblyView::toggle_breakpoint(int line_number)
{
    if (line_number > -1)
    {
        DisassemblyLine *line = &lines->at(line_number);
        line->has_breakpoint = !line->has_breakpoint;
        if (line->has_breakpoint)
        {
            emit add_breakpoint(line->address);
        }
        else
        {
            emit remove_breakpoint(line->address);
        }
    }
}

void DisassemblyView::keyPressEvent(QKeyEvent *event)
{
    switch (event->key())
    {
    case Qt::Key_Up:
        selected--;
        break;
    case Qt::Key_Down:
        selected++;
        break;
    case Qt::Key_PageUp:
        selected -= visible_items;
        break;
    case Qt::Key_PageDown:
        selected += visible_items;
        break;
    case Qt::Key_F9:
        toggle_breakpoint(selected);
        break;
    }

    if (selected < 0)
        selected = 0;

    if (selected > lines->size())
        selected = lines->size();

    if (selected > visible_items + offset - 1)
        offset++;

    if (selected < offset)
        offset--;
}

void DisassemblyView::paintEvent(QPaintEvent * /* event */)
{
    QPainter painter(this);
    if (is_memory_set)
    {
        bufferDraw();
        painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
        painter.end();
    }
}

void DisassemblyView::mousePressEvent(QMouseEvent *event)
{
    int y = event->pos().y();
    int x = event->pos().x();
    int line = offset + (y / item_height);
    if (x < 20)
    {
        toggle_breakpoint(line);
    }
    else
    {
        selected = line;
    }
    setFocus();
}

void DisassemblyView::redraw()
{
    this->update();
}

void DisassemblyView::clear_current()
{
    current = -1;
}

void DisassemblyView::clear_selected()
{
    selected = -1;
}

void DisassemblyView::set_current(offs_t address)
{
    int ctr = -1;
    std::vector<DisassemblyLine>::iterator line = lines->begin();
    while (line != lines->end())
    {
        if (line->address == address)
        {
            current = ctr + 1;
            break;
        }
        ctr++;
        line++;
    }

    if (current > offset + visible_items - 1)
    {
        offset++;
        if (offset > max_vscroll)
            offset = max_vscroll;
    }
    if (current > offset + visible_items + 2)
    {
        offset = current - visible_items / 2;
        if (offset > max_vscroll)
            offset = max_vscroll;
    }
    if (current < offset)
    {
        offset = current - visible_items / 2;
        if (offset < 0)
            offset = 0;
    }
}

void DisassemblyView::update_display()
{
    //action->trigger();
}

void DisassemblyView::set_maps(std::vector<Map> *maps)
{
    this->maps = maps;
}

// void DisassemblyView::set_breakpoints(std::vector<Map> *maps)
// {
//     this->maps = maps;
// }

// void DisassemblyView::set_range(offs_t start, offs_t end, uint8_t *memory)
// {

// }

void DisassemblyView::set_range(offs_t start, offs_t end, uint8_t *memory)
{
    this->start = start;
    this->end = end;
    this->memory = memory;

    visible_items = height() / item_height;

    lines = DisassemblyBuilder::build(start, end, emu_ptr->get_block_device(start)->get_mapped_memory(), maps);

    int x = lines->size() - visible_items + 1;
    max_vscroll = x > 0 ? x : 0;
    emit on_size(max_vscroll);
    is_memory_set = true;
}

void DisassemblyView::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;
}