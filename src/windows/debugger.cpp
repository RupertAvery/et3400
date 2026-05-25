#include "debugger.h"
#include <QMessageBox>
#include "debugger_ui.h"
#include "goto.h"
#include "label.h"
#include "mainwindow.h"
#include "../util/log.h"
#include "clear_ram.h"
#include "../common/util.h"

DebuggerDialog::DebuggerDialog() : DebuggerDialog(nullptr)
{
	setupUI();
	emu_set = false;
}

DebuggerDialog::DebuggerDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint)
{
}

void DebuggerDialog::start(bool checked)
{
	if (!emu_ptr->get_running())
	{
		emu_ptr->resume();
		disassembly_view->clearCurrent();
		update_button_state();
	}
}

void DebuggerDialog::stop(bool checked)
{
	if (emu_ptr->get_running())
	{
		pauseAndUpdateDisassembler();
		update_button_state();
		disassembly_scrollbar->setValue(disassembly_view->offset);
	}
}

void DebuggerDialog::step_into(bool checked)
{
	if (!emu_ptr->get_running())
	{
		stepAndUpdateDisassembler();
		disassembly_scrollbar->setValue(disassembly_view->offset);
	}
}

void DebuggerDialog::step_over(bool checked)
{
	if (!emu_ptr->get_running())
	{
		offs_t pc = emu_ptr->get_status().pc;
		uint8_t opcode = emu_ptr->read_byte(pc);

		int *table_entry = Disassembler::GetTableEntry(opcode);

		int instruction_length = Disassembler::get_instruction_length(table_entry[1]);
		offs_t next_instruction = pc + instruction_length;

		if (Disassembler::IsSubroutine(table_entry[0]))
		{
			// Hack? Step into next instruction so that we move past the breakpoint,
			// otherwise we end up on the same instruction for one pass
			emu_ptr->step();
			emu_ptr->breakpoints->addBreakpoint(next_instruction, true);
			emu_ptr->start();
			disassembly_view->clearCurrent();
			disassembly_view->clearSelected();
			update_button_state();
			// forces a redraw - should we just call rebuild?
			disassembly_scrollbar->setValue(disassembly_view->offset);
		}
		else
		{
			stepAndUpdateDisassembler();
			// forces a redraw - should we just call rebuild?
			disassembly_scrollbar->setValue(disassembly_view->offset);
		}
	}
}

void DebuggerDialog::step_out(bool checked)
{
	if (!emu_ptr->get_running())
	{
		emu_ptr->set_step_out();

		emu_ptr->start();
		disassembly_view->clearCurrent();
		disassembly_view->clearSelected();
		update_button_state();
		// forces a redraw - should we just call rebuild?
		disassembly_scrollbar->setValue(disassembly_view->offset);
	}
}

void DebuggerDialog::refresh()
{
	if (settings->showDasmView)
	{
		disassembly_view->rebuild();
	}
	if (settings->showMemoryView)
	{
		memory_view->redraw();
	}
}

void DebuggerDialog::reset(bool checked)
{
	emu_ptr->reset();
}

int DebuggerDialog::count_open_panels()
{
	return 1;
	// return (toggle_memory_action->isChecked() ? 1 : 0) +
	// 	   (toggle_disassembly_action->isChecked() ? 1 : 0) +
	// 	   (toggle_status_action->isChecked() ? 1 : 0);
}

void DebuggerDialog::toggle_memory_panel(bool checked)
{
	// if (count_open_panels() == 0)
	// {
	// 	toggle_memory_action->setChecked(true);
	// 	return;
	// }
	LOG_DEBUG << "toggle memory";
	memory_groupBox->setVisible(checked);
	settings->showMemoryView = checked;
	resize(sizeHint().width(), height());
	save_settings(settings);
}

void DebuggerDialog::toggle_heat_map(bool checked)
{
	memory_view->setHeatMapEnabled(checked);
	settings->showHeatMap = checked;

	LOG_DEBUG << "toggle heat";
	save_settings(settings);
}

void DebuggerDialog::toggle_disassembly_panel(bool checked)
{
	// if (count_open_panels() == 0)
	// {
	// 	toggle_disassembly_action->setChecked(true);
	// 	return;
	// }
	disassembly_groupBox->setVisible(checked);
	settings->showDasmView = checked;
	resize(sizeHint().width(), height());

	LOG_DEBUG << "toggle dasm";
	save_settings(settings);
}

void DebuggerDialog::toggle_auto_refresh_disassembly_panel(bool checked)
{
	// if (count_open_panels() == 0)
	// {
	// 	toggle_disassembly_action->setChecked(true);
	// 	return;
	// }
	disassembly_view->setAutoRefresh(checked);
	settings->autoRefreshDasm = checked;
	save_settings(settings);
}

void DebuggerDialog::toggle_status_panel(bool checked)
{
	// if (count_open_panels() == 0)
	// {
	// 	toggle_status_action->setChecked(true);
	// 	return;
	// }
	status_groupBox->setVisible(checked);
}

DebuggerDialog::~DebuggerDialog()
{
	LOG_DEBUG << "DebuggerDialog destroy";
	delete memory_view;
	delete memory_scrollbar;
	delete memory_selector;
	delete disassembly_view;
	delete disassembly_scrollbar;
	delete disassembly_selector;
	LOG_DEBUG << "DebuggerDialog destroy done";
}

void DebuggerDialog::select_memory_location(int index)
{
	QVariant v = memory_selector->itemData(index);
	memory_mapped_device *device = (memory_mapped_device *)v.value<quintptr>();

	if (device == nullptr)
		return;

	memory_view->set_range(device->get_start(), device->get_end(), device->get_mapped_memory());
	memory_scrollbar->setValue(0);
}

void DebuggerDialog::select_disassembly_location(int index)
{
	QVariant v = disassembly_selector->itemData(index);
	memory_mapped_device *device = (memory_mapped_device *)v.value<quintptr>();

	if (device == nullptr)
		return;

	disassembly_view->set_range(device->get_start(), device->get_end(), device->get_mapped_memory());
	disassembly_scrollbar->setValue(0);
	labels_dialog->populate_labels_table();
	update_clear_ram_labels_state();
}

// void DebuggerDialog::update_clear_ram_labels_state()
// {
// 	if (tab_clear_ram_labels_action)
// 		tab_clear_ram_labels_action->setEnabled(true);
// }

void DebuggerDialog::update_memory_scrollbar(int value)
{
	memory_scrollbar->setValue(memory_scrollbar->value() - value);
}

void DebuggerDialog::update_memory_scrollbar_max(int value)
{
	memory_scrollbar->setMinimum(0);
	memory_scrollbar->setMaximum(value);
}

void DebuggerDialog::adjustDisassemblyScrollbar(int value)
{
	disassembly_scrollbar->setValue(disassembly_scrollbar->value() - value);
}

void DebuggerDialog::setDisassemblyScrollbar(int value)
{
	disassembly_scrollbar->setValue(value);
}

void DebuggerDialog::update_disassembly_scrollbar_max(int value)
{
	disassembly_scrollbar->setMinimum(0);
	disassembly_scrollbar->setMaximum(value - 1);
}

void DebuggerDialog::set_emulator(et3400emu *emu)
{
	if (!emu_set)
	{
		emu_ptr = emu;
		emu_ptr->on_breakpoint = [this]
		{
			// We cannot update UI items from another thread, so we trigger a QAction breakpoint_handler_action call breakpoint_handler asynchronously
			breakpoint_handler_action->trigger();
		};
		emu_set = true;
		memory_view->set_emulator(emu);
		disassembly_view->setEmulator(emu);
		status_view->set_emulator(emu);

		auto devices = emu->memory_map->get_block_devices();
		for (auto *device : devices)
		{
			if (device->can_disassemble)
			{
				disassembly_selector->addItem(QString::fromStdString(device->name), QVariant::fromValue((quintptr)device));
			}
			memory_selector->addItem(QString::fromStdString(device->name), QVariant::fromValue((quintptr)device));
		}

		// memory_selector->addItem("RAM", 0x0000);
		// memory_selector->addItem("Keypad", 0xC003);
		// memory_selector->addItem("Display", 0xC110);
		// memory_selector->addItem("Fantom II ROM", 0x1400);
		// memory_selector->addItem("TinyBasic ROM", 0x1C00);
		// memory_selector->addItem("Monitor ROM", 0xFC00);

		// disassembly_selector->addItem("RAM", 0x0000);
		// disassembly_selector->addItem("Fantom II ROM", 0x1400);
		// disassembly_selector->addItem("TinyBasic ROM", 0x1C00);
		// disassembly_selector->addItem("Monitor ROM", 0xFC00);

		memory_selector->setCurrentIndex(0);
		disassembly_selector->setCurrentIndex(0);

		select_memory_location(0);
	}
	update_button_state();
}

void DebuggerDialog::set_settings(Settings *settings)
{
	this->settings = settings;

	disassembly_groupBox->setVisible(settings->showDasmView);
	memory_groupBox->setVisible(settings->showMemoryView);
	memory_view->setHeatMapEnabled(settings->showHeatMap);

	toggle_disassembly_action->setChecked(settings->showDasmView);
	toggle_autorefresh_disassembly_action->setChecked(settings->autoRefreshDasm);
	toggle_memory_action->setChecked(settings->showMemoryView);
	toggle_heat_map_action->setChecked(settings->showHeatMap);

	if (settings->debuggerWidth > 0 && settings->debuggerHeight > 0)
		resize(settings->debuggerWidth, settings->debuggerHeight);
	else
		resize(sizeHint().width(), height());

	if (settings->debuggerX >= 0 && settings->debuggerY >= 0)
		move(settings->debuggerX, settings->debuggerY);
}

void DebuggerDialog::set_parent_window(MainWindow *parent)
{
	parent_window = parent;
}

void DebuggerDialog::breakpoint_handler(bool checked)
{
	pauseAndUpdateDisassembler();
	update_button_state();
	disassembly_scrollbar->setValue(disassembly_view->offset);
}

void DebuggerDialog::update_button_state()
{
	bool running = emu_ptr->get_running();
	start_button->setEnabled(!running);
	stop_button->setEnabled(running);
	step_into_button->setEnabled(!running);
	step_over_button->setEnabled(!running);
	// step_out_button->setEnabled(!running);
	reset_button->setEnabled(running);

	if (debug_run_action)
	{
		debug_run_action->setEnabled(!running);
		debug_stop_action->setEnabled(running);
		debug_step_into_action->setEnabled(!running);
		debug_step_over_action->setEnabled(!running);
		// debug_step_out_action->setEnabled(!running);
		debug_reset_action->setEnabled(running);
	}
}

void DebuggerDialog::memory_slider_moved(int value)
{
	memory_view->scrollTo(value);
}

void DebuggerDialog::disassembly_slider_moved(int value)
{
	disassembly_view->scrollTo(value);
}

void DebuggerDialog::keyPressEvent(QKeyEvent *event)
{
	// These are now handled by the toolbar actions
	// switch (event->key())
	//{
	// case Qt::Key_F4:
	//	if (emu_ptr->get_running())
	//	{
	//		pauseAndUpdateDisassembler();
	//		update_button_state();
	//	}
	//	break;
	// case Qt::Key_F5:
	//	if (!emu_ptr->get_running())
	//	{
	//		emu_ptr->resume();
	//		disassembly_view->clear_current();
	//		update_button_state();
	//	}
	//	break;
	// case Qt::Key_F10:
	//	if (!emu_ptr->get_running())
	//	{
	//		stepAndUpdateDisassembler();
	//	}
	//	break;
	// case Qt::Key_Escape:
	//	emu_ptr->reset();
	//	break;
	//}
	event->ignore();
};

void DebuggerDialog::pauseAndUpdateDisassembler()
{
	emu_ptr->halt();
	offs_t address = emu_ptr->get_status().pc;

	// memory_mapped_device *device = emu_ptr->get_block_device(address);
	// int start = 0xFC00;

	// if (device == nullptr)
	// {
	// 	address = start;
	// }
	// else
	// {
	// 	start = device->get_start();
	// }

	selectByAddress(address);

	disassembly_view->setCurrent(address);
}

void DebuggerDialog::stepAndUpdateDisassembler()
{
	emu_ptr->step();
	offs_t address = emu_ptr->get_status().pc;

	// memory_mapped_device *device = emu_ptr->get_block_device(address);
	// int start = 0xFC00;

	// if (device == nullptr)
	// {
	// 	address = start;
	// }
	// else
	// {
	// 	start = device->get_start();
	// }

	selectByAddress(address);

	disassembly_view->setCurrent(address);
	disassembly_view->clearSelected();
}

void DebuggerDialog::selectByAddress(offs_t address)
{
	for (int i = 0; i < disassembly_selector->count(); ++i)
	{
		memory_mapped_device *device = (memory_mapped_device *)(quintptr)disassembly_selector->itemData(i).toULongLong();
		if (device && address >= device->get_start() && address <= device->get_end())
		{
			disassembly_selector->setCurrentIndex(i);
			return;
		}
	}
}

void DebuggerDialog::keyReleaseEvent(QKeyEvent *event) {

};

void DebuggerDialog::resizeEvent(QResizeEvent *event)
{
	QDialog::resizeEvent(event);
	LOG_DEBUG << "memory_groupBox width:" << memory_groupBox->width()
			  << "disassembly_groupBox width:" << disassembly_groupBox->width()
			  << "status_groupBox width:" << status_groupBox->width();
}

void DebuggerDialog::closeEvent(QCloseEvent *event)
{
	if (labels_dialog)
		labels_dialog->close();
	if (breakpoints_dialog)
		breakpoints_dialog->close();
	if (settings)
	{
		settings->debuggerX = pos().x();
		settings->debuggerY = pos().y();
		settings->debuggerWidth = width();
		settings->debuggerHeight = height();
	}
	if (emu_ptr && !emu_ptr->get_running())
		emu_ptr->resume();
	QDialog::closeEvent(event);
}

void DebuggerDialog::load_rom()
{
	File::load_rom_dialog(this, emu_ptr, parent_window->load_rom_settings, settings->romDir);
	after_load_rom();
}

void DebuggerDialog::load_ram()
{
	File::load_ram_dialog(this, emu_ptr, parent_window->load_ram_settings, settings->ramDir);
	after_load_ram();
}

void DebuggerDialog::save_ram()
{
	File::save_ram_dialog(this, emu_ptr, parent_window->save_ram_settings, settings->ramDir);
}

void DebuggerDialog::reset_disassembly_view()
{
	disassembly_view->rebuild();
	QVariant v = disassembly_selector->itemData(disassembly_selector->currentIndex());
	memory_mapped_device *device = (memory_mapped_device *)v.value<quintptr>();
	disassembly_view->set_range(device->get_start(), device->get_end(), device->get_mapped_memory());
	disassembly_scrollbar->setValue(0);
}

void DebuggerDialog::save_ram_labels()
{
	memory_mapped_device *device = emu_ptr->memory_map->try_get_block_device("RAM");
	if (device)
		File::save_labels_dialog(this, emu_ptr, device->get_start(), device->get_end(), settings->labelsDir);
}

void DebuggerDialog::save_labels()
{
	QVariant v = disassembly_selector->itemData(disassembly_selector->currentIndex());
	memory_mapped_device *device = (memory_mapped_device *)v.value<quintptr>();
	if (device)
		File::save_labels_dialog(this, emu_ptr, device->get_start(), device->get_end(), settings->labelsDir);
	else
		File::save_labels_dialog(this, emu_ptr, settings->labelsDir);
}

void DebuggerDialog::after_load_rom()
{
	refresh();
	update_button_state();
	memory_scrollbar->setValue(0);

	QVariant v = disassembly_selector->itemText(disassembly_selector->currentIndex());
	QString s = v.value<QString>();

	if (s == "ROM")
	{
		disassembly_scrollbar->setValue(0);
	}
}

void DebuggerDialog::after_load_ram()
{
	refresh();
	update_button_state();
	memory_scrollbar->setValue(0);

	QVariant v = disassembly_selector->itemText(disassembly_selector->currentIndex());
	QString s = v.value<QString>();

	if (s == "RAM")
	{
		disassembly_scrollbar->setValue(0);
	}
}

void DebuggerDialog::clear_ram()
{
	ClearRamDialog clearRamDialog;

	clearRamDialog.setSettings(clearRamSettings);

	QDialog::DialogCode result = (QDialog::DialogCode)clearRamDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		clearRamSettings = clearRamDialog.getSettings();

		memory_mapped_device *ram = emu_ptr->memory_map->try_get_block_device("RAM");

		if (ram != nullptr)
		{
			emu_ptr->stop();
			uint16_t addr = clearRamSettings.start;
			while (addr <= clearRamSettings.end)
			{
				ram->write(addr, clearRamSettings.value);
				addr++;
			}
			emu_ptr->reset();
			emu_ptr->start();

			disassembly_view->rebuild();
		}
	}
}

void DebuggerDialog::diassembly_refresh()
{
	disassembly_view->rebuild();
}

void DebuggerDialog::goto_label()
{
	GotoDialog gotoDialog;
	gotoDialog.setLabels(emu_ptr->labels->getLabels());

	QDialog::DialogCode result = (QDialog::DialogCode)gotoDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		offs_t address = gotoDialog.getSelectedAddress();
		memory_mapped_device *device = emu_ptr->get_block_device(address);
		int start = device->get_start();

		selectByAddress(start);

		disassembly_view->setSelected(address);
		disassembly_scrollbar->setValue(disassembly_view->offset);
	}
}

void DebuggerDialog::load_labels()
{
	File::load_labels_dialog(this, emu_ptr, settings->labelsDir);
	reset_disassembly_view();
	if (labels_dialog)
		labels_dialog->populate_labels_table();
}

void DebuggerDialog::load_breakpoints()
{
	File::load_breakpoint_dialog(this, emu_ptr, settings->breakpointsDir);
	populate_breakpoints_table();
	disassembly_view->rebuild();
}

void DebuggerDialog::save_breakpoints()
{
	File::save_breakpoint_dialog(this, emu_ptr, settings->breakpointsDir);
}

void DebuggerDialog::load_default_labels()
{
	if (labels_dialog)
		labels_dialog->load_default_labels();
}

void DebuggerDialog::populate_breakpoints_table()
{
	if (breakpoints_dialog)
		breakpoints_dialog->populate_breakpoints_table();
}

void DebuggerDialog::update_clear_ram_labels_state()
{
}

void DebuggerDialog::show_labels_dialog()
{
	labels_dialog->show();
	labels_dialog->raise();
	labels_dialog->activateWindow();
}

void DebuggerDialog::show_breakpoints_dialog()
{
	breakpoints_dialog->show();
	breakpoints_dialog->raise();
	breakpoints_dialog->activateWindow();
}

void DebuggerDialog::show_save_view_dialog()
{
	memory_mapped_device *dasm = get_disassembly_device();
	memory_mapped_device *mem = get_memory_device();
	File::save_memory_mapped_devices_dialog(this, emu_ptr, dasm, mem);
}


memory_mapped_device *DebuggerDialog::get_disassembly_device()
{
	QVariant v = disassembly_selector->itemData(disassembly_selector->currentIndex());
	memory_mapped_device *device = (memory_mapped_device *)v.value<quintptr>();
	return device;
}

memory_mapped_device *DebuggerDialog::get_memory_device()
{
	QVariant v = memory_selector->itemData(memory_selector->currentIndex());
	memory_mapped_device *device = (memory_mapped_device *)v.value<quintptr>();
	return device;
}


void DebuggerDialog::add_breakpoint(offs_t address)
{
	emu_ptr->add_breakpoint(address);
	populate_breakpoints_table();
}

void DebuggerDialog::remove_breakpoint(offs_t address)
{
	emu_ptr->remove_breakpoint(address);
	populate_breakpoints_table();
}

void DebuggerDialog::add_or_remove_breakpoint(offs_t address)
{
	emu_ptr->add_or_remove_breakpoint(address);
	populate_breakpoints_table();
}

void DebuggerDialog::set_breakpoint_enabled(offs_t address, bool enabled)
{
	emu_ptr->breakpoints->setEnabled(address, enabled);
	populate_breakpoints_table();
}
