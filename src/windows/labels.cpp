#include "labels.h"
#include "debugger.h"
#include "label.h"
#include "file.h"
#include <QMessageBox>
#include <QMenu>
#include "../common/util.h"

LabelsDialog::LabelsDialog(DebuggerDialog *debugger) : QDialog(debugger, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint)
{
    this->debugger = debugger;
    setupUi();
}

LabelsDialog::~LabelsDialog() {}

void LabelsDialog::setupUi()
{
    this->setWindowTitle("Labels");
    this->resize(600, 400);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setSpacing(0);
    layout->setMargin(0);

    QToolBar *toolbar = new QToolBar(this);
    toolbar->setMovable(false);

    QAction *add_action = toolbar->addAction(QIcon(":/buttons/Add.png"), "Add");
    tab_edit_label_action = toolbar->addAction(QIcon(":/buttons/Edit.png"), "Edit");
    tab_remove_label_action = toolbar->addAction(QIcon(":/buttons/Remove.png"), "Remove");
    tab_edit_label_action->setEnabled(false);
    tab_remove_label_action->setEnabled(false);

    toolbar->addSeparator();

    QAction *load_labels_action = toolbar->addAction(QIcon(":/buttons/OpenFile.png"), "Load Labels");
    QAction *save_labels_action = toolbar->addAction(QIcon(":/buttons/Save.png"), "Save Labels");
    toolbar->addSeparator();
    QAction *load_default_labels_action = toolbar->addAction(QIcon(":/buttons/Restart.png"), "Load Defaults");
    toolbar->addSeparator();
    tab_clear_ram_labels_action = toolbar->addAction(QIcon(":/buttons/Trash.png"), "Clear Labels");

    toolbar->addSeparator();
    tab_goto_label_action = toolbar->addAction(QIcon(":/buttons/GotoRow.png"), "Goto");

    labels_table = new QTableWidget(this);
    labels_table->setColumnCount(4);
    labels_table->setHorizontalHeaderLabels({"Start", "End", "Type", "Text"});
    labels_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    labels_table->setSelectionMode(QAbstractItemView::SingleSelection);
    labels_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    labels_table->horizontalHeader()->setStretchLastSection(true);
    labels_table->verticalHeader()->setVisible(false);

    layout->addWidget(toolbar);
    layout->addWidget(labels_table);
    this->setLayout(layout);

    connect(add_action, &QAction::triggered, this, &LabelsDialog::add_label_from_table);
    connect(tab_edit_label_action, &QAction::triggered, this, &LabelsDialog::edit_label_from_table);
    connect(tab_remove_label_action, &QAction::triggered, this, &LabelsDialog::delete_label_from_table);
    connect(tab_goto_label_action, &QAction::triggered, this, [this]()
            { this->debugger->goto_label(); });
    connect(load_labels_action, &QAction::triggered, this, [this]()
            { this->debugger->load_labels(); });
    connect(save_labels_action, &QAction::triggered, this, [this]()
            { this->debugger->save_labels(); });
    connect(load_default_labels_action, &QAction::triggered, this, [this]()
            { this->debugger->load_default_labels(); });
    connect(tab_clear_ram_labels_action, &QAction::triggered, this, &LabelsDialog::clear_labels);
    connect(labels_table, &QTableWidget::itemSelectionChanged, this, &LabelsDialog::labels_table_selection_changed);
    connect(labels_table, &QTableWidget::cellDoubleClicked, this, &LabelsDialog::goto_label_from_table);

    labels_table->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(labels_table, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos)
            {
        if (labels_table->selectedItems().isEmpty())
            return;
        QMenu menu(this);
        menu.addAction(tab_edit_label_action);
        menu.addAction(tab_remove_label_action);
        menu.exec(labels_table->viewport()->mapToGlobal(pos)); });
}

void LabelsDialog::retranslateUi()
{
}

void LabelsDialog::populate_labels_table()
{
    if (!labels_table || !debugger->emu_ptr)
        return;

    memory_mapped_device *device = debugger->get_disassembly_device();

    labels_table->setRowCount(0);

    auto *all_labels = debugger->emu_ptr->labels->getLabels();

    for (int i = 0; i < (int)all_labels->size(); i++)
    {
        const Label &label = all_labels->at(i);
        if (device && (label.start < (uint32_t)device->get_start() || label.start > (uint32_t)device->get_end()))
            continue;
        int row = labels_table->rowCount();
        labels_table->insertRow(row);
        auto *start_item = new QTableWidgetItem(toHex(label.start));
        start_item->setData(Qt::UserRole, i);
        labels_table->setItem(row, 0, start_item);
        labels_table->setItem(row, 1, new QTableWidgetItem(toHex(label.end)));
        QString type_str;
        switch (label.type)
        {
        case LabelType::COMMENT:
            type_str = "Comment";
            break;
        case LabelType::DATA:
            type_str = "Data";
            break;
        case LabelType::ASSEMBLY:
            type_str = "Assembly";
            break;
        }
        labels_table->setItem(row, 2, new QTableWidgetItem(type_str));
        labels_table->setItem(row, 3, new QTableWidgetItem(label.comment));
    }
}

void LabelsDialog::labels_table_selection_changed()
{
    bool has_selection = !labels_table->selectedItems().isEmpty();
    tab_edit_label_action->setEnabled(has_selection);
    tab_remove_label_action->setEnabled(has_selection);
}

void LabelsDialog::goto_label_from_table()
{
    int row = labels_table->currentRow();
    if (row < 0)
        return;

    int idx = labels_table->item(row, 0)->data(Qt::UserRole).toInt();

    auto *labels = debugger->emu_ptr->labels->getLabels();

    if (idx >= (int)labels->size())
        return;

    offs_t address = labels->at(idx).start;

    debugger->goto_address(address);
}

void LabelsDialog::add_label_from_table()
{
    LabelDialog labelDialog;
    labelDialog.setLabel(LabelInfo{"", LabelType::COMMENT, 0, 0}, LabelDialogMode::Add);

    if (labelDialog.exec() == QDialog::Accepted)
    {
        LabelInfo info = labelDialog.getLabel();
        debugger->emu_ptr->labels->addLabel(Label{info.start, info.end, info.type, info.text});
        populate_labels_table();
        debugger->reset_disassembly_view();
    }
}

void LabelsDialog::edit_label_from_table()
{
    int row = labels_table->currentRow();
    if (row < 0)
        return;

    int idx = labels_table->item(row, 0)->data(Qt::UserRole).toInt();
    auto *labels = debugger->emu_ptr->labels->getLabels();
    if (idx >= (int)labels->size())
        return;

    Label &label = labels->at(idx);
    LabelDialog labelDialog;
    labelDialog.setLabel(LabelInfo{label.comment, label.type, label.start, label.end}, LabelDialogMode::Edit);

    if (labelDialog.exec() == QDialog::Accepted)
    {
        LabelInfo info = labelDialog.getLabel();
        label.comment = info.text;
        label.type = info.type;
        label.start = info.start;
        label.end = info.end;
        populate_labels_table();
        debugger->reset_disassembly_view();
    }
}

void LabelsDialog::delete_label_from_table()
{
    int row = labels_table->currentRow();
    if (row < 0)
        return;

    int idx = labels_table->item(row, 0)->data(Qt::UserRole).toInt();
    auto *labels = debugger->emu_ptr->labels->getLabels();
    if (idx >= (int)labels->size())
        return;

    debugger->emu_ptr->labels->removeLabel(&labels->at(idx));
    populate_labels_table();
    debugger->reset_disassembly_view();
}


void LabelsDialog::load_default_labels()
{
	memory_mapped_device *device = debugger->get_disassembly_device();

    if (!device)
		return;

	QString name = QString::fromStdString(device->name).toLower();
	QString mapPath;
	if (name.contains("ram"))
		mapPath = ":/ram/default.map";
	else if (name.contains("monitor"))
		mapPath = ":/rom/monitor.map";
	else
		return;

	auto reply = QMessageBox::question(this, "Load Default Labels",
									   QString("Clear existing labels and load defaults for %1?").arg(QString::fromStdString(device->name)));
	if (reply != QMessageBox::Yes)
		return;

	debugger->emu_ptr->labels->clearLabels(device->get_start(), device->get_end());

    bool success;
	File::load_labels(mapPath, debugger->emu_ptr, success);
	debugger->reset_disassembly_view();
	populate_labels_table();
}


void LabelsDialog::clear_labels()
{
	memory_mapped_device *device = debugger->get_disassembly_device();

    if (!device)
		return;

	auto reply = QMessageBox::question(this, "Clear Labels",
									   QString("Clear all labels for %1?").arg(QString::fromStdString(device->name)));
	if (reply != QMessageBox::Yes)
		return;

	debugger->emu_ptr->labels->clearLabels(device->get_start(), device->get_end());
	debugger->reset_disassembly_view();
	populate_labels_table();
}

void LabelsDialog::load_labels()
{
    File::load_labels_dialog(this, debugger->emu_ptr, debugger->get_settings()->labelsDir);
    debugger->reset_disassembly_view();
    populate_labels_table();
}
