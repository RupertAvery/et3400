#ifndef BREAKPOINTS_H
#define BREAKPOINTS_H

#include <QDialog>
#include <QIcon>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QTableWidget>
class DebuggerDialog;

class BreakpointsDialog : public QDialog
{
    Q_OBJECT

public:
    BreakpointsDialog(DebuggerDialog *debugger);
    ~BreakpointsDialog();

    void populate_breakpoints_table();

private:
    void setupUi();
    void retranslateUi();

    void load_breakpoints();
    void save_breakpoints();

    void add_breakpoint();
    void edit_breakpoint();
	void remove_breakpoint();
	void clear_breakpoints();
    void table_selection_changed();
    void breakpoint_item_changed(QTableWidgetItem *item);
    
    DebuggerDialog *debugger;

	QTableWidget *breakpoints_table = nullptr;
	QAction *edit_action = nullptr;
	QAction *remove_action = nullptr;

};

#endif // BREAKPOINTS_H