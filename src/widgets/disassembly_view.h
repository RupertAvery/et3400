#ifndef DISASSEMBLYVIEW_H
#define DISASSEMBLYVIEW_H

#include "../util/common.h"
#include "../emu/et3400.h"
#include "../dev/memory_map.h"
#include "../cpu/defs.h"
#include "../dasm/disassembler.h"
//#include <thread>
#include <vector>
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
#include <QWheelEvent>
#include <QColor>

class DisassemblyView : public QFrame
{
    Q_OBJECT

public:
    DisassemblyView();
    DisassemblyView(QWidget *parent);
    ~DisassemblyView();
    void update_display();
    void scroll(int steps);
    void scrollTo(int value);
    void set_emulator(et3400emu *emu);
    void set_range(offs_t start, offs_t end, uint8_t *memory);
    void set_maps(std::vector<Map> *maps);
    void set_current(offs_t address);
    void clear_current();
    void clear_selected();
    void load_breakpoints(std::vector<Breakpoint> *breakpoints);

signals:
    void on_scroll(int steps);
    void on_size(int max);
    void add_breakpoint(offs_t address);
    void remove_breakpoint(offs_t address);

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private:
    QScrollBar *scrollbar;
    QAction *action;
    bool running;
    offs_t start;
    offs_t end;
    int offset;
    int visible_items;
    int item_height = 20;
    int max_vscroll;
    et3400emu *emu_ptr;
    uint8_t *memory;
    QPixmap *buffer;
    QPixmap breakpoint_icon;
    QTimer *m_paintTimer;
    bool is_memory_set;
    DisassemblyLine find_line(offs_t address);
    void toggle_breakpoint(int line_number);
    void bufferDraw();
    std::vector<Map> *maps;
    std::vector<DisassemblyLine> *lines;
    int selected;
    int current;
};

#endif // DISASSEMBLYVIEW_H
