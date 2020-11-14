#ifndef TIPS_H
#define TIPS_H

#include "stdlib.h"
#include "../util/settings.h"

#include <QDialog>
#include <QVBoxLayout>
#include <QPushButton>
#include <QAction>
#include <QLabel>
#include <QSize>
#include <QSpacerItem>
#include <QCheckBox>

class Tips : public QDialog
{
    Q_OBJECT

public:
    Tips(QWidget *parent);
    void set_settings(Settings *settings);

private:
    int tip;
    Settings *settings;
    QCheckBox *show_tip_checkbox;

    QString tips[15] = {
        // 0
        QString("You can label addresses as Data to prevent the disassembler from trying to interpret them as instructions.\r\n\r\nSelect an address, right-click and select Add Label, then choose Data, set the Text and set the Start and End address"),
        QString("Clicking in the space on left of an address in the Disassembly View will add or remove a breakpoint"),
        QString("You can press F5 to resume emulation"),
        QString("You can press F4 to pause emulation"),
        QString("You can press F10 to step into the next instruction while emulation is paused"),
        // 5
        QString("You can press F9 while an address is selected in the Disassembly View to add or remove a breakpoint"),
        QString("Press CTRL-M to hide or show the Memory View"),
        QString("You can label addresses with a comment to make the assembly easier to read.\r\n\r\nSelect an address, right-click and select Add Label, then choose Comment and set the Text and set the Address"),
        QString("When the emulator reaches a breakpoint, emulation will pause and you can view registers, memory and step through the assembly."),
        QString("You can adjust the clock speed under ET-3400 Emulator > Comfig > Settings"),
        // 10
        QString("You can load or save breakpoints under Debugger > File"),
        QString("You can load or save labels under Debugger > File"),
        QString("You can choose to disassemble RAM ($0000) or ROM ($FCOO) by selecting using the dropdown in the Disassembly View"),
        QString("You can choose to view the memory contents of RAM, Keypad, Display or ROM by selecting using the dropdown in the Memory View"),
        QString("You can resize the Debugger by dragging the bottom-right corner of the window")};
};

#endif // TIPS_H
