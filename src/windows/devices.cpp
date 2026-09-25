#include "devices.h"
#include "debugger.h"
#include "file.h"
#include <QMessageBox>
#include <QInputDialog>
#include <QMenu>
#include "../common/util.h"
#include "../util/log.h"

DevicesDialog::DevicesDialog(DebuggerDialog *debugger) : QDialog(debugger, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint)
{
	this->debugger = debugger;
	setupUi();
}

DevicesDialog::~DevicesDialog() {}

void DevicesDialog::setupUi()
{
	this->setWindowTitle("Custom Devices");
	this->resize(500, 300);
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setSpacing(0);
	layout->setMargin(0);

	QToolBar *toolbar = new QToolBar(this);
	toolbar->setMovable(false);

	QAction *add_action = toolbar->addAction(QIcon(":/buttons/Add.png"), "Add");
	edit_action = toolbar->addAction(QIcon(":/buttons/Edit.png"), "Edit");
	edit_action->setEnabled(false);
	remove_action = toolbar->addAction(QIcon(":/buttons/Remove.png"), "Remove");
	remove_action->setEnabled(false);

	toolbar->addSeparator();

	// QAction *load_breakpoints_action = toolbar->addAction(QIcon(":/buttons/OpenFile.png"), "Load Breakpoints");
	// QAction *save_breakpoints_action = toolbar->addAction(QIcon(":/buttons/Save.png"), "Save Breakpoints");
	// toolbar->addSeparator();
	// QAction *clear_breakpoints_action = toolbar->addAction(QIcon(":/buttons/Trash.png"), "Clear Breakpoints");

	devices_table = new QTableWidget(this);
	devices_table->setColumnCount(4);
	devices_table->setHorizontalHeaderLabels({"Start", "End", "Name", "Address Pattern"});
	devices_table->setSelectionBehavior(QAbstractItemView::SelectRows);
	devices_table->setSelectionMode(QAbstractItemView::SingleSelection);
	devices_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
	devices_table->horizontalHeader()->setStretchLastSection(true);
	devices_table->setColumnWidth(3, 120);
	devices_table->verticalHeader()->setVisible(false);

	layout->addWidget(toolbar);
	layout->addWidget(devices_table);
	this->setLayout(layout);

	connect(add_action, &QAction::triggered, this, &DevicesDialog::show_add_device);
	connect(edit_action, &QAction::triggered, this, &DevicesDialog::show_edit_device);
	connect(remove_action, &QAction::triggered, this, &DevicesDialog::show_remove_device);

	connect(devices_table, &QTableWidget::itemSelectionChanged, this, &DevicesDialog::table_selection_changed);
	// connect(devices_table, &QTableWidget::itemChanged, this, &DevicesDialog::item_changed);
	connect(devices_table, &QTableWidget::cellDoubleClicked, this, [this](int, int)
			{ show_edit_device(); });

	devices_table->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(devices_table, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos)
			{
        if (devices_table->selectedItems().isEmpty())
            return;
        QMenu menu(this);
        menu.addAction(edit_action);
        menu.addAction(remove_action);
        menu.exec(devices_table->viewport()->mapToGlobal(pos)); });
}

void DevicesDialog::show_add_device()
{
	DeviceDialog *device_dialog = new DeviceDialog();
	device_dialog->has_collision = [this](offs_t start, offs_t end)
	{
		return debugger->emu_ptr->memory_map->has_collision(start, end);
	};

	device_dialog->setDeviceInfo(DeviceInfo({"Custom Device", {"0100000XXXXXXXXX", 0, 0, 0, 0}}), DeviceDialogMode::AddDevice);

	if (device_dialog->exec() == QDialog::Accepted)
	{
		DeviceInfo device_info = device_dialog->getDeviceInfo();
		debugger->emu_ptr->memory_map->map(new custom_device(device_info.name.toUtf8().constData(), device_info.bit_pattern, false));
		populate_devices_table();
		debugger->save_settings();
		debugger->update_devices();
		debugger->refresh();
	}

	device_dialog->deleteLater();
}

void DevicesDialog::show_edit_device()
{
	int row = devices_table->currentRow();
	if (row < 0)
		return;

	custom_device *device = reinterpret_cast<custom_device *>(devices_table->item(row, 0)->data(Qt::UserRole).value<quintptr>());

	DeviceDialog *device_dialog = new DeviceDialog();
	device_dialog->setDeviceInfo(DeviceInfo({QString::fromStdString(device->name), device->get_bit_pattern()}), DeviceDialogMode::EditDevice);

	if (device_dialog->exec() == QDialog::Accepted)
	{
		DeviceInfo device_info = device_dialog->getDeviceInfo();
		device->name = device_info.name.toUtf8().constData();
		device->set_bit_pattern(device_info.bit_pattern);
		populate_devices_table();
		debugger->save_settings();
		debugger->update_devices();
		debugger->refresh();
	}

	device_dialog->deleteLater();
}

void DevicesDialog::show_remove_device()
{
	int row = devices_table->currentRow();
	if (row < 0)
		return;

	custom_device *device = reinterpret_cast<custom_device *>(devices_table->item(row, 0)->data(Qt::UserRole).value<quintptr>());

	auto result = QMessageBox::question(this, "Remove Custom Device", "Are you sure you want to remove the device \"" + QString::fromStdString(device->name) + "\"?", QMessageBox::StandardButton::Yes, QMessageBox::StandardButton::No);

	if (result == QMessageBox::StandardButton::Yes)
	{
		debugger->emu_ptr->memory_map->unmap(device);

		delete device;

		populate_devices_table();
		debugger->save_settings();
		debugger->update_devices();
		debugger->refresh();
	}
}

void DevicesDialog::retranslateUi()
{
}

void DevicesDialog::table_selection_changed()
{
	bool has_selection = !devices_table->selectedItems().isEmpty();
	edit_action->setEnabled(has_selection);
	remove_action->setEnabled(has_selection);
}

void DevicesDialog::populate_devices_table()
{
	if (!devices_table)
		return;

	devices_table->blockSignals(true);
	devices_table->setRowCount(0);

	auto devices = debugger->emu_ptr->memory_map->get_custom_devices();

	for (const auto &device : devices)
	{
		int row = devices_table->rowCount();
		devices_table->insertRow(row);
		devices_table->setItem(row, 0, new QTableWidgetItem(toHex(device->get_start())));
		devices_table->setItem(row, 1, new QTableWidgetItem(toHex(device->get_end())));
		devices_table->setItem(row, 2, new QTableWidgetItem(QString::fromStdString(device->name)));

		QString pattern = QString::fromLatin1(device->get_pattern());
		for (int i = pattern.length() - 4; i > 0; i -= 4)
			pattern.insert(i, ' ');

		QTableWidgetItem *pattern_item = new QTableWidgetItem(pattern);
		pattern_item->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
		devices_table->setItem(row, 3, pattern_item);

		devices_table->item(row, 0)->setData(Qt::UserRole, QVariant::fromValue<quintptr>(reinterpret_cast<quintptr>(device)));
	}

	devices_table->blockSignals(false);
}
