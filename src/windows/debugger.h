#ifndef DEBUGGER_H
#define DEBUGGER_H

//#include "memory_location.h"
#include "../emu/et3400.h"
#include "../widgets/memory_view.h"
#include "../widgets/disassembly_view.h"
#include "../widgets/status_view.h"

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

class DebuggerDialog : public QDialog
{
    Q_OBJECT

public:
    DebuggerDialog();
    DebuggerDialog(QWidget *parent);
    ~DebuggerDialog();
    void set_emulator(et3400emu *emu);
    void update_button_state();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    QSlider *slider;
    QLabel *label;
    QGroupBox *memory_groupBox;
    QGroupBox *disassembly_groupBox;
    QGroupBox *status_groupBox;
    QAction *breakpoint_handler_action;
    QScrollBar *memmory_scrollbar;
    QScrollBar *disassembly_scrollbar;
    QComboBox *memory_selector;
    QComboBox *disassembly_selector;
    QToolButton *panel_selector;
    QToolButton *start_button;
    QToolButton *stop_button;
    QToolButton *step_button;
    QToolButton *reset_button;
    QAction *toggle_memory_action;
    QAction *toggle_disassembly_action;
    QAction *toggle_status_action;

    MemoryView *memory_view;
    DisassemblyView *disassembly_view;
    StatusView *status_view;

    et3400emu *emu_ptr;
    bool emu_set;

    void start(bool checked);
    void stop(bool checked);
    void step(bool checked);
    void reset(bool checked);

    void setupUI();
    void update_memory_scrollbar(int value);
    void update_disassembly_scrollbar(int value);
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

    void breakpoint_handler(bool checked);
};

#endif // DEBUGGER_H