#include "debugger.h"
#include "debugger_ui.h"

DebuggerDialog::DebuggerDialog() : DebuggerDialog(nullptr)
{
	setupUI();
	emu_set = false;
}

DebuggerDialog::DebuggerDialog(QWidget* parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
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
	}
}

void DebuggerDialog::step(bool checked)
{
	if (!emu_ptr->get_running())
	{
		stepAndUpdateDisassembler();
	}
}

void DebuggerDialog::reset(bool checked)
{
	emu_ptr->reset();
}

int DebuggerDialog::count_open_panels()
{
	return (toggle_memory_action->isChecked() ? 1 : 0) +
		(toggle_disassembly_action->isChecked() ? 1 : 0) +
		(toggle_status_action->isChecked() ? 1 : 0);
}

void DebuggerDialog::toggle_memory_panel(bool checked)
{
	if (count_open_panels() == 0)
	{
		toggle_memory_action->setChecked(true);
		return;
	}
	memory_groupBox->setVisible(checked);
}

void DebuggerDialog::toggle_disassembly_panel(bool checked)
{
	if (count_open_panels() == 0)
	{
		toggle_disassembly_action->setChecked(true);
		return;
	}
	disassembly_groupBox->setVisible(checked);
}

void DebuggerDialog::toggle_status_panel(bool checked)
{
	if (count_open_panels() == 0)
	{
		toggle_status_action->setChecked(true);
		return;
	}
	status_groupBox->setVisible(checked);
}

DebuggerDialog::~DebuggerDialog()
{
	qDebug() << "Destroying debugger";
	delete memory_view;
	delete memmory_scrollbar;
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

	qDebug() << v;

	int sstart = 0;
	if (s == "RAM")
	{
		sstart = 0x0000;
	}
	else if (s == "ROM")
	{
		sstart = 0xFC00;
	}

	memory_mapped_device* device = emu_ptr->memory_map->get_block_device(sstart);
	int start = device->get_start();
	int end = device->get_end();
	uint8_t* memory = device->get_mapped_memory();

	memory_view->set_range(start, end, memory);
}

void DebuggerDialog::select_disassembly_location(int index)
{
	QVariant v = disassembly_selector->itemText(index);
	QString s = v.value<QString>();

	qDebug() << v;

	int sstart = 0;
	if (s == "RAM")
	{
		sstart = 0x0000;
	}
	else if (s == "ROM")
	{
		sstart = 0xFC00;
	}

	memory_mapped_device* device = emu_ptr->memory_map->get_block_device(sstart);
	int start = device->get_start();
	int end = device->get_end();
	uint8_t* memory = device->get_mapped_memory();

	disassembly_view->set_range(start, end, memory);
}

void DebuggerDialog::update_memory_scrollbar(int value)
{
	memmory_scrollbar->setValue(memmory_scrollbar->value() - value);
}

void DebuggerDialog::update_memory_scrollbar_max(int value)
{
	memmory_scrollbar->setMinimum(0);
	memmory_scrollbar->setMaximum(value);
}

void DebuggerDialog::update_disassembly_scrollbar(int value)
{
	disassembly_scrollbar->setValue(disassembly_scrollbar->value() - value);
}

void DebuggerDialog::update_disassembly_scrollbar_max(int value)
{
	disassembly_scrollbar->setMinimum(0);
	disassembly_scrollbar->setMaximum(value);
}

void DebuggerDialog::set_emulator(et3400emu* emu)
{
	if (!emu_set)
	{
		emu_ptr = emu;
		emu_ptr->on_breakpoint = [this] {
			breakpoint_handler_action->trigger();
		};
		emu_set = true;
		memory_view->set_emulator(emu);
		disassembly_view->set_emulator(emu);
		disassembly_view->set_maps(emu->maps);
		status_view->set_emulator(emu);
		memory_selector->setCurrentIndex(0);
		disassembly_selector->setCurrentIndex(1);
		select_memory_location(0);
		select_disassembly_location(1);
	}
	update_button_state();
}


void DebuggerDialog::breakpoint_handler(bool checked)
{
	pauseAndUpdateDisassembler();
	update_button_state();
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

void DebuggerDialog::keyPressEvent(QKeyEvent* event)
{

	switch (event->key())
	{
	case Qt::Key_F4:
		if (emu_ptr->get_running())
		{
			pauseAndUpdateDisassembler();
			update_button_state();
		}
		break;
	case Qt::Key_F5:
		if (!emu_ptr->get_running())
		{
			emu_ptr->resume();
			disassembly_view->clear_current();
			update_button_state();
		}
		break;
	case Qt::Key_F10:
		if (!emu_ptr->get_running())
		{
			stepAndUpdateDisassembler();
		}
		break;
	case Qt::Key_Escape:
		emu_ptr->reset();
		break;
	}
};

void DebuggerDialog::pauseAndUpdateDisassembler()
{
	emu_ptr->halt();
	offs_t address = emu_ptr->get_status().pc;

	memory_mapped_device* device = emu_ptr->memory_map->get_block_device(address);
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

	memory_mapped_device* device = emu_ptr->memory_map->get_block_device(address);
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

void DebuggerDialog::keyReleaseEvent(QKeyEvent* event) {

};

void DebuggerDialog::add_breakpoint(offs_t address)
{
	emu_ptr->add_breakpoint(address);
}
void DebuggerDialog::remove_breakpoint(offs_t address)
{
	emu_ptr->remove_breakpoint(address);
}
