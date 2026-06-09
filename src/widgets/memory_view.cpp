#include "memory_view.h"
#include "colors.h"

#include "../util/log.h"
#include "../common/util.h"
#include <QStringBuilder>

MemoryView::MemoryView(QWidget *parent)
	: QFrame(parent)
{
	// setMidLineWidth(0);
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setLineWidth(3);
	setFocusPolicy(Qt::StrongFocus);

	// setBackgroundRole(QPalette::Base);
	// setAutoFillBackground(true);
	running = true;
	offset = 0;

	m_font = QFont("Courier", 12);
	m_font.setWeight(QFont::Medium);
	m_fm = new QFontMetrics(m_font);
	item_height = m_fm->lineSpacing();

	QColor cold = QColor("#ffffff");
	QColor hot = QColor("#d8ce44");
	for (int i = 0; i < 16; ++i)
	{
		float t = i / 15.0f;
		m_heat_colors[i] = QColor(
			cold.red() + t * (hot.red() - cold.red()),
			cold.green() + t * (hot.green() - cold.green()),
			cold.blue() + t * (hot.blue() - cold.blue()));
	}

	m_paintTimer = new QTimer(this);
	m_paintTimer->start(100); // 10fps is plenty for a memory view
	connect(this->m_paintTimer, &QTimer::timeout, this, &MemoryView::redraw);

	buffer = new QPixmap;
	setLineWidth(3);
}

MemoryView::~MemoryView()
{
	LOG_DEBUG << "MemoryView destroy";
	m_paintTimer->stop();
	delete m_paintTimer;
	delete buffer;
	delete m_fm;
	LOG_DEBUG << "MemoryView destroy done";
}

void MemoryView::wheelEvent(QWheelEvent *event)
{
	int degrees = event->delta() / 8;
	int steps = degrees / 15;

	if (event->orientation() == Qt::Vertical)
	{
		scroll(steps);
		emit on_scroll(steps);
	}
	event->accept();
}

void MemoryView::scroll(int steps)
{
	offset -= steps;
	if (offset < 0)
		offset = 0;
	if (offset > max_vscroll)
		offset = max_vscroll;
	this->update();
}

void MemoryView::scrollTo(int value)
{
	offset = value;
	if (offset < 0)
		offset = 0;
	if (offset > max_vscroll)
		offset = max_vscroll;
	this->update();
}

void MemoryView::rebuild()
{
	MemoryBuilder::build(lines, start, end, memory);
	this->update();
}

void MemoryView::bufferDraw()
{
	QPainter painter(buffer);
	painter.setBrush(QBrush(Qt::white));
	painter.fillRect(contentsRect(), painter.brush());

	painter.setFont(m_font);
	visible_items = height() / item_height;

	int y = item_height;

	QColor darkblue = QColor("#00018B");
	QColor darkred = QColor("#8B0000");
	QBrush selected_brush = QBrush(selected_bg_color);

	bool hasFocus = this->hasFocus();

	painter.save();

	// Snapshot live memory once — all reads below use shadow, not the live buffer
	memcpy(shadow_memory, memory, end - start + 1);

	if (is_editing)
	{
		blink_counter -= 1;
		if (blink_counter <= 0)
		{
			blink_counter = blink_counter_max;
		}
	}

	int half_life = blink_counter_max / 2;

	if (heat_map_enabled)
	{
		for (int i = 0; i < end - start + 1; i++)
		{
			if (last_memory[i] - shadow_memory[i] != 0)
			{
				heat_map[i] = 255;
			}
			else if (heat_map[i] > 0)
			{
				heat_map[i] = heat_map[i] > heat_map_decay ? heat_map[i] - heat_map_decay : 0;
			}
		}
	}

	int ascent = m_fm->ascent();
	int descent = m_fm->descent();

	for (int line = offset; line < offset + visible_items && (start + (line * 8) < end); line++)
	{
		int address = start + line * 8;

		painter.setPen(darkblue);
		painter.drawText(5, y, toHex(address) % ":");

		int i = 0;

		while (address + i <= end && i < 8)
		{
			int heat_idx = address + i - start;
			int text_x = 80 + i * 30;
			int box_x = text_x - 4;
			int box_y = y - ascent - 1;
			int box_width = 28;
			int box_height = ascent + descent;

			painter.setPen(darkred);

			if (heat_map_enabled && heat_map[heat_idx] > 0)
			{
				painter.fillRect(box_x, box_y, box_width, box_height,
								 m_heat_colors[heat_map[heat_idx] / 16]);
			}

			if (hasFocus && !is_editing && selected_address == address + i)
			{

				painter.fillRect(box_x, box_y, box_width, box_height, selected_brush);
			}

			if (hasFocus && is_editing && address + i == editing_address)
			{
				painter.setPen(Qt::blue);
				painter.drawRect(box_x, box_y, box_width, box_height);

				if (editing_nibble == 0 && blink_counter > half_life)
				{
					painter.drawText(text_x, y, QString(" %1").arg(editing_value & 0x0F, 1, 16, QChar('0')).toUpper());
				}
				else if (editing_nibble == 1 && blink_counter > half_life)
				{
					painter.drawText(text_x, y, QString("%1 ").arg((editing_value >> 4) & 0x0F, 1, 16, QChar('0')).toUpper());
				}
				else
				{
					painter.drawText(text_x, y, QString("%1").arg(editing_value, 2, 16, QChar('0')).toUpper());
				}
			}
			else
			{
				if (hasFocus && !is_editing && address + i == selected_address)
				{
					painter.setPen(selected_fg_color);
				}

				painter.drawText(text_x, y, QString("%1").arg(shadow_memory[address + i - start], 2, 16, QChar('0')).toUpper());
			}

			i++;
		}

		while (i < 8)
		{
			painter.setPen(darkred);
			painter.drawText(80 + i * 30, y, QString("%1").arg(0, 2, 16, QChar('0')).toUpper());
			i++;
		}

		y += item_height;
	}

	memcpy(last_memory, shadow_memory, end - start + 1);

	painter.restore();
}

void MemoryView::setHeatMapEnabled(bool enabled)
{
	heat_map_enabled = enabled;
}

void MemoryView::setHeatMapDecay(int decay)
{
	heat_map_decay = decay;
	if (decay == 0)
	{
		for (int i = 0; i < end - start + 1; i++)
		{
			if (this->heat_map[i] > 0)
			{
				this->heat_map[i] = 255;
			}
		}
	}
}

void MemoryView::clearHeatMap()
{
	memset(this->heat_map, 0, end - start + 1);
}

void MemoryView::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();
	buffer = new QPixmap(size);
	visible_items = size.height() / item_height;
	int x = (end - start) / 8 - visible_items + 2;
	max_vscroll = x > 0 ? x : 0;
	emit on_size(max_vscroll);
}

void MemoryView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (is_device_set)
	{
		bufferDraw();
		painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
		painter.end();
	}
	QFrame::paintEvent(event);
}

void MemoryView::redraw()
{
	this->update();
}

void MemoryView::update_display()
{
	// action->trigger();
}

void MemoryView::set_device(memory_mapped_device *device)
{
	this->device = device;

	this->start = device->get_start();
	this->end = device->get_end();
	this->memory = device->get_mapped_memory();

	if (this->last_memory != nullptr)
		free(this->last_memory);

	if (this->heat_map != nullptr)
		free(this->heat_map);

	if (this->shadow_memory != nullptr)
		free(this->shadow_memory);

	this->last_memory = (uint8_t *)calloc(end - start + 1, 1);
	this->shadow_memory = (uint8_t *)calloc(end - start + 1, 1);
	this->heat_map = (uint8_t *)calloc(end - start + 1, 1);

	resizeEvent(new QResizeEvent(size(), size()));
	offset = 0;
	is_device_set = true;
}

void MemoryView::set_emulator(et3400emu *emu)
{
	emu_ptr = emu;
}

void MemoryView::keyPressEvent(QKeyEvent *event)
{
	if (event->key() == Qt::Key_Escape && is_editing)
	{
		stop_editing();
		return;
	}

	if (!is_editing)
	{
		if (event->key() == Qt::Key_Home)
		{
			selected_address = start;
			update_offset();
			update();
		}
		else if (event->key() == Qt::Key_End)
		{
			selected_address = end;
			update_offset();
			update();
		}
		else if (event->key() == Qt::Key_F2)
		{
			start_editing(selected_address);
		}
		else if (event->key() == Qt::Key_Up)
		{
			if (selected_address > (int)start)
			{
				if (selected_address - 8 >= (int)start)
				{
					selected_address -= 8;
					update_offset();
					if (selected_address < (int)start)
						selected_address = start;
				}

				update();
			}
		}
		else if (event->key() == Qt::Key_Down)
		{
			if (selected_address < (int)end)
			{
				if (selected_address + 8 <= end)
				{
					selected_address += 8;
					update_offset();
					if (selected_address > end)
						selected_address = end;
				}

				update();
			}
		}
		else if (event->key() == Qt::Key_Left)
		{
			if (selected_address > (int)start)
			{
				selected_address -= 1;
				update_offset();
				if (selected_address < (int)start)
					selected_address = start;

				update();
			}
		}
		else if (event->key() == Qt::Key_Right)
		{
			if (selected_address < (int)end)
			{
				selected_address += 1;
				update_offset();
				if (selected_address > (int)end)
					selected_address = end;

				update();
			}
		}
		else if (event->key() == Qt::Key_PageUp)
		{
			if (selected_address > (int)start)
			{
				selected_address -= visible_items * 8;
				update_offset();
				if (selected_address < (int)start)
					selected_address = start;

				update();
			}
		}
		else if (event->key() == Qt::Key_PageDown)
		{
			if (selected_address < (int)end)
			{
				selected_address += visible_items * 8;
				update_offset();
				if (selected_address > (int)end)
					selected_address = end;

				update();
			}
		}
	}
	else
	{
		if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter)
		{
			device->write(editing_address, editing_value);
			// memory[editing_address - start] = editing_value;
			is_editing = false;
			update();
		}
		else if (event->key() == Qt::Key_Backspace)
		{
			if (editing_nibble == 0)
			{
				editing_value = editing_value & 0x0F;
				editing_nibble = 1;
				editing_address -= 1;
				if (editing_address < (int)start)
					editing_address = start;
				selected_address = editing_address;
				editing_value = memory[editing_address - start];
			}
			else
			{
				editing_value = editing_value & 0xF0;
				editing_nibble = 0;
			}
			update();
		}
		else if (event->key() == Qt::Key_Left)
		{
			if (editing_nibble == 0)
			{
				editing_address -= 1;
				if (editing_address < (int)start)
					editing_address = start;
				else
				{
					editing_nibble = 1;
					selected_address = editing_address;
					editing_value = memory[editing_address - start];
				}
			}
			else
			{
				editing_nibble = 0;
			}
			update();
		}
		else if (event->key() == Qt::Key_Right)
		{
			if (editing_nibble == 0)
			{
				editing_nibble = 1;
			}
			else
			{
				editing_address += 1;
				if (editing_address > (int)end)
					editing_address = end;
				else
				{
					editing_nibble = 0;
					selected_address = editing_address;
					editing_value = memory[editing_address - start];
				}
			}
			update();
		}
		else
		{
			int key = event->key();
			if ((key >= Qt::Key_0 && key <= Qt::Key_9) || (key >= Qt::Key_A && key <= Qt::Key_F))
			{
				int value = (key >= Qt::Key_A) ? (key - Qt::Key_A + 10) : (key - Qt::Key_0);

				if (editing_nibble == 0)
				{
					editing_value = (editing_value & 0x0F) | (value << 4);
					editing_nibble = 1;
				}
				else
				{
					editing_value = (editing_value & 0xF0) | value;
					device->write(editing_address, editing_value);
					editing_nibble = 0;
					if (editing_address < (int)end)
					{
						editing_address += 1;
						selected_address = editing_address;
					}
					editing_value = memory[editing_address - start];
				}

				update();
			}
		}
	}
}

void MemoryView::update_offset()
{
	if (selected_address < (int)start + offset * 8)
	{
		while (selected_address < (int)start + offset * 8)
		{
			offset -= 1;
			if (offset < 0)
			{
				offset = 0;
				break;
			}
		}
		emit on_offset_change(offset);
	}
	else if (selected_address > (int)start + (offset + visible_items) * 8 - 1)
	{
		while (selected_address > (int)start + (offset + visible_items) * 8 - 1)
		{
			offset += 1;
			if (offset > max_vscroll)
			{
				offset = max_vscroll;
				break;
			}
		}
		emit on_offset_change(offset);
	}
}

void MemoryView::mousePressEvent(QMouseEvent *event)
{
	if (is_editing)
		return;
	int y = event->y();
	int x = event->x();
	int ascent = m_fm->ascent();
	int line = (y - 1) / item_height;
	int col = (x - 80) / 30;
	int address = start + (offset + line) * 8 + col;
	if (address <= end)
	{
		selected_address = address;
		update();
	}
}

void MemoryView::mouseDoubleClickEvent(QMouseEvent *event)
{
	int y = event->y();
	int x = event->x();
	int ascent = m_fm->ascent();
	int line = (y - 1) / item_height;
	int col = (x - 80) / 30;
	int address = start + (offset + line) * 8 + col;
	if (address <= end)
	{
		start_editing(address);
	}
}

void MemoryView::focusOutEvent(QFocusEvent *event)
{
	Q_UNUSED(event);
	selected_address = -1;
	stop_editing();
	update();
}

void MemoryView::start_editing(uint16_t address)
{
	LOG_DEBUG << "Start editing address" << toHex(address);
	LOG_DEBUG << "Device name:" << QString::fromStdString(device->name);
	LOG_DEBUG << "Device flags:" << device->get_flags();
	if ((device->get_flags() & DEVICE_WRITE) != DEVICE_WRITE)
		return;

	is_editing = true;
	editing_address = address;
	editing_nibble = 0;
	editing_value = memory[address - start];
	blink_counter = blink_counter_max;
	update();
}

void MemoryView::stop_editing()
{
	is_editing = false;
	blink_counter = blink_counter_max;
	editing_nibble = 0;
	editing_value = 0;
	update();
}