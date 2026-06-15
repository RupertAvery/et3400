#include "register.h"
#include <QApplication>
#include "colors.h"

RegisterView::RegisterView(RegisterType type, QWidget *parent) : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(3);
    setFocusPolicy(Qt::StrongFocus);

    m_font = QFont("Courier", 12);
    m_font.setWeight(QFont::Medium);
    m_fm = new QFontMetrics(m_font);
    item_height = m_fm->lineSpacing();
    m_type = type;

    blink_timer = new QTimer(this);

    connect(blink_timer, &QTimer::timeout, [this]()
            {
                if (is_editing)
                {
                    blink_state = (blink_state + 1) % 2;
                    update();
                } });

    setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, &QFrame::customContextMenuRequested, this, &RegisterView::showContextMenu);

    buffer = new QPixmap;
}

RegisterView::~RegisterView()
{
    delete m_fm;
    delete buffer;
    delete blink_timer;
}

void RegisterView::set_value(uint16_t value)
{
    this->value = value;
    update();
}

void RegisterView::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    bufferDraw();
    painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
    painter.end();
    QFrame::paintEvent(event);
}

void RegisterView::bufferDraw()
{
    if (buffer->size() != size())
        *buffer = QPixmap(size());

    if (is_selected && !is_editing)
    {
        buffer->fill(selected_bg_color);
    }
    else
    {
        buffer->fill(Qt::white);
    }

    QPainter painter(buffer);
    painter.setFont(m_font);
    painter.setPen(Qt::black);

    QString text;

    uint16_t value = is_editing ? editing_value : this->value;

    if (m_type == WORD)
    {
        text = QString("%1").arg(value, 4, 16, QChar('0')).toUpper();
    }
    else if (m_type == BYTE)
    {
        text = QString("%1").arg(value, 2, 16, QChar('0')).toUpper();
    }
    else if (m_type == FLAGS)
    {
        text = QString("11%1%2%3%4%5%6").arg(value >> 5 & 1).arg(value >> 4 & 1).arg(value >> 3 & 1).arg(value >> 2 & 1).arg(value >> 1 & 1).arg(value >> 0 & 1);
    }

    if (is_editing && blink_state == 1)
    {
        if (m_type == WORD)
        {
            text[editing_nibble] = ' ';
        }
        else if (m_type == BYTE)
        {
            text[editing_nibble] = ' ';
        }
        else if (m_type == FLAGS)
        {
            text[2 + editing_nibble] = ' ';
        }
    }

    QRect text_rect = m_fm->boundingRect(text);

    int x = 5;
    int y = (height() + m_fm->ascent() - m_fm->descent()) / 2;

    if (is_selected && !is_editing)
    {
        painter.setPen(selected_fg_color);
    }
    else if (is_editing)
    {
        painter.setPen(Qt::blue);
    }

    painter.drawText(x, y, text);
}

void RegisterView::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape && is_editing)
    {
        stop_editing();
        update();
        return;
    }

    if (!is_editing)
    {
        if (event->key() == Qt::Key_F2)
        {
            start_editing();
            update();
        }
        else
        {
            QFrame::keyPressEvent(event);
        }
    }
    else
    {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
        {
            value = editing_value;
            emit(on_value_changed(value));
            is_editing = false;
            update();
        }
        else if (event->key() == Qt::Key_Backspace)
        {
            update();
        }
        else if (event->key() == Qt::Key_Left)
        {
            if (editing_nibble > 0)
                editing_nibble--;
            update();
        }
        else if (event->key() == Qt::Key_Right)
        {
            int max_nibble = m_type == FLAGS ? 5 : ((m_type == WORD) ? 3 : 1);
            if (editing_nibble < max_nibble)
                editing_nibble++;
            update();
        }
        else
        {
            int key = event->key();
            if (m_type == FLAGS)
            {
                if (key == Qt::Key_0 || key == Qt::Key_1)
                {
                    int bit = key - Qt::Key_0;
                    int max_nibble = 5;
                    if (bit == 1)
                    {
                        editing_value |= (1 << (5 - editing_nibble));
                    }
                    else
                    {
                        editing_value &= ~(1 << (5 - editing_nibble));
                    }

                    if (editing_nibble < max_nibble)
                        editing_nibble++;
                    update();
                }
                return;
            }
            else
            {
                if ((key >= Qt::Key_0 && key <= Qt::Key_9) || (key >= Qt::Key_A && key <= Qt::Key_F))
                {
                    int value = (key >= Qt::Key_A) ? (key - Qt::Key_A + 10) : (key - Qt::Key_0);
                    int max_nibble = (m_type == WORD) ? 3 : 1;

                    int nibble_mask = 0xFFFF & ~(0xF << (4 * (max_nibble - editing_nibble)));
                    int value_mask = value << (4 * (max_nibble - editing_nibble));

                    editing_value = (editing_value & nibble_mask) | value_mask;

                    if (editing_nibble < max_nibble)
                        editing_nibble++;

                    update();
                }
            }
        }
    }
}

void RegisterView::mousePressEvent(QMouseEvent *event)
{
    is_selected = true;
    update();
}

void RegisterView::mouseDoubleClickEvent(QMouseEvent *event)
{
    start_editing();
    update();
}

void RegisterView::focusInEvent(QFocusEvent *event)
{
    is_selected = true;
    update();
}

void RegisterView::focusOutEvent(QFocusEvent *event)
{
    is_selected = false;
    if (is_editing)
    {
        stop_editing();
    }
    update();
}

void RegisterView::start_editing()
{
    if (!enabled)
    {
        emit on_edit_abort(REGISTER_DISABLED);
        return;
    }
    blink_timer->start(500);
    blink_state = 1;
    is_editing = true;
    editing_nibble = 0;
    editing_value = value;
}

void RegisterView::stop_editing()
{
    blink_timer->stop();
    is_editing = false;
    blink_state = 1;
    editing_nibble = 0;
    editing_value = 0;
}

void RegisterView::showContextMenu(const QPoint &pos)
{
    QMenu contextMenu(tr("Context menu"), this->parentWidget());

    QAction editAction("Edit\tF2", this);
    connect(&editAction, &QAction::triggered, this, [this]
            { start_editing(); });

    contextMenu.addAction(&editAction);

    contextMenu.exec(mapToGlobal(pos));
}