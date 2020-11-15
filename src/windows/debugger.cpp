#include "debugger.h"
#include "debugger_ui.h"

DebuggerDialog::DebuggerDialog() : DebuggerDialog(nullptr)
{
	setupUI();
	emu_set = false;
}

DebuggerDialog::DebuggerDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
}

void DebuggerDialog::start(bool checked)
{
	if (!emu_ptr->get_running())
	{
		emu_ptr->resume();
		disassembly_view->clear_current();
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

void DebuggerDialog::step(bool checked)
{
	if (!emu_ptr->get_running())
	{
		stepAndUpdateDisassembler();
		disassembly_scrollbar->setValue(disassembly_view->offset);
	}
}

void DebuggerDialog::refresh()
{
	disassembly_view->refresh();
	memory_view->redraw();
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
	memory_groupBox->setVisible(checked);
	settings->showMemoryView = checked;
	save_settings(settings);
}

void DebuggerDialog::toggle_disassembly_panel(bool checked)
{
	// if (count_open_panels() == 0)
	// {
	// 	toggle_disassembly_action->setChecked(true);
	// 	return;
	// }
	// disassembly_groupBox->setVisible(checked);
}

void DebuggerDialog::toggle_status_panel(bool checked)
{
	// if (count_open_panels() == 0)
	// {
	// 	toggle_status_action->setChecked(true);
	// 	return;
	// }
	// status_groupBox->setVisible(checked);
}

DebuggerDialog::~DebuggerDialog()
{
	qDebug() << "Destroying debugger";
	delete memory_view;
	delete memory_scrollbar;
	delete memory_selector;
	delete disassembly_view;
	delete disassembly_scrollbar;
	delete disassembly_selector;
	qDebug() << "Destroying debugger done";
}

void DebuggerDialog::select_memory_location(int index)
{
	QVariant v = memory_selector->itemText(index);
	QString s = v.value<QString>();

	int address = 0;

	if (s == "RAM")
	{
		address = 0x0000;
	}
	else if (s == "ROM")
	{
		address = 0xFC00;
	}
	else if (s == "Keypad")
	{
		address = 0xC003;
	}
	else if (s == "Display")
	{
		address = 0xC110;
	}

	memory_mapped_device *device = emu_ptr->get_block_device(address);
	int start = device->get_start();
	int end = device->get_end();
	uint8_t *memory = device->get_mapped_memory();

	memory_view->set_range(start, end, memory);
	memory_scrollbar->setValue(0);
}

void DebuggerDialog::select_disassembly_location(int index)
{
	QVariant v = disassembly_selector->itemText(index);
	QString s = v.value<QString>();

	int sstart = 0;
	if (s == "RAM")
	{
		sstart = 0x0000;
	}
	else if (s == "ROM")
	{
		sstart = 0xFC00;
	}

	memory_mapped_device *device = emu_ptr->get_block_device(sstart);
	int start = device->get_start();
	int end = device->get_end();
	uint8_t *memory = device->get_mapped_memory();

	disassembly_view->set_range(start, end, memory);
	disassembly_scrollbar->setValue(0);
}

void DebuggerDialog::update_memory_scrollbar(int value)
{
	memory_scrollbar->setValue(memory_scrollbar->value() - value);
}

void DebuggerDialog::update_memory_scrollbar_max(int value)
{
	memory_scrollbar->setMinimum(0);
	memory_scrollbar->setMaximum(value);
}

void DebuggerDialog::update_disassembly_scrollbar(int value)
{
	disassembly_scrollbar->setValue(disassembly_scrollbar->value() - value);
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
		emu_ptr->on_breakpoint = [this] {
			// We cannot update UI items from another thread, so we trigger a QAction breakpoint_handler_action call breakpoint_handler asynchronously
			breakpoint_handler_action->trigger();
		};
		emu_set = true;
		memory_view->set_emulator(emu);
		disassembly_view->set_emulator(emu);
		status_view->set_emulator(emu);
		memory_selector->setCurrentIndex(0);
		disassembly_selector->setCurrentIndex(1);
		select_memory_location(0);
	}
	update_button_state();
}

void DebuggerDialog::set_settings(Settings *settings)
{
	this->settings = settings;
	memory_groupBox->setVisible(settings->showMemoryView);
	toggle_memory_action->setChecked(settings->showMemoryView);
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
	step_button->setEnabled(!running);
	//reset_button->setEnabled(!running);
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
	//switch (event->key())
	//{
	//case Qt::Key_F4:
	//	if (emu_ptr->get_running())
	//	{
	//		pauseAndUpdateDisassembler();
	//		update_button_state();
	//	}
	//	break;
	//case Qt::Key_F5:
	//	if (!emu_ptr->get_running())
	//	{
	//		emu_ptr->resume();
	//		disassembly_view->clear_current();
	//		update_button_state();
	//	}
	//	break;
	//case Qt::Key_F10:
	//	if (!emu_ptr->get_running())
	//	{
	//		stepAndUpdateDisassembler();
	//	}
	//	break;
	//case Qt::Key_Escape:
	//	emu_ptr->reset();
	//	break;
	//}
	event->ignore();
};

void DebuggerDialog::pauseAndUpdateDisassembler()
{
	emu_ptr->halt();
	offs_t address = emu_ptr->get_status().pc;

	memory_mapped_device *device = emu_ptr->get_block_device(address);
	int start = device->get_start();

	if (start == 0x0000)
	{
		disassembly_selector->setCurrentIndex(0);
	}
	else if (start == 0xFC00)
	{
		disassembly_selector->setCurrentIndex(1);
	}

	disassembly_view->set_current(address);
}

void DebuggerDialog::stepAndUpdateDisassembler()
{
	emu_ptr->step();
	offs_t address = emu_ptr->get_status().pc;

	memory_mapped_device *device = emu_ptr->get_block_device(address);
	int start = device->get_start();

	if (start == 0x0000)
	{
		disassembly_selector->setCurrentIndex(0);
	}
	else if (start == 0xFC00)
	{
		disassembly_selector->setCurrentIndex(1);
	}

	disassembly_view->set_current(address);
	disassembly_view->clear_selected();
}

void DebuggerDialog::keyReleaseEvent(QKeyEvent *event){

};

void DebuggerDialog::add_breakpoint(offs_t address)
{
	emu_ptr->add_breakpoint(address);
}

void DebuggerDialog::remove_breakpoint(offs_t address)
{
	emu_ptr->remove_breakpoint(address);
}

void DebuggerDialog::add_or_remove_breakpoint(offs_t address)
{
	emu_ptr->add_or_remove_breakpoint(address);
}

void DebuggerDialog::load_ram()
{
	File::load_ram(this, emu_ptr);
	after_load_ram();
}

void DebuggerDialog::save_ram()
{
	File::save_ram(this, emu_ptr);
}

void DebuggerDialog::load_breakpoints()
{
	bool success;
	QString fileName = QFileDialog::getOpenFileName(this,
													"Load Breakpoints", "", "Breakpoint Files (*.brk)");

	if (fileName == nullptr)
		return;

	emu_ptr->stop();
	emu_ptr->breakpoints->loadBreakpoints(fileName, success);
	disassembly_view->refresh();
	emu_ptr->start();
}

void DebuggerDialog::save_breakpoints()
{
	bool success;
	QString fileName = QFileDialog::getSaveFileName(this,
													"Save Breakpoints", "", "Breakpoint Files (*.brk)");

	if (fileName == nullptr)
		return;

	emu_ptr->breakpoints->saveBreakpoints(fileName, success);
}

void DebuggerDialog::load_labels()
{
	bool success;
	QString fileName = QFileDialog::getOpenFileName(this,
													"Load Labels", "", "Label Files (*.map, *.lbl)");

	if (fileName == nullptr)
		return;

	emu_ptr->stop();
	emu_ptr->labels->loadLabels(fileName, success);
	disassembly_view->refresh();
	emu_ptr->start();
}

void DebuggerDialog::save_labels()
{
	bool success;
	QString fileName = QFileDialog::getSaveFileName(this,
													"Save Labels", "", "Label Files (*.lbl)");

	if (fileName == nullptr)
		return;

	emu_ptr->labels->saveLabels(fileName, 0x0000, 0x03FF, success);
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
