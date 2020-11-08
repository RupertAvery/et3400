#ifndef DEBUGGER_H
#define DEBUGGER_H

//#include "memory_location.h"
#include "../emu/et3400.h"
#include "../widgets/memory_view.h"
#include "../widgets/disassembly_view.h"
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

class DebuggerDialog : public QDialog
{
    Q_OBJECT

public:
    DebuggerDialog();
    DebuggerDialog(QWidget *parent);
    ~DebuggerDialog();
    void set_emulator(et3400emu *emu);
    void select_memory_location(int index);
    //signals:

    // public slots:
    //     void setClockRate(int value);
protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent *event) override;

private:
    QSlider *slider;
    QLabel *label;
    QScrollBar *memmory_scrollbar;
    QScrollBar *disassembly_scrollbar;
    QComboBox *memory_selector;
    MemoryView *memory_view;
    DisassemblyView *disassembler_view;
    et3400emu *emu_ptr;

    void update_memory_scrollbar(int value);
    void update_disassembly_scrollbar(int value);
    void update_memory_scrollbar_max(int value);

    void memory_slider_moved(int value);
    void disassembly_slider_moved(int value);
};

#endif // DEBUGGER_H