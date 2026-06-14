#ifndef MEMORY_VIEW_H
#define MEMORY_VIEW_H

// #include <thread>
#include <QTimer>
#include <QWidget>
#include <QFrame>
#include <QPainter>
#include <QPainterPath>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QGridLayout>
#include <QScrollBar>
#include <QAction>
#include <QFont>
#include <QFontMetrics>
#include <QWheelEvent>
#include <QMenu>

#include "../emu/et3400.h"
#include "../dev/memory_map.h"
#include "../common/common_defs.h"
#include "../util/memory_builder.h"

class MemoryView : public QFrame
{
    Q_OBJECT

public:
    MemoryView();
    MemoryView(QWidget *parent);
    ~MemoryView();
    void update_display();
    void scroll(int steps);
    void scrollTo(int value);
    void scrollIntoView();
    void set_emulator(et3400emu *emu);
    // void set_range(offs_t start, offs_t end, uint8_t *memory);
    void set_device(memory_mapped_device *device);

    void setHeatMapEnabled(bool enabled);
    void setHeatMapDecay(int decay);
    void clearHeatMap();

    void rebuild();

signals:
    void on_scroll(int steps);
    void on_size(int max);
    void on_offset_change(int offset);
    void on_heat_map_change(bool enabled, int decay);
    void on_heat_map_enabled_change(bool enabled);
    void on_heat_map_decay_change(int decay);

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent *event) override;
    bool eventFilter(QObject *obj, QEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    QScrollBar *scrollbar = nullptr;
    QFrame *frame;

    QAction *action;
    QPixmap *buffer;
    QTimer *m_paintTimer;

    et3400emu *emu_ptr;
    memory_mapped_device *device = nullptr;
    std::vector<MemoryLine> *lines;

    bool running;
    offs_t start;
    offs_t end;
    uint8_t *memory;
    uint8_t *last_memory = nullptr;
    uint8_t *shadow_memory = nullptr;
    uint8_t *heat_map = nullptr;

    bool is_device_set = false;

    bool heat_map_enabled = true;
    int heat_map_decay = 12;

    int offset;
    int visible_items;
    int item_height = 20;
    int max_vscroll;

    int blink_counter_max = 10;

    QFont m_font;
    QFontMetrics *m_fm = nullptr;
    QColor m_heat_colors[16];

    bool is_editing = false;
    int selected_address = -1;
    int last_selected_address = -1;
    int editing_address = 0;
    int editing_value = 0;
    int editing_nibble = 0;
    int blink_counter = 0;
    bool gained_focus = false;

    void bufferDraw();
    void start_editing(uint16_t address);
    void stop_editing();
    void showContextMenu(const QPoint &pos);

    const int address_col_width = 80;
    const int data_cell_width = 30;

    void setupUI(QWidget *parent)
    {
        this->setFocusPolicy(Qt::StrongFocus);

        QHBoxLayout *layout = new QHBoxLayout(parent);

        frame = new QFrame(parent);
        frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
        frame->setLineWidth(3);
        frame->setMouseTracking(true);
        frame->installEventFilter(this);

        scrollbar = new QScrollBar(Qt::Orientation::Vertical);

        layout->addWidget(frame);
        layout->addWidget(scrollbar);
        layout->setMargin(0);

        this->setLayout(layout);

        connect(scrollbar, &QScrollBar::sliderMoved, this, &MemoryView::scrollTo);
        connect(scrollbar, &QScrollBar::valueChanged, this, &MemoryView::scrollTo);
    }
};

#endif // MEMORY_VIEW_H
