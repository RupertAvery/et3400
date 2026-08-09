#ifndef LABELS_H
#define LABELS_H

#include <QDialog>
#include <QIcon>
#include <QVBoxLayout>
#include <QToolBar>
#include <QAction>
#include <QTableWidget>
class DebuggerDialog;

class LabelsDialog : public QDialog
{
    Q_OBJECT

public:
    LabelsDialog(DebuggerDialog *debugger);
    ~LabelsDialog();

    void populate_labels_table();
    void load_default_labels();

private:
    DebuggerDialog *debugger;
    QAction *edit_action;
    QAction *remove_action;
    QAction *tab_clear_ram_labels_action;
    QAction *tab_goto_label_action;
    QTableWidget *labels_table;

    void setupUi();
    void retranslateUi();

    void clear_labels();

	void add_label();
	void edit_label();
	void delete_label();
	void goto_label();
	void table_selection_changed();

    void load_labels();
    void save_labels();
};

#endif // LABELS_H