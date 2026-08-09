#include "breakpoints.h"
#include "breakpoint.h"
#include "debugger.h"
#include "file.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include "../common/util.h"

BreakpointsDialog::BreakpointsDialog(DebuggerDialog *debugger) : QDialog(debugger, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint)
{
	this->debugger = debugger;
	setupUi();
}

BreakpointsDialog::~BreakpointsDialog() {}

void BreakpointsDialog::setupUi()
{
	this->setWindowTitle("Breakpoints");
	this->resize(400, 300);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);

	QToolBar *toolbar = new QToolBar(this);
	toolbar->setMovable(false);

	QAction *add_action = toolbar->addAction(QIcon(":/buttons/Add.png"), "Add");
	edit_action = toolbar->addAction(QIcon(":/buttons/Edit.png"), "Edit");
	edit_action->setShortcut(Qt::Key_F2);
	edit_action->setEnabled(false);
	remove_action = toolbar->addAction(QIcon(":/buttons/Remove.png"), "Remove");
	remove_action->setShortcut(Qt::Key_Delete);
	remove_action->setEnabled(false);

	toolbar->addSeparator();

	QAction *load_breakpoints_action = toolbar->addAction(QIcon(":/buttons/OpenFile.png"), "Load Breakpoints");
	QAction *save_breakpoints_action = toolbar->addAction(QIcon(":/buttons/Save.png"), "Save Breakpoints");
	toolbar->addSeparator();
	QAction *clear_breakpoints_action = toolbar->addAction(QIcon(":/buttons/Trash.png"), "Clear Breakpoints");

	breakpoints_table = new QTableWidget(this);
	breakpoints_table->setColumnCount(3);
	breakpoints_table->setHorizontalHeaderLabels({"", "Address", "Type"});
	breakpoints_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	breakpoints_table->setSelectionMode(QAbstractItemView::SingleSelection);
	breakpoints_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	breakpoints_table->horizontalHeader()->setStretchLastSection(true);
	breakpoints_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
	breakpoints_table->setColumnWidth(0, 24);
	breakpoints_table->verticalHeader()->setVisible(false);

	layout->addWidget(toolbar);
	layout->addWidget(breakpoints_table);
	this->setLayout(layout);

	connect(add_action, &QAction::triggered, this, &BreakpointsDialog::add_breakpoint);
	connect(edit_action, &QAction::triggered, this, &BreakpointsDialog::edit_breakpoint);
	connect(remove_action, &QAction::triggered, this, &BreakpointsDialog::remove_breakpoint);
	connect(load_breakpoints_action, &QAction::triggered, this, &BreakpointsDialog::load_breakpoints);
	connect(save_breakpoints_action, &QAction::triggered, this, &BreakpointsDialog::save_breakpoints);
	connect(clear_breakpoints_action, &QAction::triggered, this, &BreakpointsDialog::clear_breakpoints);
	connect(breakpoints_table, &QTableWidget::itemSelectionChanged, this, &BreakpointsDialog::table_selection_changed);
	connect(breakpoints_table, &QTableWidget::itemChanged, this, &BreakpointsDialog::breakpoint_item_changed);
	connect(breakpoints_table, &QTableWidget::cellDoubleClicked, this, [this](int, int)
			{ edit_breakpoint(); });

	breakpoints_table->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(breakpoints_table, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos)
			{
        if (breakpoints_table->selectedItems().isEmpty())
            return;
        QMenu menu(this);
        menu.addAction(edit_action);
        menu.addAction(remove_action);
        menu.exec(breakpoints_table->viewport()->mapToGlobal(pos)); });
}

void BreakpointsDialog::retranslateUi()
{
}

void BreakpointsDialog::remove_breakpoint()
{
	int row = breakpoints_table->currentRow();
	if (row < 0)
		return;

	offs_t address = (offs_t)breakpoints_table->item(row, 0)->data(Qt::UserRole).toUInt();

	debugger->emu_ptr->remove_breakpoint(address);
	populate_breakpoints_table();
	debugger->disassembly_view->rebuild();

	bool has_selection = !breakpoints_table->selectedItems().isEmpty();
	edit_action->setEnabled(has_selection);
	remove_action->setEnabled(has_selection);
}

void BreakpointsDialog::table_selection_changed()
{
	bool has_selection = !breakpoints_table->selectedItems().isEmpty();
	edit_action->setEnabled(has_selection);
	remove_action->setEnabled(has_selection);
}

void BreakpointsDialog::breakpoint_item_changed(QTableWidgetItem *item)
{
	if (item->column() != 0)
		return;
	offs_t address = (offs_t)item->data(Qt::UserRole).toUInt();
	debugger->set_breakpoint_enabled(address, item->checkState() == Qt::Checked);
}

void BreakpointsDialog::add_breakpoint()
{
	BreakpointDialog breakpointDialog;
	// breakpointDialog.hasCollision = [this](Label *label, offs_t start, offs_t end)
	// { return debugger->emu_ptr->labels->hasCollision(label, start, end); };
	breakpointDialog.addBreakpoint(0);

	if (breakpointDialog.exec() == QDialog::Accepted)
	{
		BreakpointInfo info = breakpointDialog.getBreakpoint();
		debugger->emu_ptr->add_breakpoint(info.address);
	}
	populate_breakpoints_table();
}

void BreakpointsDialog::edit_breakpoint()
{
	int row = breakpoints_table->currentRow();
	if (row < 0)
		return;

	int idx = breakpoints_table->item(row, 0)->data(Qt::UserRole).toInt();
	auto breakpoints = debugger->emu_ptr->breakpoints->getBreakpoints();
	if (idx >= (int)breakpoints.size())
		return;

	Breakpoint breakpoint = breakpoints.at(idx);

	BreakpointDialog breakpointDialog;
	// breakpointDialog.hasCollision = [this](Label *label, offs_t start, offs_t end)
	// { return debugger->emu_ptr->labels->hasCollision(label, start, end); };
	breakpointDialog.editBreakpoint(&breakpoint);

	if (breakpointDialog.exec() == QDialog::Accepted)
	{
		BreakpointInfo info = breakpointDialog.getBreakpoint();
		debugger->emu_ptr->add_breakpoint(info.address);
	}
	populate_breakpoints_table();
}

void BreakpointsDialog::clear_breakpoints()
{
	auto reply = QMessageBox::question(this, "Clear Breakpoints", "Clear all breakpoints?");
	if (reply != QMessageBox::Yes)
		return;
	debugger->emu_ptr->breakpoints->clearBreakpoints();
	populate_breakpoints_table();
	debugger->disassembly_view->rebuild();
}

void BreakpointsDialog::populate_breakpoints_table()
{
	if (!breakpoints_table)
		return;

	breakpoints_table->blockSignals(true);
	breakpoints_table->setRowCount(0);

	auto bps = debugger->emu_ptr->breakpoints->getBreakpoints();
	for (const auto &bp : bps)
	{
		int row = breakpoints_table->rowCount();
		breakpoints_table->insertRow(row);

		auto *check_item = new QTableWidgetItem();
		check_item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);
		check_item->setCheckState(bp.is_enabled ? Qt::Checked : Qt::Unchecked);
		check_item->setData(Qt::UserRole, bp.address);
		breakpoints_table->setItem(row, 0, check_item);

		breakpoints_table->setItem(row, 1, new QTableWidgetItem(toHex(bp.address)));

		QString type_str;
		switch (bp.type)
		{
		case 0:
			type_str = "Execute";
			break;
		case 1:
			type_str = "Read";
			break;
		case 2:
			type_str = "Write";
			break;
		default:
			type_str = QString::number(bp.type);
			break;
		}
		breakpoints_table->setItem(row, 2, new QTableWidgetItem(type_str));
	}

	breakpoints_table->blockSignals(false);
}

void BreakpointsDialog::load_breakpoints()
{
	File::load_breakpoint_dialog(this, debugger->emu_ptr, debugger->get_settings()->breakpointsDir);
	populate_breakpoints_table();
	debugger->disassembly_view->rebuild();
}

void BreakpointsDialog::save_breakpoints()
{
	File::save_breakpoint_dialog(this, debugger->emu_ptr, debugger->get_settings()->breakpointsDir);
}