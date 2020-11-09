#ifndef DEBUGGER_UI_H
#define DEBUGGER_UI_H

#include "debugger.h"

void DebuggerDialog::setupUI()
{
    QToolBar *toolbar = new QToolBar(this);
    QLabel *memory_label = new QLabel("Memory");
    memory_label->setStyleSheet("margin: 0px 5px 0px 5px;");
    QLabel *disassembly_label = new QLabel("Disassembly");
    disassembly_label->setStyleSheet("margin: 0px 5px 0px 5px;");

    memory_selector = new QComboBox(toolbar);
    memory_selector->addItem("RAM");
    memory_selector->addItem("ROM");

    disassembly_selector = new QComboBox(toolbar);
    disassembly_selector->addItem("RAM");
    disassembly_selector->addItem("ROM");

    start_button = new QToolButton(toolbar);
    start_button->setIcon(QIcon(":/buttons/Run_16x.png"));
    start_button->setShortcut(QKeySequence(Qt::Key_F5));
    connect(start_button, &QToolButton::clicked, this, &DebuggerDialog::start);

    stop_button = new QToolButton(toolbar);
    stop_button->setIcon(QIcon(":/buttons/Stop_16x.png"));
    stop_button->setShortcut(QKeySequence(Qt::Key_F4));
    connect(stop_button, &QToolButton::clicked, this, &DebuggerDialog::stop);

    step_button = new QToolButton(toolbar);
    step_button->setIcon(QIcon(":/buttons/StepIntoArrow_16x.png"));
    step_button->setShortcut(QKeySequence(Qt::Key_F10));
    connect(step_button, &QToolButton::clicked, this, &DebuggerDialog::step);

    reset_button = new QToolButton(toolbar);
    reset_button->setIcon(QIcon(":/buttons/Restart_16x.png"));
    reset_button->setShortcut(QKeySequence(Qt::Key_Escape));
    connect(reset_button, &QToolButton::clicked, this, &DebuggerDialog::reset);

    panel_selector = new QToolButton(toolbar);
    panel_selector->setToolButtonStyle(Qt::ToolButtonTextOnly);
    panel_selector->setText("Panels");
    panel_selector->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *panel_selector_menu = new QMenu(panel_selector);

    toggle_memory_action = new QAction("Memory", this);
    toggle_memory_action->setCheckable(true);
    toggle_memory_action->setChecked(true);
    toggle_memory_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_1));
    connect(toggle_memory_action, &QAction::toggled, this, &DebuggerDialog::toggle_memory_panel);

    toggle_disassembly_action = new QAction("Disassembly", this);
    toggle_disassembly_action->setCheckable(true);
    toggle_disassembly_action->setChecked(true);
    toggle_disassembly_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_2));
    connect(toggle_disassembly_action, &QAction::toggled, this, &DebuggerDialog::toggle_disassembly_panel);

    toggle_status_action = new QAction("Status", this);
    toggle_status_action->setCheckable(true);
    toggle_status_action->setChecked(true);
    toggle_status_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_3));
    connect(toggle_status_action, &QAction::toggled, this, &DebuggerDialog::toggle_status_panel);

    panel_selector_menu->addAction(toggle_memory_action);
    panel_selector_menu->addAction(toggle_disassembly_action);
    panel_selector_menu->addAction(toggle_status_action);
    panel_selector->setMenu(panel_selector_menu);

    toolbar->addWidget(start_button);
    toolbar->addWidget(stop_button);
    toolbar->addWidget(step_button);
    toolbar->addWidget(reset_button);
    toolbar->addSeparator();
    toolbar->addWidget(memory_label);
    toolbar->addWidget(memory_selector);
    toolbar->addSeparator();
    toolbar->addWidget(disassembly_label);
    toolbar->addWidget(disassembly_selector);
    toolbar->addSeparator();
    toolbar->addWidget(panel_selector);

    memory_groupBox = new QGroupBox("Memory", this);
    disassembly_groupBox = new QGroupBox("Disassembly", this);
    status_groupBox = new QGroupBox("Status", this);

    memmory_scrollbar = new QScrollBar(Qt::Orientation::Vertical);
    disassembly_scrollbar = new QScrollBar(Qt::Orientation::Vertical);

    memory_view = new MemoryView(this);
    disassembly_view = new DisassemblyView(this);
    status_view = new StatusView(this);

    QHBoxLayout *memory_groupBox_layout = new QHBoxLayout(this);
    memory_groupBox_layout->addWidget(memory_view);
    memory_groupBox_layout->addWidget(memmory_scrollbar);
    memory_groupBox_layout->setMargin(10);
    memory_groupBox->setLayout(memory_groupBox_layout);

    QHBoxLayout *disassembly_groupBox_layout = new QHBoxLayout(this);
    disassembly_groupBox_layout->addWidget(disassembly_view);
    disassembly_groupBox_layout->addWidget(disassembly_scrollbar);
    disassembly_groupBox_layout->setMargin(10);
    disassembly_groupBox->setLayout(disassembly_groupBox_layout);

    QHBoxLayout *status_groupBox_layout = new QHBoxLayout(this);
    status_groupBox_layout->addWidget(status_view);
    status_groupBox_layout->setMargin(10);
    status_groupBox->setLayout(status_groupBox_layout);
    status_groupBox->setFixedWidth(177);

    QWidget *panels = new QWidget(this);
    QHBoxLayout *panels_layout = new QHBoxLayout(this);
    panels_layout->addWidget(memory_groupBox);
    panels_layout->addWidget(disassembly_groupBox);
    panels_layout->addWidget(status_groupBox);
    panels_layout->setMargin(10);
    panels->setLayout(panels_layout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(panels);

    setLayout(mainLayout);

    connect(memmory_scrollbar, &QScrollBar::sliderMoved, this, &DebuggerDialog::memory_slider_moved);
    connect(disassembly_scrollbar, &QScrollBar::sliderMoved, this, &DebuggerDialog::disassembly_slider_moved);
    // this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

    connect(memory_view, &MemoryView::on_scroll, this, &DebuggerDialog::update_memory_scrollbar);
    connect(disassembly_view, &DisassemblyView::on_scroll, this, &DebuggerDialog::update_disassembly_scrollbar);

    connect(memory_view, &MemoryView::on_size, this, &DebuggerDialog::update_memory_scrollbar_max);
    connect(disassembly_view, &DisassemblyView::on_size, this, &DebuggerDialog::update_disassembly_scrollbar_max);

    connect(memory_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_memory_location);
    connect(disassembly_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_disassembly_location);

    connect(disassembly_view, &DisassemblyView::add_breakpoint, this, &DebuggerDialog::add_breakpoint);
    connect(disassembly_view, &DisassemblyView::remove_breakpoint, this, &DebuggerDialog::remove_breakpoint);

    breakpoint_handler_action = new QAction;
    connect(breakpoint_handler_action, &QAction::triggered, this, &DebuggerDialog::breakpoint_handler);

    setFixedSize(QSize(985, 721));
    setWindowTitle("Debugger");
}

#endif // DEBUGGER_UI_H
