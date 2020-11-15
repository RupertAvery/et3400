#ifndef DEBUGGER_UI_H
#define DEBUGGER_UI_H

#include "debugger.h"

void DebuggerDialog::setupUI()
{
    QToolBar *toolbar = new QToolBar(this);

    // QLabel *memory_label = new QLabel("Memory");
    // memory_label->setStyleSheet("margin: 0px 5px 0px 5px;");

    // QLabel *disassembly_label = new QLabel("Disassembly");
    // disassembly_label->setStyleSheet("margin: 0px 5px 0px 5px;");

    MakeButton(start_button, "Start (F5)", ":/buttons/Run_16x.png", Qt::Key_F5, start);
    MakeButton(stop_button, "Stop (F4)", ":/buttons/Stop_16x.png", Qt::Key_F4, stop);
    MakeButton(step_button, "Step in (F10)", ":/buttons/StepIntoArrow_16x.png", Qt::Key_F10, step);
    MakeButton(reset_button, "Reset (ESC)", ":/buttons/Restart_16x.png", Qt::Key_Escape, reset);

    panel_selector = new QToolButton(toolbar);
    panel_selector->setToolButtonStyle(Qt::ToolButtonTextOnly);
    panel_selector->setText("Panels   ");
    panel_selector->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *panel_selector_menu = new QMenu(panel_selector);

    // MakeAction(toggle_disassembly_action, "Disassembly", Qt::CTRL + Qt::Key_2, toggle_disassembly_panel);
    MakeAction(toggle_memory_action, "Memory", Qt::CTRL + Qt::Key_M, toggle_memory_panel);
    // MakeAction(toggle_status_action, "Status", Qt::CTRL + Qt::Key_3, toggle_status_panel);

    panel_selector_menu->addAction(toggle_memory_action);
    // panel_selector_menu->addAction(toggle_disassembly_action);
    // panel_selector_menu->addAction(toggle_status_action);
    panel_selector->setMenu(panel_selector_menu);

    QAction *openRam_action = new QAction("&Load RAM", this);
    openRam_action->setShortcut(Qt::CTRL + Qt::Key_O);

    QAction *saveRam_action = new QAction("&Save RAM", this);
    saveRam_action->setShortcut(Qt::CTRL + Qt::Key_S);

    QAction *openBrk_action = new QAction("Load Breakpoints", this);
    QAction *saveBrk_action = new QAction("Save Breakpoints", this);

    QAction *openMap_action = new QAction("Load Labels (RAM)", this);
    QAction *saveMap_action = new QAction("Save Labels (RAM)", this);

    QToolButton *file_button = new QToolButton(toolbar);
    file_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    file_button->setText("File   ");
    file_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *file_menu = new QMenu(file_button);
    file_menu->addAction(openRam_action);
    file_menu->addAction(saveRam_action);
    file_menu->addSeparator();
    file_menu->addAction(openBrk_action);
    file_menu->addAction(saveBrk_action);
    file_menu->addAction(openMap_action);
    file_menu->addAction(saveMap_action);
    file_button->setMenu(file_menu);

    connect(openRam_action, &QAction::triggered, this, &DebuggerDialog::load_ram);
    connect(saveRam_action, &QAction::triggered, this, &DebuggerDialog::save_ram);
    connect(openBrk_action, &QAction::triggered, this, &DebuggerDialog::load_breakpoints);
    connect(saveBrk_action, &QAction::triggered, this, &DebuggerDialog::save_breakpoints);
    connect(openMap_action, &QAction::triggered, this, &DebuggerDialog::load_labels);
    connect(saveMap_action, &QAction::triggered, this, &DebuggerDialog::save_labels);

    toolbar->addWidget(file_button);
    toolbar->addSeparator();
    toolbar->addWidget(start_button);
    toolbar->addWidget(stop_button);
    toolbar->addWidget(step_button);
    toolbar->addWidget(reset_button);
    toolbar->addSeparator();
    toolbar->addWidget(panel_selector);

    memory_groupBox = new QGroupBox("Memory", this);
    memory_groupBox->setMaximumWidth(380);

    disassembly_groupBox = new QGroupBox("Disassembly", this);
    status_groupBox = new QGroupBox("Status", this);

    memory_scrollbar = new QScrollBar(Qt::Orientation::Vertical);
    disassembly_scrollbar = new QScrollBar(Qt::Orientation::Vertical);

    memory_view = new MemoryView(this);
    disassembly_view = new DisassemblyView(this);
    status_view = new StatusView(this);

    memory_selector = new QComboBox(toolbar);
    memory_selector->addItem("RAM");
    memory_selector->addItem("Keypad");
    memory_selector->addItem("Display");
    memory_selector->addItem("ROM");

    QWidget *inner_memory = new QWidget(memory_groupBox);
    QHBoxLayout *memory_groupBox_layout = new QHBoxLayout(inner_memory);
    memory_groupBox_layout->addWidget(memory_view);
    memory_groupBox_layout->addWidget(memory_scrollbar);
    memory_groupBox_layout->setMargin(0);
    inner_memory->setLayout(memory_groupBox_layout);

    QVBoxLayout *memory_groupBox_layout_v = new QVBoxLayout(this);
    memory_groupBox_layout_v->addWidget(memory_selector);
    memory_groupBox_layout_v->addWidget(inner_memory);
    memory_groupBox->setLayout(memory_groupBox_layout_v);

    disassembly_selector = new QComboBox(toolbar);
    disassembly_selector->addItem("RAM");
    disassembly_selector->addItem("ROM");

    QWidget *inner_disassembly = new QWidget(disassembly_groupBox);
    QHBoxLayout *disassembly_groupBox_layout = new QHBoxLayout(this);
    disassembly_groupBox_layout->addWidget(disassembly_view);
    disassembly_groupBox_layout->addWidget(disassembly_scrollbar);
    disassembly_groupBox_layout->setMargin(0);
    inner_disassembly->setLayout(disassembly_groupBox_layout);

    QVBoxLayout *disassembly_groupBox_layout_v = new QVBoxLayout(this);
    disassembly_groupBox_layout_v->addWidget(disassembly_selector);
    disassembly_groupBox_layout_v->addWidget(inner_disassembly);
    disassembly_groupBox->setLayout(disassembly_groupBox_layout_v);

    QHBoxLayout *status_groupBox_layout = new QHBoxLayout(this);
    status_groupBox_layout->addWidget(status_view);
    status_groupBox_layout->setMargin(10);
    status_groupBox->setLayout(status_groupBox_layout);
    status_groupBox->setFixedWidth(177);

    QWidget *panels = new QWidget(this);
    QHBoxLayout *panels_layout = new QHBoxLayout(this);
    panels_layout->addWidget(disassembly_groupBox);
    panels_layout->addWidget(memory_groupBox);
    panels_layout->addWidget(status_groupBox);
    panels_layout->setMargin(10);
    panels->setLayout(panels_layout);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(panels);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    setSizeGripEnabled(true);

    connect(memory_scrollbar, &QScrollBar::sliderMoved, this, &DebuggerDialog::memory_slider_moved);
    connect(disassembly_scrollbar, &QScrollBar::sliderMoved, this, &DebuggerDialog::disassembly_slider_moved);
    connect(memory_scrollbar, &QScrollBar::valueChanged, this, &DebuggerDialog::memory_slider_moved);
    connect(disassembly_scrollbar, &QScrollBar::valueChanged, this, &DebuggerDialog::disassembly_slider_moved);
    // this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

    connect(memory_view, &MemoryView::on_scroll, this, &DebuggerDialog::update_memory_scrollbar);
    connect(disassembly_view, &DisassemblyView::on_scroll, this, &DebuggerDialog::update_disassembly_scrollbar);

    connect(memory_view, &MemoryView::on_size, this, &DebuggerDialog::update_memory_scrollbar_max);
    connect(disassembly_view, &DisassemblyView::on_size, this, &DebuggerDialog::update_disassembly_scrollbar_max);

    connect(disassembly_view, &DisassemblyView::on_set_current, this, &DebuggerDialog::update_disassembly_scrollbar);

    connect(memory_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_memory_location);
    connect(disassembly_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_disassembly_location);

    connect(disassembly_view, &DisassemblyView::add_breakpoint, this, &DebuggerDialog::add_breakpoint);
    connect(disassembly_view, &DisassemblyView::remove_breakpoint, this, &DebuggerDialog::remove_breakpoint);
    connect(disassembly_view, &DisassemblyView::add_or_remove_breakpoint_signal, this, &DebuggerDialog::add_or_remove_breakpoint);

    breakpoint_handler_action = new QAction;
    connect(breakpoint_handler_action, &QAction::triggered, this, &DebuggerDialog::breakpoint_handler);

    resize(QSize(985, 721));

    //setFixedSize(QSize(985, 721));
    setWindowTitle("Debugger");
}

#endif // DEBUGGER_UI_H
