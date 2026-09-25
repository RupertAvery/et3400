#ifndef DEVICES_H
#define DEVICES_H

#include <QDialog>
#include <QIcon>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QTableWidget>
#include "device.h"

class DebuggerDialog;

class DevicesDialog : public QDialog
{
    Q_OBJECT

public:
    DevicesDialog(DebuggerDialog *debugger);
    ~DevicesDialog();

    void populate_devices_table();

private:

    void setupUi();
    void retranslateUi();

    void show_add_device();
    void show_edit_device();
    void show_remove_device();
    void table_selection_changed();
    // void breakpoint_item_changed(QTableWidgetItem *item);
    
    DebuggerDialog *debugger;

	QTableWidget *devices_table = nullptr;
	QAction *edit_action = nullptr;
	QAction *remove_action = nullptr;

};

#endif // BREAKPOINTS_H