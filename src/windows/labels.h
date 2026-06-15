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
    QAction *tab_edit_label_action;
    QAction *tab_remove_label_action;
    QAction *tab_clear_ram_labels_action;
    QAction *tab_goto_label_action;
    QTableWidget *labels_table;

    void setupUi();
    void retranslateUi();

    void clear_labels();

	void add_label_from_table();
	void edit_label_from_table();
	void delete_label_from_table();
	void goto_label_from_table();
	void labels_table_selection_changed();

    void load_labels();
    void save_labels();
};

#endif // LABELS_H