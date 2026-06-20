#include "disassembly_view.h"
#include "colors.h"
#include "../util/log.h"
#include "../common/util.h"
#include <QStringBuilder>

DisassemblyView::DisassemblyView(QWidget *parent)
	: QFrame(parent)
{
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

	setupUI(parent);
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

	// scrollbar->setValue(scrollbar->value() - steps);

	event->accept();
}

void DisassemblyView::scroll(int steps)
{
	offset -= steps;
	if (offset < 0)
		offset = 0;
	if (offset > max_vscroll - 1)
		offset = max_vscroll - 1;
	scrollbar->setValue(offset);
	this->update();
}

void DisassemblyView::scrollTo(int value)
{
	offset = value;
	if (offset < 0)
		offset = 0;
	if (offset > max_vscroll)
		offset = max_vscroll;
	scrollbar->setValue(offset);
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
	QBrush selected_nofocus_brush = QBrush(selected_bg_nofocus_color);
	QBrush breakpoint_brush = QBrush(QColor("#8B0000"));
	QBrush current_brush = QBrush(QColor("#fff181"));
	QBrush current_selected_brush = QBrush(QColor("#81ffaf"));

	QColor white = QColor("#FFFFFF");

	bool hasFocus = this->hasFocus();

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

		if (is_current && is_selected && hasFocus)
		{
			background_brush = current_selected_brush;
		}
		else if (is_current)
		{
			background_brush = current_brush;
		}
		else if (is_selected)
		{
			if (hasFocus)
			{
				background_brush = selected_brush;
			}
			else
			{
				background_brush = selected_nofocus_brush;
			}
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
		else if (is_selected && hasFocus)
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

		scrollbar->setMinimum(0);
		scrollbar->setMaximum(max_vscroll - 1);

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
	case Qt::Key_Space:
		if (selected_line > -1)
			selected_line = -1;
		else
			selected_line = last_selected_line;
		update();
		break;
	case Qt::Key_Up:
		adjustSelected(-1);
		scrollIntoView();
		break;
	case Qt::Key_Down:
		adjustSelected(1);
		scrollIntoView();
		break;
	case Qt::Key_PageUp:
		adjustSelected(-visible_items);
		scrollIntoView();
		break;
	case Qt::Key_PageDown:
		adjustSelected(visible_items);
		scrollIntoView();
		break;
	case Qt::Key_Home:
		setSelected(start);
		scrollIntoView();
		break;
	case Qt::Key_End:
		setSelected(end);
		scrollIntoView();
		break;
	case Qt::Key_F2:
		if (selected_line > -1)
		{
			try
			{
				DisassemblyLine *line = &lines->at(selected_line);
				if (line->label != nullptr)
				{
					editLabel(line);
				}
			}
			catch (const std::out_of_range &e)
			{
			}
		}
		break;
	case Qt::Key_F9:
		addOrRemoveBreakpoint(selected_line);
		break;
	default:
		event->ignore();
		break;
	}
}

void DisassemblyView::scrollIntoView()
{
	// if (selected_line > visible_items + offset - 1)
	// {
	// 	selected_line = visible_items + offset - 1;
	// 	return;
	// }

	// if (selected_line < offset)
	// {
	// 	selected_line = offset;
	// 	return;
	// }

	if (selected_line > visible_items + offset - 1)
	{
		while (selected_line > visible_items + offset - 1)
		{
			offset++;
			if (offset > max_vscroll)
			{
				offset = max_vscroll;
				break;
			}
		}
		emit onOffsetUpdated(offset);
		scrollbar->setValue(offset);
	}

	if (selected_line < offset)
	{
		while (selected_line < offset)
		{
			offset--;
			if (offset < 0)
			{
				offset = 0;
				break;
			}
		}
		emit onOffsetUpdated(offset);
		scrollbar->setValue(offset);
	}
}

void DisassemblyView::adjustSelected(int direction)
{

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

	last_selected_line = selected_line;
}

void DisassemblyView::paintEvent(QPaintEvent *event)
{
	QFrame::paintEvent(event);
}

bool DisassemblyView::eventFilter(QObject *obj, QEvent *event)
{
	if (obj == frame)
	{
		switch (event->type())
		{
		case QEvent::Paint:
			if (is_memory_set)
			{
				bufferDraw();
				QPainter painter(frame);
				painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
			}
			return false; // let QFrame draw its border on top
		case QEvent::MouseButtonPress:
			mousePressEvent(static_cast<QMouseEvent *>(event));
			return true;
		case QEvent::MouseMove:
			mouseMoveEvent(static_cast<QMouseEvent *>(event));
			return true;
		case QEvent::Leave:
			leaveEvent(event);
			return true;
		case QEvent::Wheel:
			wheelEvent(static_cast<QWheelEvent *>(event));
			return true;
		default:
			break;
		}
	}
	return QObject::eventFilter(obj, event);
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
			if (selected_line == line && !gained_focus)
			{
				selected_line = -1;
			}
			else
			{
				selected_line = line;
				last_selected_line = selected_line;
			}
		}
		gained_focus = false;
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
			last_selected_line = selected_line;
		}
		else
		{
			selected_line = -1;
		}
		gained_focus = false;
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

void DisassemblyView::focusInEvent(QFocusEvent *event)
{
	LOG_DEBUG << "Reason: " << event->reason();

	Qt::FocusReason reason = event->reason();

	if (reason == Qt::MouseFocusReason)
		gained_focus = true;

	if (reason == Qt::ActiveWindowFocusReason || reason == Qt::PopupFocusReason || reason == Qt::MouseFocusReason)
		return;

	if (selected_line == -1)
	{
		selected_line = 0;
		last_selected_line = selected_line;
	}
	scrollIntoView();
	update();
}

void DisassemblyView::focusOutEvent(QFocusEvent *event)
{
	gained_focus = false;
	update();
}

void DisassemblyView::rebuild()
{
	DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());
	resizeEvent(new QResizeEvent(size(), size()));
	this->update();
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

	scrollbar->setValue(offset);
}

int DisassemblyView::getSelectedAddress()
{
	if (selected_line == -1)
		return -1;

	int ctr = 0;
	std::vector<DisassemblyLine>::iterator line = lines->begin();
	while (line != lines->end())
	{
		if (selected_line == ctr)
			return line->address;
		ctr++;
		line++;
	}

	return -1;
}

void DisassemblyView::setSelected(offs_t address)
{
	int ctr = 0;
	bool found = false;
	std::vector<DisassemblyLine>::iterator line = lines->begin();
	while (line != lines->end())
	{
		if (line->type != DisassemblyType::Comment)
		{
			if ((address >= line->address) && (address <= (line->address + line->bytes - 1)))
			{
				selected_line = ctr;
				last_selected_line = selected_line;
				found = true;
				break;
			}
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
	selected_line = -1;
	is_memory_set = true;
	resizeEvent(new QResizeEvent(size(), size()));
	scrollbar->setValue(offset);
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

		// clearSelected();

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

		// clearSelected();

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

		// clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::showContextMenu(const QPoint &pos)
{
	// int line_number = offset + (pos.y() / item_height);
	QMenu contextMenu(tr("Context menu"), this);

	QAction addLabelAction("Add label\tIns", this);
	QAction editLabelAction("Edit label\tF2", this);
	QAction removeLabelAction("Remove label\tDel", this);

	QAction addBreakpointAction("Add breakpoint\tF9", this);
	QAction removeBreakpointAction("Remove breakpoint\tF9", this);
	QAction disableBreakpointAction("Disable breakpoint\tCtrl+F9", this);
	QAction enableBreakpointAction("Enable breakpoint\tCtrl+F9", this);

	QAction showInMemoryAction("Show in &Memory", this);

	if (selected_line > -1)
	{
		DisassemblyLine *line = &lines->at(selected_line);

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
			if (line->label == nullptr || line->label->type != DATA)
			{
				connect(&addBreakpointAction, &QAction::triggered, this, [this, line]
						{ emu_ptr->breakpoints->addBreakpoint(line->address); });
				contextMenu.addAction(&addBreakpointAction);
			}
		}

		contextMenu.addSeparator();

		connect(&showInMemoryAction, &QAction::triggered, this, [this, line]()
				{ emit onShowInMemory(line->address); });
		contextMenu.addAction(&showInMemoryAction);

		contextMenu.addSeparator();
	}

	QAction refreshAction("&Refresh\tCtrl+R", this);
	connect(&refreshAction, &QAction::triggered, this, [this]()
			{ refresh(); });
	contextMenu.addAction(&refreshAction);

	QAction autoRefreshAction("&Auto Refresh", this);
	autoRefreshAction.setCheckable(true);
	autoRefreshAction.setChecked(auto_refresh);
	connect(&autoRefreshAction, &QAction::toggled, this, [this](bool checked)
			{ setAutoRefresh(checked); emit onAutorefreshChanged(checked); });
	contextMenu.addAction(&autoRefreshAction);

	contextMenu.exec(mapToGlobal(pos));
}