#include "disassembly_view.h"
#include "colors.h"
#include "../util/log.h"
#include "../common/util.h"
#include <QStringBuilder>

DisassemblyView::DisassemblyView(QWidget *parent)
	: QFrame(parent)
{
	// setMidLineWidth(0);
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setLineWidth(3);
	setFocusPolicy(Qt::StrongFocus);

	// setBackgroundRole(QPalette::Base);
	// setAutoFillBackground(true);
	QFont font("Courier", 12);
	font.setWeight(QFont::Medium);
	item_height = QFontMetrics(font).lineSpacing();

	running = true;
	start = 0;
	end = 0x100;
	offset = 0;
	is_memory_set = false;
	selected_line = -1;
	current = -1;
	breakpoint_enabled_icon = QPixmap(":/buttons/BreakpointEnabled.png");
	breakpoint_disabled_icon = QPixmap(":/buttons/BreakpointDisabled.png");
	breakpoint_available_icon = QPixmap(":/buttons/BreakpointAvailable.png");
	lines = new std::vector<DisassemblyLine>;

	m_paintTimer = new QTimer(this);
	m_paintTimer->start(100);
	connect(this->m_paintTimer, &QTimer::timeout, this, &DisassemblyView::refresh);

	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, &QFrame::customContextMenuRequested, this, &DisassemblyView::showContextMenu);

	buffer = new QPixmap;
	setMouseTracking(true);
	setLineWidth(3);
}

DisassemblyView::~DisassemblyView()
{
	LOG_DEBUG << "DisassemblyView destroy";
	m_paintTimer->stop();
	delete m_paintTimer;
	delete buffer;
	LOG_DEBUG << "DisassemblyView destroy done";
}

void DisassemblyView::wheelEvent(QWheelEvent *event)
{
	int degrees = event->delta() / 8;
	int steps = degrees / 15;

	if (event->orientation() == Qt::Vertical)
	{
		scroll(steps);
		emit onScroll(steps);
	}
	event->accept();
}

void DisassemblyView::scroll(int steps)
{
	offset -= steps;
	if (offset < 0)
		offset = 0;
	if (offset > max_vscroll - 1)
		offset = max_vscroll - 1;
	this->update();
}

void DisassemblyView::scrollTo(int value)
{
	offset = value;
	if (offset < 0)
		offset = 0;
	if (offset > max_vscroll)
		offset = max_vscroll;
	this->update();
}

void DisassemblyView::bufferDraw()
{
	if (!is_memory_set || emu_ptr == nullptr)
		return;
	QPainter painter(buffer);
	// painter.setRenderHint(QPainter::TextAntialiasing);
	//  Clear display
	painter.setBrush(QBrush(Qt::white));

	painter.fillRect(contentsRect(), painter.brush());

	QFont font("Courier", 12);
	font.setWeight(QFont::Medium);
	// font.setStyleStrategy(QFont::NoAntialias);

	painter.setFont(font);
	item_height = QFontMetrics(font).lineSpacing();
	visible_items = height() / item_height;

	int y = item_height;
	int ctr = offset;
	int x = 5;
	painter.save();

	std::vector<DisassemblyLine>::iterator line = lines->begin();

	QColor darkblue = QColor("#00018B");
	QColor black = QColor("#000000");
	QColor darkred = QColor("#8B0000");
	QColor green = QColor("#38761D");

	QBrush selected_brush = QBrush(selected_bg_color);
	QBrush selected_nofocus_brush = QBrush(QColor("#c0c0c0"));
	QBrush breakpoint_brush = QBrush(QColor("#8B0000"));
	QBrush current_brush = QBrush(QColor("#fff181"));
	QBrush current_selected_brush = QBrush(QColor("#81ffaf"));

	QColor white = QColor("#FFFFFF");

	// emu_ptr->breakpoints->lock();

	int linesSize = lines->size();

	QBrush background_brush;

	while (ctr < linesSize && ctr < offset + visible_items && ctr - visible_items + 1 < max_vscroll)
	{
		// Default colors
		QColor address_color = darkblue;
		QColor opcode_color = black;
		QColor instruction_color = darkblue;
		QColor operand_color = darkred;

		bool is_comment = line[ctr].type == DisassemblyType::Comment;
		bool is_data = line[ctr].type == DisassemblyType::Data;
		bool is_selected = selected_line > -1 && selected_line < linesSize && !is_comment && line[ctr].address == line[selected_line].address;
		bool is_current = current > -1 && current < linesSize && !is_comment && line[ctr].address == line[current].address;

		Breakpoint breakpoint;

		bool has_breakpoint = emu_ptr->breakpoints->tryGetBreakpoint(line[ctr].address, breakpoint) && !breakpoint.is_hidden && !is_comment;

		painter.save();

		if (has_breakpoint)
		{
			if (breakpoint.is_enabled)
				painter.drawPixmap(2, y - 13, 16, 16, breakpoint_enabled_icon);
			else
				painter.drawPixmap(2, y - 13, 16, 16, breakpoint_disabled_icon);
		}
		else if (ctr == hover_row && !is_comment)
		{
			painter.drawPixmap(2, y - 13, 16, 16, breakpoint_available_icon);
		}

		const int yOffset = 13;
		const int xOffset = 20;
		const int breakpoint_width = 22;

		background_brush = Qt::NoBrush;

		if (is_current && is_selected)
		{
			background_brush = current_selected_brush;
		}
		else if (is_current)
		{
			background_brush = current_brush;
		}
		else if (is_selected)
		{
			background_brush = selected_brush;
			// if (hasFocus)
			// {
			// 	background_brush = selected_brush;
			// }
			// else
			// {
			// 	background_brush = selected_nofocus_brush;
			// }
		}
		else if (has_breakpoint)
		{
			background_brush = breakpoint_brush;
		}

		if (background_brush.style() != Qt::NoBrush)
		{
			painter.fillRect(xOffset, y - yOffset, width() - breakpoint_width, item_height - 2, background_brush);
		}

		painter.restore();

		if (is_comment)
		{
			opcode_color = green;
		}

		if (is_current)
		{
			address_color = black;
			opcode_color = black;
			instruction_color = black;
			operand_color = black;
		}
		else if (is_selected)
		{
			address_color = selected_fg_color;
			opcode_color = selected_fg_color;
			instruction_color = selected_fg_color;
			operand_color = selected_fg_color;
		}
		else if (has_breakpoint)
		{
			address_color = white;
			opcode_color = white;
			instruction_color = white;
			operand_color = white;
		}

		if (is_comment)
		{
			// painter.setPen(address_color);
			// painter.drawText(20, y, QString("$%1:").arg(line[ctr].address, 4, 16, QChar('0')).toUpper());

			// painter.setPen(opcode_color);
			// painter.drawText(90, y, line[ctr].opcodes);

			painter.setPen(opcode_color);
			painter.drawText(20, y, line[ctr].opcodes);
		}
		else
		{
			painter.setPen(address_color);
			painter.drawText(20, y, toHex(line[ctr].address) % ":");

			painter.setPen(opcode_color);

			if (is_data)
			{
				// live view of data
				// int i = 0;
				// int ptr = line[ctr].address - start;
				// QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
				// for (; i < line[ctr].bytes; i++)
				// {
				// 	data = data.arg(memory[ptr + i], 2, 16, QChar('0')).toUpper();
				// }
				// for (; i < 8; i++)
				// {
				// 	data = data.arg(" ");
				// }

				// painter.drawText(90, y, data);
				painter.drawText(90, y, line[ctr].opcodes);

				// while (address < line[ctr].map->end)
				// {
				// }
			}
			else
			{
				painter.drawText(90, y, line[ctr].opcodes);
			}

			if (line[ctr].type == DisassemblyType::Assembly)
			{
				painter.setPen(instruction_color);
				painter.drawText(200, y, line[ctr].instruction);
				painter.setPen(operand_color);
				painter.drawText(260, y, line[ctr].operand);
			}
		}

		ctr++;
		y += item_height;
	}

	// emu_ptr->breakpoints->unlock();

	painter.restore();
}

void DisassemblyView::resizeEvent(QResizeEvent *event)
{
	QSize size = event->size();
	buffer = new QPixmap(size);

	if (is_memory_set)
	{
		visible_items = size.height() / item_height;
		int x = lines->size() - visible_items + 2;
		max_vscroll = x > 0 ? x : 0;
		emit onSize(max_vscroll);

		redraw();
	}
}

DisassemblyLine DisassemblyView::findLine(offs_t address)
{
	std::vector<DisassemblyLine>::iterator it = lines->begin();
	while (it != lines->end())
	{
		if ((*it).address == address)
		{
			return (*it);
		}
		it++;
	}
	return DisassemblyLine{0, DisassemblyType::Empty, NULL, NULL, NULL};
}

void DisassemblyView::addOrRemoveBreakpoint(int line_number)
{
	if (line_number > -1)
	{
		DisassemblyLine *line = &lines->at(line_number);
		if (line->type == DisassemblyType::Assembly)
		{
			emit onAddorRemoveBreakpoint(line->address);
		}
	}
}

void DisassemblyView::keyPressEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Up:
		adjustSelected(-1);
		break;
	case Qt::Key_Down:
		adjustSelected(1);
		break;
	case Qt::Key_PageUp:
		adjustSelected(-visible_items);
		break;
	case Qt::Key_PageDown:
		adjustSelected(visible_items);
		break;
	case Qt::Key_F9:
		addOrRemoveBreakpoint(selected_line);
		break;
	default:
		event->ignore();
		break;
	}
}

void DisassemblyView::adjustSelected(int direction)
{
	if (selected_line > visible_items + offset - 1)
	{
		selected_line = visible_items + offset - 1;
		return;
	}

	if (selected_line < offset)
	{
		selected_line = offset;
		return;
	}

	int oldselected = selected_line;
	int newSelected = selected_line + direction;

	if (newSelected < 0)
		newSelected = 0;

	if (newSelected > lines->size() - 1)
		newSelected = lines->size() - 1;

	bool success = true;

	while (lines->at(newSelected).type == DisassemblyType::Comment)
	{
		newSelected += direction;

		if (newSelected < 0)
		{
			offset = 0;
			newSelected = 0;
			success = false;
			break;
		}

		if (newSelected > lines->size() - 1)
		{
			offset = max_vscroll;
			newSelected = lines->size() - 1;
			success = false;
			break;
		}
	}

	if (success)
	{
		selected_line = newSelected;
	}
	else
	{
		while (lines->at(newSelected).type == DisassemblyType::Comment)
		{
			newSelected += 1;
		}
		selected_line = newSelected;
	}

	// if (newSelected < 0)
	//	newSelected = 0;

	// if (newSelected > lines->size())
	//	newSelected = lines->size();

	if (selected_line > visible_items + offset - 1)
	{
		offset += newSelected - oldselected;
		if (offset > max_vscroll)
		{
			offset = max_vscroll;
		}
		onOffsetUpdated(offset);
	}

	if (selected_line < offset)
	{
		offset += newSelected - oldselected;
		if (offset < 0)
		{
			offset = 0;
		}
		onOffsetUpdated(offset);
	}
}

void DisassemblyView::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	if (is_memory_set)
	{
		bufferDraw();
		painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
		painter.end();
	}
	QFrame::paintEvent(event);
}

void DisassemblyView::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::MouseButton::LeftButton)
	{
		int y = event->pos().y();
		int x = event->pos().x();
		int line = offset + (y / item_height);
		if (x < 20)
		{
			addOrRemoveBreakpoint(line);
		}
		else
		{
			if (selected_line == line)
			{
				selected_line = -1;
			}
			else
			{
				selected_line = line;
			}
		}
		setFocus();
	}
	else if (event->button() == Qt::MouseButton::RightButton)
	{
		int y = event->pos().y();
		int x = event->pos().x();
		int line = offset + (y / item_height);
		if (x > 20)
		{
			selected_line = line;
		}
		else
		{
			selected_line = -1;
		}
		setFocus();
	}
}

void DisassemblyView::mouseMoveEvent(QMouseEvent *event)
{
	if (event->pos().x() < 20)
		hover_row = offset + (event->pos().y() / item_height);
	else
		hover_row = -1;
}

void DisassemblyView::leaveEvent(QEvent *event)
{
	hover_row = -1;
	QFrame::leaveEvent(event);
}

void DisassemblyView::focusOutEvent(QFocusEvent *event)
{
	selected_line = -1;
	update();
}

void DisassemblyView::rebuild()
{
	DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());
	this->update();
	resizeEvent(new QResizeEvent(size(), size()));
}

void DisassemblyView::redraw()
{
	// DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

	// int x = lines->size() - visible_items + 1;
	// max_vscroll = x > 0 ? x : 0;
	// emit on_size(max_vscroll);
	// is_memory_set = true;

	this->update();
}

void DisassemblyView::clearCurrent()
{
	current = -1;
}

void DisassemblyView::clearSelected()
{
	selected_line = -1;
}

void DisassemblyView::ensureVisible(offs_t address)
{
	if (address > offset + visible_items)
	{
		offset = address - visible_items / 2;
		if (offset > max_vscroll)
			offset = max_vscroll;
	}
	else if (address == offset + visible_items)
	{
		offset++;
		if (offset > max_vscroll)
			offset = max_vscroll;
	}
	else if (address < offset)
	{
		offset = address - visible_items / 2;
		if (offset < 0)
			offset = 0;
	}
}

void DisassemblyView::setSelected(offs_t address)
{
	int ctr = -1;
	bool found = false;
	std::vector<DisassemblyLine>::iterator line = lines->begin();
	while (line != lines->end())
	{
		if (line->address == address)
		{
			selected_line = ctr + 1;
			found = true;
			break;
		}
		ctr++;
		line++;
	}

	if (found)
		ensureVisible(selected_line);
}

void DisassemblyView::setCurrent(offs_t address)
{
	int ctr = -1;
	bool found = false;
	std::vector<DisassemblyLine>::iterator line = lines->begin();
	while (line != lines->end())
	{
		if (line->address == address)
		{
			current = ctr + 1;
			found = true;
			break;
		}
		ctr++;
		line++;
	}

	if (found)
		ensureVisible(current);
}

void DisassemblyView::setAutoRefresh(bool value)
{
	auto_refresh = value;
}

void DisassemblyView::refresh()
{
	if (auto_refresh)
	{
		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());
		resizeEvent(new QResizeEvent(size(), size()));
	}
	redraw();
}

void DisassemblyView::set_range(offs_t start, offs_t end, uint8_t *memory)
{
	this->start = start;
	this->end = end;
	this->memory = memory;

	visible_items = height() / item_height;

	DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

	offset = 0;
	is_memory_set = true;
	resizeEvent(new QResizeEvent(size(), size()));
}

void DisassemblyView::setEmulator(et3400emu *emu)
{
	emu_ptr = emu;
}

void DisassemblyView::clearLabels()
{
	emu_ptr->labels->clearLabels();
	rebuild();
}

void DisassemblyView::addLabel()
{
	LabelDialog labelDialog;

	offs_t address = 0;
	if (selected_line > -1)
	{
		address = lines->at(selected_line).address;
	}

	labelDialog.setLabel(LabelInfo{QString("New Label"), LabelType::COMMENT, address, address}, LabelDialogMode::Add);

	QDialog::DialogCode result = (QDialog::DialogCode)labelDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		LabelInfo label = labelDialog.getLabel();

		emu_ptr->labels->addLabel(Label{label.start, label.end, label.type, label.text});

		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

		clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::addLabel(DisassemblyLine *line)
{
	LabelDialog labelDialog;
	labelDialog.setLabel(LabelInfo{QString("New Label"), LabelType::DATA, line->address, line->address}, LabelDialogMode::Add);

	QDialog::DialogCode result = (QDialog::DialogCode)labelDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		LabelInfo label = labelDialog.getLabel();

		emu_ptr->labels->addLabel(Label{label.start, label.end, label.type, label.text});

		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

		clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::editLabel(DisassemblyLine *line)
{
	LabelDialog labelDialog;
	labelDialog.setLabel(LabelInfo{line->label->comment, line->label->type, line->label->start, line->label->end}, LabelDialogMode::Edit);

	QDialog::DialogCode result = (QDialog::DialogCode)labelDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		LabelInfo label = labelDialog.getLabel();

		emu_ptr->labels->removeLabel(line->label);

		emu_ptr->labels->addLabel(Label{label.start, label.end, label.type, label.text});

		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

		clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::removeLabel(DisassemblyLine *line)
{
	RemoveLabelDialog remove_label;
	remove_label.setLabel(QString(line->label->comment));

	QDialog::DialogCode result = (QDialog::DialogCode)remove_label.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		emu_ptr->labels->removeLabel(line->label);

		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

		clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::showContextMenu(const QPoint &pos)
{
	// int line_number = offset + (pos.y() / item_height);

	if (selected_line > -1)
	{
		DisassemblyLine *line = &lines->at(selected_line);

		QMenu contextMenu(tr("Context menu"), this);
		QAction addLabelAction("Add label", this);
		QAction editLabelAction("Edit label", this);
		QAction removeLabelAction("Remove label", this);

		QAction addBreakpointAction("Add breakpoint", this);
		QAction removeBreakpointAction("Remove breakpoint", this);
		QAction disableBreakpointAction("Disable breakpoint", this);
		QAction enableBreakpointAction("Enable breakpoint", this);

		bool hasAction = false;

		if (line->label == nullptr)
		{
			connect(&addLabelAction, &QAction::triggered, this, [this, line]
					{ addLabel(line); });
			contextMenu.addAction(&addLabelAction);
			hasAction = true;
		}
		else
		{
			connect(&editLabelAction, &QAction::triggered, this, [this, line]
					{ editLabel(line); });
			contextMenu.addAction(&editLabelAction);
			connect(&removeLabelAction, &QAction::triggered, this, [this, line]
					{ removeLabel(line); });
			contextMenu.addAction(&removeLabelAction);
			hasAction = true;
		}

		if (hasAction)
		{
			contextMenu.addSeparator();
		}

		Breakpoint breakpoint;

		if (emu_ptr->breakpoints->tryGetBreakpoint(line->address, breakpoint))
		{
			if (breakpoint.is_enabled)
			{
				connect(&disableBreakpointAction, &QAction::triggered, this, [this, line]
						{ emu_ptr->breakpoints->disableBreakpoint(line->address); redraw(); emit onBreakpointChanged(); });
				contextMenu.addAction(&disableBreakpointAction);
			}
			else
			{
				connect(&enableBreakpointAction, &QAction::triggered, this, [this, line]
						{ emu_ptr->breakpoints->enableBreakpoint(line->address); redraw(); emit onBreakpointChanged(); });
				contextMenu.addAction(&enableBreakpointAction);
			}

			connect(&removeBreakpointAction, &QAction::triggered, this, [this, line]
					{ emu_ptr->breakpoints->removeBreakpoint(line->address); });
			contextMenu.addAction(&removeBreakpointAction);
		}
		else
		{
			connect(&addBreakpointAction, &QAction::triggered, this, [this, line]
					{ emu_ptr->breakpoints->addBreakpoint(line->address); });
			contextMenu.addAction(&addBreakpointAction);
		}

		contextMenu.exec(mapToGlobal(pos));
	}

	////connect(&action1, &QAction::triggered(), this, SLOT(removeDataPoint()));
	// QAction action3("Remove Breakpoint", this);
	// QAction action4("Add Breakpoint", this);

	// if (line->type == DisassemblyType::Assembly)
	//{
	//	if (line->has_breakpoint)
	//	{
	//		//connect(&action1, &QAction::triggered(), this, SLOT(removeDataPoint()));
	//		contextMenu.addAction(&action3);
	//	}
	//	else
	//	{
	//		//connect(&action1, &QAction::triggered(), this, SLOT(removeDataPoint()));
	//		contextMenu.addAction(&action4);
	//	}
	// }
}