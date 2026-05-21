#ifndef DEBUGGER_H
#define DEBUGGER_H

#define MakeToolButton(toolbar, button, toolTip, icon, seq, delegate) \
	button = new QToolButton();                                   \
	button->setToolTip(toolTip);                                  \
	button->setIcon(QIcon(icon));                                 \
	button->setShortcut(QKeySequence(seq));                       \
	connect(button, &QToolButton::clicked, this, &DebuggerDialog::delegate)

#define MakeTriggeredActionNS(action, name, delegate) \
	action = new QAction(name, this);                 \
	connect(action, &QAction::triggered, this, &DebuggerDialog::delegate)

#define MakeTriggeredAction(action, name, seq, delegate) \
	action = new QAction(name, this);                    \
	action->setShortcut(QKeySequence(seq));              \
	connect(action, &QAction::triggered, this, &DebuggerDialog::delegate)

#define MakeToggledAction(action, name, seq, delegate) \
	action = new QAction(name, this);                  \
	action->setCheckable(true);                        \
	action->setChecked(true);                          \
	action->setShortcut(QKeySequence(seq));            \
	connect(action, &QAction::toggled, this, &DebuggerDialog::delegate)

#define MakeToggledActionNS(action, name, delegate) \
	action = new QAction(name, this);               \
	action->setCheckable(true);                     \
	action->setChecked(true);                       \
	connect(action, &QAction::toggled, this, &DebuggerDialog::delegate)

// #include "memory_location.h"
#include "../emu/et3400.h"
#include "../widgets/memory_view.h"
#include "../widgets/disassembly_view.h"
#include "../widgets/status_view.h"
#include "../util/settings.h"
#include "file.h"
#include "save.h"
#include "clear_ram.h"

class MainWindow;

#include <QObject>
#include <QVariant>
#include <QString>
#include <QVector>
#include <QDialog>
#include <QScrollBar>
#include <QWidget>
#include <QSlider>
#include <QGridLayout>
#include <QHBoxLayout>
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
#include <QSplitter>
#include <QTabWidget>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>
#include <QInputDialog>

class DebuggerDialog : public QDialog
{
	Q_OBJECT

public:
	DebuggerDialog();
	DebuggerDialog(QWidget *parent);
	~DebuggerDialog();
	void set_emulator(et3400emu *emu);
	void set_settings(Settings *settings);
	void set_parent_window(MainWindow *parent);
	void update_button_state();
	void refresh();
	void after_load_ram();
	void after_load_rom();

protected:
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;
	void resizeEvent(QResizeEvent *event) override;

private:
	QToolButton *start_button;
	QToolButton *stop_button;
	QToolButton *step_into_button;
	QToolButton *step_over_button;
	QToolButton *step_out_button;
	QToolButton *reset_button;

	// QSlider* slider;
	QLabel *label;

	QComboBox *memory_selector;
	QComboBox *disassembly_selector;

	QToolButton *view_button;
	QAction *toggle_memory_action;
	QAction *toggle_disassembly_action;
	QAction *toggle_status_action;
	QAction *toggle_heat_map_action;
	QAction *refresh_disassembly_action;
	QAction *toggle_autorefresh_disassembly_action;
	QAction *toggle_load_default_labels_action = nullptr;
	QAction *clear_ram_action;

	QAction *breakpoint_handler_action;

	QAction *debug_run_action = nullptr;
	QAction *debug_stop_action = nullptr;
	QAction *debug_step_over_action = nullptr;
	QAction *debug_step_into_action = nullptr;
	QAction *debug_step_out_action = nullptr;
	QAction *debug_reset_action = nullptr;

	MemoryView *memory_view = nullptr;
	DisassemblyView *disassembly_view = nullptr;
	StatusView *status_view = nullptr;

	et3400emu *emu_ptr = nullptr;
	Settings *settings = nullptr;
	ClearRamSettings clearRamSettings{0x0000, 0x01FF, 0x00};

	MainWindow *parent_window = nullptr;

	bool emu_set;

	void selectByAddress(offs_t address);

	void start(bool checked);
	void stop(bool checked);
	void step_into(bool checked);
	void step_over(bool checked);
	void step_out(bool checked);
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

	void clear_ram();
	void diassembly_refresh();

	int count_open_panels();
	void toggle_memory_panel(bool checked);
	void toggle_disassembly_panel(bool checked);
	void toggle_status_panel(bool checked);
	void toggle_heat_map(bool checked);
	void toggle_auto_refresh_disassembly_panel(bool checked);
	void toggle_load_default_labels(bool checked);
	void load_default_labels();

	void pauseAndUpdateDisassembler();
	void stepAndUpdateDisassembler();

	void add_breakpoint(offs_t address);
	void remove_breakpoint(offs_t address);
	void add_or_remove_breakpoint(offs_t address);

	void breakpoint_handler(bool checked);

	void clear_labels();
	void goto_label();
	void add_label_from_table();
	void edit_label_from_table();
	void delete_label_from_table();
	void goto_label_from_table();
	void labels_table_selection_changed();
	void populate_labels_table();

	void add_breakpoint_from_table();
	void remove_breakpoint_from_table();
	void clear_breakpoints();
	void breakpoints_table_selection_changed();
	void breakpoint_item_changed(QTableWidgetItem *item);
	void populate_breakpoints_table();
	void update_clear_ram_labels_state();

	void load_rom();
	void load_ram();
	void save_ram();
	void load_breakpoints();
	void save_breakpoints();
	void load_labels();
	void save_labels();
	void save_ram_labels();

	void reset_disassembly_view();

	QGroupBox *memory_groupBox = nullptr;
	QGroupBox *disassembly_groupBox = nullptr;
	QGroupBox *status_groupBox = nullptr;

	QTableWidget *labels_table = nullptr;
	QAction *tab_edit_label_action = nullptr;
	QAction *tab_remove_label_action = nullptr;
	QAction *tab_goto_label_action = nullptr;
	QAction *tab_clear_ram_labels_action = nullptr;

	QTableWidget *breakpoints_table = nullptr;
	QAction *tab_remove_breakpoint_action = nullptr;

	QScrollBar *memory_scrollbar = nullptr;
	QScrollBar *disassembly_scrollbar = nullptr;

	QToolBar *create_menu_toolbar();
	QToolBar *create_shortcuts_toolbar();

	QToolButton *create_file_menu(QToolBar *toolbar);
	QToolButton *create_debug_menu(QToolBar *toolbar);
	QToolButton *create_view_menu(QToolBar *toolbar);
	QToolButton *create_settings_menu(QToolBar *toolbar);

	QGroupBox *create_status_group();
	QGroupBox *create_disassembly_group();
	QGroupBox *create_memory_group();
	QWidget *create_tab_panel();

	QWidget *create_labels_tab();
	QWidget *create_breakpoints_tab();
};

#endif // DEBUGGER_H