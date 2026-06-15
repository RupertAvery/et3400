#ifndef DISASSEMBLYVIEW_H
#define DISASSEMBLYVIEW_H

#include "../common/common_defs.h"
#include "../emu/et3400.h"
#include "../dev/memory_map.h"
#include "../dasm/disassembler.h"
#include "../windows/label.h"
#include "../windows/remove_label.h"
// #include <thread>
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
	DisassemblyView(QWidget *parent);
	~DisassemblyView();
	void scroll(int steps);
	void scrollTo(int value);
	void scrollIntoView();
	void setEmulator(et3400emu *emu);
	void set_range(offs_t start, offs_t end, uint8_t *memory);
	void setCurrent(offs_t address);
	void setSelected(offs_t address);
	int getSelectedAddress(); 

	void clearCurrent();
	void clearSelected();

	void refresh();
	void rebuild();
	void clearLabels();
	void addLabel();
	void ensureVisible(offs_t address);
	void setAutoRefresh(bool value);

signals:
	void onScroll(int steps);
	void onOffsetUpdated(int offset);
	void onSize(int max);
	void onAddBreakpoint(offs_t address);
	void onRemoveBreakpoint(offs_t address);
	void onAddorRemoveBreakpoint(offs_t address);
	void onBreakpointChanged();
	void onAutorefreshChanged(bool value);
	void onShowInMemory(offs_t address);

public slots:
	void redraw();

protected:
	void paintEvent(QPaintEvent *event) override;
	bool eventFilter(QObject *obj, QEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void leaveEvent(QEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void focusInEvent(QFocusEvent *event) override;
	void focusOutEvent(QFocusEvent *event) override;

private:
	QScrollBar *scrollbar = nullptr;
	QFrame *frame;

	QAction *action;
	QPixmap *buffer;
	QPixmap breakpoint_enabled_icon;
	QPixmap breakpoint_disabled_icon;
	QPixmap breakpoint_available_icon;
	QTimer *m_paintTimer;

	et3400emu *emu_ptr = nullptr;
	uint8_t *memory;

	std::vector<DisassemblyLine> *lines;

	bool running;
	bool is_memory_set;
	offs_t start;
	offs_t end;
	int visible_items;
	int item_height = 20;
	int max_vscroll;
	int selected_line;
	int last_selected_line = -1;

	int current;
	int hover_row = -1;
	bool auto_refresh = false;
	bool gained_focus = false;

	DisassemblyLine findLine(offs_t address);
	void addOrRemoveBreakpoint(int line_number);
	void bufferDraw();
	void showContextMenu(const QPoint &pos);
	void adjustSelected(int direction);

	void addLabel(DisassemblyLine *line);
	void editLabel(DisassemblyLine *line);
	void removeLabel(DisassemblyLine *line);

	void setupUI(QWidget *parent)
	{
		this->setFocusPolicy(Qt::StrongFocus);

		QHBoxLayout *layout = new QHBoxLayout(parent);

		frame = new QFrame(parent);
		frame->setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
		frame->setLineWidth(3);
		frame->setMouseTracking(true);
		frame->setContextMenuPolicy(Qt::CustomContextMenu);
		frame->installEventFilter(this);

		scrollbar = new QScrollBar(Qt::Orientation::Vertical);

		layout->addWidget(frame);
		layout->addWidget(scrollbar);
		layout->setMargin(0);

		this->setLayout(layout);

		connect(scrollbar, &QScrollBar::sliderMoved, this, &DisassemblyView::scrollTo);
		connect(scrollbar, &QScrollBar::valueChanged, this, &DisassemblyView::scrollTo);
		connect(frame, &QFrame::customContextMenuRequested, this, &DisassemblyView::showContextMenu);
	}
};

#endif // DISASSEMBLYVIEW_H
