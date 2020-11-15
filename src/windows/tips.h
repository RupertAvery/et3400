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

    static const int TIPCOUNT = 16;

    QString tips[TIPCOUNT] = {
        // 0
        QString("<p>You can label addresses as <b>Data</b> to prevent the disassembler from trying to interpret them as instructions.</p><p>In the Disassembly View, select an address, right-click and select <b>Add Label</b>, then choose <b>Data</b> and set the <b>Text</b> and <b>Start</b> and <b>End</b> address values.</p>"),
        QString("Clicking in the space on left of an address in the Disassembly View will add or remove a breakpoint"),
        QString("<p>In the debugger, You can press the following keys to control emulation:</p><ul><li><b>F4</b> to pause emulation</li><li><b>F10</b> to step into the next instruction while emulation is paused</li><li><b>F5</b> to resume emulation</li></ul>"),
        QString("<p>In the Disassembly View, You can use the following keys to navigate:</p><ul><li><b>Up/Down</b> to move one instruction</li><li><b>Page Up/Page Down</b> to skip several instructions</li><ul>"),
        QString("Instead of clicking on the keypad button in the window, you can press the corresponding keys <b>(0-9, A-F)</b> on your keyboard to interact with the emulator. Pressing <b>ESC</b> will reset the emulator."),
        // 5
        QString("You can press <b>F9</b> while an address is selected in the Disassembly View to add or remove a breakpoint."),
        QString("Press <b>CTRL-M</b> in the Debugger to hide or show the Memory View."),
        QString("<p>You can label addresses with a <b>Comment</b> to make the assembly easier to read.</p><p>In the Disassembly View, select an address, right-click and select <b>Add Label</b>, then choose <b>Comment</b> and set the <b>Text</b> and the <b>Address</b> values.</p>"),
        QString("When the emulator reaches a breakpoint, emulation will pause and you can view registers, memory and step through the assembly."),
        QString("You can adjust the clock speed under <b>ET-3400 Emulator > Config > Settings</b>."),
        // 10
        QString("You can load or save breakpoints under <b>Debugger > File</b>."),
        QString("You can load or save labels under <b>Debugger > File</b>."),
        QString("You can choose to disassemble <b>RAM</b> ($0000) or <b>ROM</b> ($FCOO) using the dropdown in the Disassembly View."),
        QString("You can choose to view the memory contents of <b>RAM</b>, <b>Keypad</b>, <b>Display</b> or <b>ROM</b> using the dropdown in the Memory View."),
        QString("You can resize the Debugger by dragging the bottom-right corner of the window."),
        // 15
        QString("<p>To remove a Comment label, right-click on the label, or on the instruction under the label, and select <b>Remove Label</b></p><p>To remove a Data label, right-click on the label, or on the range included in the label, and select <b>Remove Label</b></p>"),
        };
};

#endif // TIPS_H
