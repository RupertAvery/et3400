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

    void add_breakpoint_from_table();
	void remove_breakpoint_from_table();
	void clear_breakpoints();
    void breakpoints_table_selection_changed();
    void breakpoint_item_changed(QTableWidgetItem *item);
    
    DebuggerDialog *debugger;

	QTableWidget *breakpoints_table = nullptr;
	QAction *tab_remove_breakpoint_action = nullptr;

};

#endif // BREAKPOINTS_H