#ifndef DISASSEMBLYVIEW_H
#define DISASSEMBLYVIEW_H

#include "../util/common.h"
#include "../emu/et3400.h"
#include "../dev/memory_map.h"
#include "../cpu/defs.h"
#include "../dasm/disassembler.h"
//#include <thread>
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

signals:
    void on_scroll(int steps);
    void on_size(int max);

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

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
    QTimer *m_paintTimer;
    bool is_memory_set;
    void bufferDraw();
};

#endif // DISASSEMBLYVIEW_H
