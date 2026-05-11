#include "memory_view.h"
#include "../util/log.h"

MemoryView::MemoryView(QWidget *parent)
	: QFrame(parent)
{
	// setMidLineWidth(0);
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setLineWidth(3);

	// setBackgroundRole(QPalette::Base);
	// setAutoFillBackground(true);
	running = true;
	start = 0;
	end = 0x100;
	offset = 0;
	is_memory_set = false;
	;

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

void MemoryView::bufferDraw()
{
	QPainter painter(buffer);
	// painter.setRenderHint(QPainter::TextAntialiasing);
	//  Clear display
	painter.setBrush(QBrush(Qt::white));
	painter.fillRect(contentsRect(), painter.brush());

	QFont font("Courier", 12);
	font.setWeight(QFont::Medium);
	// font.setStyleStrategy(QFont::NoAntialias);

	painter.setFont(font);
	QFontMetrics fm(font);
	item_height = fm.lineSpacing();
	visible_items = height() / item_height;

	int y = item_height;

	QColor darkblue = QColor("#00018B");
	QColor black = QColor("#000000");
	QColor darkred = QColor("#8B0000");
	QColor green = QColor("#7db700");
	QColor heat_colors[16];

	{
		QColor cold = QColor("#ffffff");
		QColor hot = QColor("#d8ce44");
		for (int i = 0; i < 16; ++i)
		{
			float t = i / 15.0f;
			heat_colors[i] = QColor(
				cold.red() + t * (hot.red() - cold.red()),
				cold.green() + t * (hot.green() - cold.green()),
				cold.blue() + t * (hot.blue() - cold.blue()));
		}
	}

	// QString addr = QString("$%1:");
	// QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");

	QChar filler = QLatin1Char('0');

	painter.save();

	// Snapshot live memory once — all reads below use shadow, not the live buffer
	memcpy(shadow_memory, memory, end - start + 1);

	for (int i = 0; i < end - start + 1; i++)
	{
		if (last_memory[i] - shadow_memory[i] != 0)
		{
			heat_map[i] = 255;
		}
		else
		{
			if (heat_map[i] > 0)
			{
				heat_map[i] = heat_map[i] > 12 ? heat_map[i] - 12 : 0;
			}
		}
	}

	for (int line = offset; line < offset + visible_items && (start + (line * 8) < end); line++)
	{
		int address = start + line * 8;

		painter.setPen(darkblue);
		painter.drawText(5, y, QString("$%1:").arg(address, 4, 16, filler).toUpper());

		// QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
		int i = 0;

		while (address + i <= end && i < 8)
		{

			int heat_idx = address + i - start;
			int text_x = 80 + i * 30;
			painter.setPen(darkred);
			if (heat_map[heat_idx] > 0)
			{
				painter.fillRect(text_x - 2, y - fm.ascent(), 28, fm.ascent() + fm.descent(),
								 heat_colors[heat_map[heat_idx] / 16]);
			}

			painter.drawText(text_x, y, QString("%1").arg(shadow_memory[address + i - start], 2, 16, filler).toUpper());

			// data = data.arg(memory[address + i - start], 2, 16, filler).toUpper();
			i++;
		}

		while (i < 8)
		{
			painter.setPen(darkred);
			painter.drawText(80 + i * 30, y, QString("%1").arg(0, 2, 16, filler).toUpper());
			// data = data.arg(0, 2, 16, filler);
			i++;
		}

		y += item_height;
	}

	memcpy(last_memory, shadow_memory, end - start + 1);

	painter.restore();
}

void MemoryView::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();
	buffer = new QPixmap(size);
	visible_items = size.height() / item_height;
	int x = (end - start) / 8 - visible_items + 1;
	max_vscroll = x > 0 ? x : 0;
	emit on_size(max_vscroll);
}

void MemoryView::paintEvent(QPaintEvent * /* event */)
{
	QPainter painter(this);
	if (is_memory_set)
	{
		bufferDraw();
		painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
		painter.end();
	}
}

// void MemoryView::set_range()
// {
// }

void MemoryView::redraw()
{
	this->update();
}

void MemoryView::update_display()
{
	// action->trigger();
}

void MemoryView::set_range(offs_t start, offs_t end, uint8_t *memory)
{
	this->start = start;
	this->end = end;
	this->memory = memory;

	if (this->last_memory != nullptr)
		free(this->last_memory);

	if (this->heat_map != nullptr)
		free(this->heat_map);

	if (this->shadow_memory != nullptr)
		free(this->shadow_memory);

	this->last_memory    = (uint8_t *)calloc(end - start + 1, 1);
	this->shadow_memory  = (uint8_t *)calloc(end - start + 1, 1);
	this->heat_map       = (uint8_t *)calloc(end - start + 1, 1);

	resizeEvent(new QResizeEvent(size(), size()));
	offset = 0;
	is_memory_set = true;
}

void MemoryView::set_emulator(et3400emu *emu)
{
	emu_ptr = emu;
}