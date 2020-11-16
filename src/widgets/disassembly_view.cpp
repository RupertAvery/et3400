#include <QDebug>
#include "disassembly_view.h"

DisassemblyView::DisassemblyView(QWidget* parent)
	: QFrame(parent)
{
	//setMidLineWidth(0);
	setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
	setLineWidth(3);

	// setBackgroundRole(QPalette::Base);
	// setAutoFillBackground(true);
	running = true;
	start = 0;
	end = 0x100;
	offset = 0;
	is_memory_set = false;
	selected = -1;
	current = -1;
	breakpoint_icon = QPixmap(":/buttons/BreakpointEnable_16x.png");
	lines = new std::vector<DisassemblyLine>;

	m_paintTimer = new QTimer(this);
	m_paintTimer->start(36); // 38ms, or every 1/30th of a second
	connect(this->m_paintTimer, &QTimer::timeout, this, &DisassemblyView::redraw);

	setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, &QFrame::customContextMenuRequested, this, &DisassemblyView::showContextMenu);

	buffer = new QPixmap;
	// action = new QAction;
	// connect(action, &QAction::triggered, this, &Display::redraw);

	// this->setFixedSize(QSize(320, 85));
	setLineWidth(3);
}

DisassemblyView::~DisassemblyView()
{
	qDebug() << "DisassemblyView view destroy";
	m_paintTimer->stop();
	delete m_paintTimer;
	delete buffer;
	qDebug() << "DisassemblyView view destroy done";
}

void DisassemblyView::wheelEvent(QWheelEvent* event)
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
	if (!is_memory_set)
		return;
	QPainter painter(buffer);
	//painter.setRenderHint(QPainter::TextAntialiasing);
	// Clear display
	painter.setBrush(QBrush(Qt::white));

	painter.fillRect(contentsRect(), painter.brush());

	QFont font("Courier", 12);
	font.setWeight(QFont::Medium);
	//font.setStyleStrategy(QFont::NoAntialias);

	painter.setFont(font);

	int y = 15;
	int ctr = offset;
	int x = 5;
	painter.save();

	std::vector<DisassemblyLine>::iterator line = lines->begin();

	QColor darkblue = QColor("#00018B");
	QColor black = QColor("#000000");
	QColor darkred = QColor("#8B0000");
	QColor green = QColor("#38761D");

	QBrush selected_brush = QBrush(QColor("#0000FF"));
	QBrush breakpoint_brush = QBrush(QColor("#8B0000"));
	QBrush current_brush = QBrush(QColor("#fff181"));
	QBrush current_selected_brush = QBrush(QColor("#81ffaf"));

	QColor white = QColor("#FFFFFF");

	//emu_ptr->breakpoints->lock();

	while (ctr < offset + visible_items && ctr - visible_items + 1 < max_vscroll)
	{
		// Default colors
		QColor address_color = darkblue;
		QColor opcode_color = black;
		QColor insgtruction_color = darkblue;
		QColor operand_color = darkred;

		bool is_comment = line[ctr].type == DisassemblyType::Comment;
		bool is_data = line[ctr].type == DisassemblyType::Data;
		bool is_selected = selected > -1 && !is_comment && line[ctr].address == line[selected].address;
		bool is_current = current > -1 && !is_comment && line[ctr].address == line[current].address;
		bool has_breakpoint = emu_ptr->has_breakpoint(line[ctr].address) && !is_comment;

		painter.save();

		if (has_breakpoint)
		{
			painter.drawPixmap(2, y - 13, 16, 16, breakpoint_icon);
		}

		if (is_current && is_selected)
		{
			painter.fillRect(20, y - 14, width() - 22, item_height - 2, current_selected_brush);
		}
		else if (is_current)
		{
			painter.fillRect(20, y - 14, width() - 22, item_height - 2, current_brush);
		}
		else if (is_selected)
		{
			painter.fillRect(20, y - 14, width() - 22, item_height - 2, selected_brush);
		}
		else if (has_breakpoint)
		{
			painter.fillRect(20, y - 14, width() - 22, item_height - 2, breakpoint_brush);
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
			insgtruction_color = black;
			operand_color = black;
		}
		else if (is_selected)
		{
			address_color = white;
			opcode_color = white;
			insgtruction_color = white;
			operand_color = white;
		}
		else if (has_breakpoint)
		{
			address_color = white;
			opcode_color = white;
			insgtruction_color = white;
			operand_color = white;
		}

		if (is_comment)
		{
			//painter.setPen(address_color);
			//painter.drawText(20, y, QString("$%1:").arg(line[ctr].address, 4, 16, QChar('0')).toUpper());

			//painter.setPen(opcode_color);
			//painter.drawText(90, y, line[ctr].opcodes);

			painter.setPen(opcode_color);
			painter.drawText(20, y, line[ctr].opcodes);
		}
		else
		{
			painter.setPen(address_color);
			painter.drawText(20, y, QString("$%1:").arg(line[ctr].address, 4, 16, QChar('0')).toUpper());

			painter.setPen(opcode_color);

			if (is_data)
			{
				// live view of data
				int i = 0;
				int ptr = line[ctr].address - start;
				QString data = QString("%1 %2 %3 %4 %5 %6 %7 %8");
				for (; i < line[ctr].bytes; i++)
				{
					data = data.arg(memory[ptr + i], 2, 16, QChar('0')).toUpper();
				}
				for (; i < 8; i++)
				{
					data = data.arg(" ");
				}
				painter.drawText(90, y, data);

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
				painter.setPen(insgtruction_color);
				painter.drawText(200, y, line[ctr].instruction);
				painter.setPen(operand_color);
				painter.drawText(260, y, line[ctr].operand);
			}
		}

		ctr++;
		y += item_height;
	}

	//emu_ptr->breakpoints->unlock();

	painter.restore();
}

void DisassemblyView::resizeEvent(QResizeEvent* event)
{
	QSize size = event->size();
	buffer = new QPixmap(size);

	if (is_memory_set)
	{
		visible_items = size.height() / item_height;
		int x = lines->size() - visible_items + 1;
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
	return DisassemblyLine{ 0, DisassemblyType::Empty, NULL, NULL, NULL };
}

void DisassemblyView::addOrRemoveBreakpoint(int line_number)
{
	if (line_number > -1)
	{
		DisassemblyLine* line = &lines->at(line_number);
		if (line->type == DisassemblyType::Assembly)
		{
			emit onAddorRemoveBreakpoint(line->address);
		}
	}
}

void DisassemblyView::keyPressEvent(QKeyEvent* event)
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
		addOrRemoveBreakpoint(selected);
		break;
	default:
		event->ignore();
		break;
	}
}

void DisassemblyView::adjustSelected(int direction)
{
	if (selected > visible_items + offset - 1)
	{
		selected = visible_items + offset - 1;
		return;
	}

	if (selected < offset)
	{
		selected = offset;
		return;
	}

	int oldselected = selected;
	int newSelected = selected + direction;

	if (newSelected < 0)
		newSelected = 0;

	if (newSelected > lines->size() - 1)
		newSelected = lines->size() - 1;

	bool success = true;

	while (lines->at(newSelected).type == DisassemblyType::Comment) {
		newSelected += direction;

		if (newSelected < 0)
		{
			offset = 0;
			newSelected = 0;
			success = false;
			break;
		}

		if (newSelected > lines->size()-1)
		{
			offset = max_vscroll;
			newSelected = lines->size() - 1;
			success = false;
			break;
		}
	}

	if (success) {
		selected = newSelected;
	}
	else
	{
		while (lines->at(newSelected).type == DisassemblyType::Comment) {
			newSelected += 1;
		}
		selected = newSelected;
	}

	//if (newSelected < 0)
	//	newSelected = 0;

	//if (newSelected > lines->size())
	//	newSelected = lines->size();

	if (selected > visible_items + offset - 1)
	{
		offset += newSelected - oldselected;
		if (offset > max_vscroll)
		{
			offset = max_vscroll;
		}
		onOffsetUpdated(offset);
	}

	if (selected < offset)
	{
		offset += newSelected - oldselected;
		if (offset < 0)
		{
			offset = 0;
		}
		onOffsetUpdated(offset);
	}
}

void DisassemblyView::paintEvent(QPaintEvent* /* event */)
{
	QPainter painter(this);
	if (is_memory_set)
	{
		bufferDraw();
		painter.drawPixmap(0, 0, *buffer, 0, 0, 0, 0);
		painter.end();
	}
}

void DisassemblyView::mousePressEvent(QMouseEvent* event)
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
			selected = line;
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
			selected = line;
		}
		else
		{
			selected = -1;
		}
		setFocus();
	}
}

void DisassemblyView::redraw()
{
	//  DisassemblyBuilder::build(lines, start, end, memory, maps);

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
	selected = -1;
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
	std::vector<DisassemblyLine>::iterator line = lines->begin();
	while (line != lines->end())
	{
		if (line->address == address)
		{
			selected = ctr + 1;
			break;
		}
		ctr++;
		line++;
	}

	ensureVisible(selected);
}

void DisassemblyView::setCurrent(offs_t address)
{
	int ctr = -1;
	std::vector<DisassemblyLine>::iterator line = lines->begin();
	while (line != lines->end())
	{
		if (line->address == address)
		{
			current = ctr + 1;
			break;
		}
		ctr++;
		line++;
	}

	ensureVisible(current);
}

void DisassemblyView::refresh()
{
	DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());
	redraw();
}

void DisassemblyView::setRange(offs_t start, offs_t end, uint8_t* memory)
{
	this->start = start;
	this->end = end;
	this->memory = memory;

	visible_items = height() / item_height;

	DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

	//int x = lines->size() - visible_items;
	//max_vscroll = x > 0 ? x : 0;
	//emit on_size(max_vscroll);
	resizeEvent(new QResizeEvent(size(), size()));
	offset = 0;
	is_memory_set = true;
}

void DisassemblyView::setEmulator(et3400emu* emu)
{
	emu_ptr = emu;
}

void DisassemblyView::addLabel()
{
	LabelDialog labelDialog;

	offs_t address = 0;
	if (selected > -1)
	{
		address = lines->at(selected).address;
	}

	labelDialog.setLabel(LabelInfo{ QString("New Label"), LabelType::COMMENT, address, address }, LabelDialogMode::Add);

	QDialog::DialogCode result = (QDialog::DialogCode)labelDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		LabelInfo label = labelDialog.getLabel();

		emu_ptr->labels->addLabel(Label{ label.start, label.end, label.type, label.text });

		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

		clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::addLabel(DisassemblyLine* line)
{
	LabelDialog labelDialog;
	labelDialog.setLabel(LabelInfo{ QString("New Label"), LabelType::DATA, line->address, line->address }, LabelDialogMode::Add);

	QDialog::DialogCode result = (QDialog::DialogCode)labelDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		LabelInfo label = labelDialog.getLabel();

		emu_ptr->labels->addLabel(Label{ label.start, label.end, label.type, label.text });

		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

		clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::editLabel(DisassemblyLine* line)
{
	LabelDialog labelDialog;
	labelDialog.setLabel(LabelInfo{ line->label->comment, line->label->type, line->label->start, line->label->end }, LabelDialogMode::Edit);

	QDialog::DialogCode result = (QDialog::DialogCode)labelDialog.exec();

	if (result == QDialog::DialogCode::Accepted)
	{
		LabelInfo label = labelDialog.getLabel();

		emu_ptr->labels->removeLabel(line->label);

		emu_ptr->labels->addLabel(Label{ label.start, label.end, label.type, label.text });

		DisassemblyBuilder::build(lines, start, end, memory, emu_ptr->labels->getLabels());

		clearSelected();

		resizeEvent(new QResizeEvent(size(), size()));
	}
}

void DisassemblyView::removeLabel(DisassemblyLine* line)
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

void DisassemblyView::showContextMenu(const QPoint& pos)
{
	// int line_number = offset + (pos.y() / item_height);

	if (selected > -1)
	{
		DisassemblyLine* line = &lines->at(selected);

		QMenu contextMenu(tr("Context menu"), this);
		QAction addLabelAction("Add label", this);
		QAction editLabelAction("Edit label", this);
		QAction removeLabelAction("Remove label", this);

		if (line->label == NULL)
		{
			connect(&addLabelAction, &QAction::triggered, this, [this, line] { addLabel(line); });
			contextMenu.addAction(&addLabelAction);
		}
		else
		{
			connect(&editLabelAction, &QAction::triggered, this, [this, line] { editLabel(line); });
			contextMenu.addAction(&editLabelAction);
			connect(&removeLabelAction, &QAction::triggered, this, [this, line] { removeLabel(line); });
			contextMenu.addAction(&removeLabelAction);
		}

		contextMenu.exec(mapToGlobal(pos));
	}

	////connect(&action1, &QAction::triggered(), this, SLOT(removeDataPoint()));
	//QAction action3("Remove Breakpoint", this);
	//QAction action4("Add Breakpoint", this);

	//if (line->type == DisassemblyType::Assembly)
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
	//}
}