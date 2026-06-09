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

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    QScrollBar *scrollbar;
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
    int editing_address = 0;
    int editing_value = 0;
    int editing_nibble = 0;
    int blink_counter = 0;

    void bufferDraw();
    void update_offset();
    void start_editing(uint16_t address);
    void stop_editing();
};

#endif // MEMORY_VIEW_H
