#ifndef DISASSEMBLYVIEW_H
#define DISASSEMBLYVIEW_H

#include "../common/common_defs.h"
#include "../emu/et3400.h"
#include "../dev/memory_map.h"
#include "../dasm/disassembler.h"
#include "../windows/add_label.h"
#include "../windows/remove_label.h"
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
#include <QMenu>
#include <QAction>
#include <QFont>
#include <QWheelEvent>
#include <QColor>


class DisassemblyView : public QFrame
{
	Q_OBJECT

public:
	int offset;

	DisassemblyView();
	DisassemblyView(QWidget* parent);
	~DisassemblyView();
	void scroll(int steps);
	void scrollTo(int value);
	void set_emulator(et3400emu* emu);
	void set_range(offs_t start, offs_t end, uint8_t* memory);
	void set_current(offs_t address);
	void clear_current();
	void clear_selected();
	void refresh();

signals:
	void on_scroll(int steps);
	void on_set_current(int offset);
	void on_size(int max);
	void add_breakpoint(offs_t address);
	void remove_breakpoint(offs_t address);
	void add_or_remove_breakpoint_signal(offs_t address);

public slots:
	void redraw();

protected:
	void paintEvent(QPaintEvent* event) override;
	void wheelEvent(QWheelEvent* event) override;
	void resizeEvent(QResizeEvent* event) override;
	void mousePressEvent(QMouseEvent* event) override;
	void keyPressEvent(QKeyEvent* event) override;

private:
	QScrollBar* scrollbar;
	QAction* action;
	QPixmap* buffer;
	QPixmap breakpoint_icon;
	QTimer* m_paintTimer;

	et3400emu* emu_ptr;
	uint8_t* memory;

	std::vector<DisassemblyLine>* lines;

	bool running;
	bool is_memory_set;
	offs_t start;
	offs_t end;
	int visible_items;
	int item_height = 20;
	int max_vscroll;
	int selected;
	int current;

	DisassemblyLine find_line(offs_t address);
	void add_or_remove_breakpoint(int line_number);
	void bufferDraw();
	void showContextMenu(const QPoint& pos);
	void adjust_selected();
	void add_label(DisassemblyLine* line);
	void remove_label(DisassemblyLine* line);
};

#endif // DISASSEMBLYVIEW_H
