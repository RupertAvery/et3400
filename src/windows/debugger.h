#ifndef DEBUGGER_H
#define DEBUGGER_H


#define MakeButton(button, toolTip, icon, seq, delegate) \
    button = new QToolButton(toolbar); \
    button->setToolTip(toolTip); \
    button->setIcon(QIcon(icon)); \
    button->setShortcut(QKeySequence(seq)); \
    connect(button, &QToolButton::clicked, this, &DebuggerDialog::delegate)

#define MakeTriggeredAction(action, name, seq, delegate) \
    action = new QAction(name, this); \
    action->setShortcut(QKeySequence(seq)); \
    connect(action, &QAction::triggered, this, &DebuggerDialog::delegate)


#define MakeToggledAction(action, name, seq, delegate) \
    action = new QAction(name, this); \
    action->setCheckable(true); \
    action->setChecked(true); \
    action->setShortcut(QKeySequence(seq)); \
    connect(action, &QAction::toggled, this, &DebuggerDialog::delegate)


//#include "memory_location.h"
#include "../emu/et3400.h"
#include "../widgets/memory_view.h"
#include "../widgets/disassembly_view.h"
#include "../widgets/status_view.h"
#include "../util/settings.h"
#include "file.h"

#include <QObject>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QDialog>
#include <QScrollBar>
#include <QWidget>
#include <QSlider>
#include <QGridLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QToolBar>
#include <QDebug>
#include <QComboBox>
#include <QKeyEvent>
#include <QMenu>
#include <QToolButton>
#include <QSizeGrip>

class DebuggerDialog : public QDialog
{
	Q_OBJECT

public:
	DebuggerDialog();
	DebuggerDialog(QWidget* parent);
	~DebuggerDialog();
	void set_emulator(et3400emu* emu);
	void set_settings(Settings* settings);
	void update_button_state();
	void refresh();
	void after_load_ram();

protected:
	void keyPressEvent(QKeyEvent* event) override;
	void keyReleaseEvent(QKeyEvent* event) override;

private:
	QToolButton* start_button;
	QToolButton* stop_button;
	QToolButton* step_button;
	QToolButton* reset_button;

	// QSlider* slider;
	QLabel* label;

	QComboBox* memory_selector;
	QComboBox* disassembly_selector;

	QToolButton* panel_selector;
	QAction* toggle_memory_action;
	QAction* toggle_disassembly_action;
	QAction* toggle_status_action;

	QToolButton* labels_selector; 
	QAction* add_label_action;
	QAction* goto_label_action;

	QScrollBar* memory_scrollbar;
	QGroupBox* memory_groupBox;
	QGroupBox* disassembly_groupBox;
	QScrollBar* disassembly_scrollbar;
	QGroupBox* status_groupBox;

	QAction* breakpoint_handler_action;

	MemoryView* memory_view;
	DisassemblyView* disassembly_view;
	StatusView* status_view;

	et3400emu* emu_ptr;
	Settings* settings;

	bool emu_set;

	void selectByAddress(offs_t address);

	void start(bool checked);
	void stop(bool checked);
	void step(bool checked);
	void reset(bool checked);

	void setupUI();
	void update_memory_scrollbar(int value);
	void setDisassemblyScrollbar(int value);
	void adjustDisassemblyScrollbar(int value);
	void update_memory_scrollbar_max(int value);
	void update_disassembly_scrollbar_max(int value);

	void memory_slider_moved(int value);
	void disassembly_slider_moved(int value);

	void select_memory_location(int index);
	void select_disassembly_location(int index);

	int count_open_panels();
	void toggle_memory_panel(bool checked);
	void toggle_disassembly_panel(bool checked);
	void toggle_status_panel(bool checked);

	void pauseAndUpdateDisassembler();
	void stepAndUpdateDisassembler();

	void add_breakpoint(offs_t address);
	void remove_breakpoint(offs_t address);
	void add_or_remove_breakpoint(offs_t address);

	void breakpoint_handler(bool checked);

	void add_label();
	void goto_label();

	void load_ram();
	void save_ram();
	void load_breakpoints();
	void save_breakpoints();
	void load_labels();
	void save_labels();
};

#endif // DEBUGGER_H