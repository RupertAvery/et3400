#ifndef DISASSEMBLYVIEW_H
#define DISASSEMBLYVIEW_H

#include "../common/common_defs.h"
#include "../emu/et3400.h"
#include "../dev/memory_map.h"
#include "../dasm/disassembler.h"
#include "../windows/label.h"
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
	void setEmulator(et3400emu* emu);
	void setRange(offs_t start, offs_t end, uint8_t* memory);
	void setCurrent(offs_t address);
	void setSelected(offs_t address);
	void clearCurrent();
	void clearSelected();

	void refresh();
	void addLabel();
	void ensureVisible(offs_t address);

signals:
	void onScroll(int steps);
	void onOffsetUpdated(int offset);
	void onSize(int max);
	void onAddBreakpoint(offs_t address);
	void onRemoveBreakpoint(offs_t address);
	void onAddorRemoveBreakpoint(offs_t address);

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

	DisassemblyLine findLine(offs_t address);
	void addOrRemoveBreakpoint(int line_number);
	void bufferDraw();
	void showContextMenu(const QPoint& pos);
	void adjustSelected(int direction);

	void addLabel(DisassemblyLine* line);
	void editLabel(DisassemblyLine* line);
	void removeLabel(DisassemblyLine* line);
};

#endif // DISASSEMBLYVIEW_H
