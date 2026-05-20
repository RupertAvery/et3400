#ifndef DEBUGGER_UI_H
#define DEBUGGER_UI_H

#include "debugger.h"
#include "label.h"

QToolBar *DebuggerDialog::create_menu_toolbar()
{
    QToolBar *toolbar = new QToolBar(this);

    QToolButton *file_button = create_file_menu(toolbar);
    QToolButton *labels_button = create_labels_menu(toolbar);
    QToolButton *view_button = create_view_menu(toolbar);

    // toolbar->setStyleSheet("QLabel { font-size:9pt; }");

    // QLabel *memory_label = new QLabel("Memory");
    // memory_label->setStyleSheet("margin: 0px 5px 0px 5px;");

    // QLabel *disassembly_label = new QLabel("Disassembly");
    // disassembly_label->setStyleSheet("margin: 0px 5px 0px 5px;");
    toolbar->addWidget(file_button);
    toolbar->addWidget(view_button);
    toolbar->addWidget(labels_button);

    return toolbar;
}

QToolBar *DebuggerDialog::create_shortcuts_toolbar()
{
    QToolBar *shortcut_toolbar = new QToolBar(this);

    MakeButton(shortcut_toolbar, start_button, "Run (F5)", ":/buttons/Run.png", Qt::Key_F5, start);
    MakeButton(shortcut_toolbar, stop_button, "Stop (F4)", ":/buttons/Stop.png", Qt::Key_F4, stop);
    MakeButton(shortcut_toolbar, step_over_button, "Step Over (F10)", ":/buttons/StepOver.png", Qt::Key_F10, step_over);
    MakeButton(shortcut_toolbar, step_into_button, "Step Into (F11)", ":/buttons/StepInto.png", Qt::Key_F11, step_into);
    MakeButton(shortcut_toolbar, step_out_button, "Step Out (Shift+F11)", ":/buttons/StepOut.png", Qt::SHIFT + Qt::Key_F11, step_out);
    MakeButton(shortcut_toolbar, reset_button, "Reset (ESC)", ":/buttons/Restart.png", Qt::Key_Escape, reset);

    shortcut_toolbar->addWidget(start_button);
    shortcut_toolbar->addWidget(stop_button);
    shortcut_toolbar->addWidget(step_over_button);
    shortcut_toolbar->addWidget(step_into_button);
    shortcut_toolbar->addWidget(step_out_button);
    shortcut_toolbar->addWidget(reset_button);

    return shortcut_toolbar;
}

QToolButton *DebuggerDialog::create_file_menu(QToolBar *toolbar)
{

    QAction *openRam_action = new QAction("&Load RAM", this);
    openRam_action->setShortcut(Qt::CTRL + Qt::Key_O);

    QAction *saveRam_action = new QAction("&Save RAM", this);
    saveRam_action->setShortcut(Qt::CTRL + Qt::Key_S);

    QAction *openRom_action = new QAction("Load ROM", this);

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
    file_menu->addAction(openRom_action);
    file_menu->addSeparator();
    file_menu->addAction(openBrk_action);
    file_menu->addAction(saveBrk_action);
    file_menu->addSeparator();
    file_menu->addAction(openMap_action);
    file_menu->addAction(saveMap_action);
    file_button->setMenu(file_menu);

    connect(openRam_action, &QAction::triggered, this, &DebuggerDialog::load_ram);
    connect(saveRam_action, &QAction::triggered, this, &DebuggerDialog::save_ram);

    connect(openRom_action, &QAction::triggered, this, &DebuggerDialog::load_rom);

    connect(openBrk_action, &QAction::triggered, this, &DebuggerDialog::load_breakpoints);
    connect(saveBrk_action, &QAction::triggered, this, &DebuggerDialog::save_breakpoints);

    connect(openMap_action, &QAction::triggered, this, &DebuggerDialog::load_labels);
    connect(saveMap_action, &QAction::triggered, this, &DebuggerDialog::save_labels);

    return file_button;
}

QToolButton *DebuggerDialog::create_labels_menu(QToolBar *toolbar)
{
    QToolButton *labels_button = new QToolButton(toolbar);
    labels_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    labels_button->setText("Labels   ");
    labels_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    clear_labels_action = new QAction("Clear Labels (RAM)", this);
    connect(clear_labels_action, &QAction::triggered, this, &DebuggerDialog::clear_labels);

    add_label_action = new QAction("Add Label", this);
    connect(add_label_action, &QAction::triggered, this, &DebuggerDialog::add_label);

    MakeTriggeredAction(goto_label_action, "Goto Label", Qt::CTRL + Qt::Key_G, goto_label);

    QMenu *labels_menu = new QMenu(labels_button);
    // labels_selector_menu->setStyleSheet("QLabel { font-size:9pt; }");
    labels_menu->addAction(add_label_action);
    labels_menu->addAction(goto_label_action);
    labels_menu->addSeparator();
    labels_menu->addAction(clear_labels_action);
    labels_button->setMenu(labels_menu);

    return labels_button;
}

QToolButton *DebuggerDialog::create_view_menu(QToolBar *toolbar)
{
    QToolButton *view_button = new QToolButton(toolbar);
    view_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    view_button->setText("View   ");
    view_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *view_menu = new QMenu(view_button);

    MakeToggledAction(toggle_disassembly_action, "Disassembly", Qt::CTRL + Qt::Key_D, toggle_disassembly_panel);
    MakeToggledActionNS(toggle_autorefresh_disassembly_action, "Auto Refresh Disassembly", toggle_auto_refresh_disassembly_panel);

    MakeTriggeredAction(refresh_disassembly_action, "Refresh", Qt::CTRL + Qt::Key_R, diassembly_refresh);
    MakeToggledAction(toggle_memory_action, "Memory", Qt::CTRL + Qt::Key_M, toggle_memory_panel);
    MakeToggledAction(toggle_heat_map_action, "Heat Map", Qt::CTRL + Qt::Key_H, toggle_heat_map);

    MakeTriggeredActionNS(clear_ram_action, "Clear RAM", clear_ram);

    view_menu->addAction(toggle_disassembly_action);
    view_menu->addAction(refresh_disassembly_action);
    view_menu->addAction(toggle_autorefresh_disassembly_action);
    view_menu->addSeparator();
    view_menu->addAction(toggle_memory_action);
    view_menu->addAction(toggle_heat_map_action);
    view_menu->addSeparator();
    view_menu->addAction(clear_ram_action);

    view_button->setMenu(view_menu);

    return view_button;
}

QGroupBox *DebuggerDialog::create_status_group()
{
    QGroupBox *status_groupBox = new QGroupBox("Status", this);

    QHBoxLayout *status_groupBox_layout = new QHBoxLayout(this);

    status_view = new StatusView(this);
    status_groupBox_layout->addWidget(status_view);

    status_groupBox_layout->setMargin(10);
    status_groupBox->setLayout(status_groupBox_layout);
    status_groupBox->setFixedWidth(200);

    return status_groupBox;
}

QGroupBox *DebuggerDialog::create_disassembly_group()
{
    disassembly_scrollbar = new QScrollBar(Qt::Orientation::Vertical);

    QGroupBox *disassembly_groupBox = new QGroupBox("Disassembly", this);
    disassembly_groupBox->setMinimumWidth(400);
    disassembly_view = new DisassemblyView(this);
    disassembly_selector = new QComboBox(disassembly_groupBox);

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

    connect(disassembly_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_disassembly_location);

    connect(disassembly_scrollbar, &QScrollBar::sliderMoved, this, &DebuggerDialog::disassembly_slider_moved);
    connect(disassembly_scrollbar, &QScrollBar::valueChanged, this, &DebuggerDialog::disassembly_slider_moved);

    connect(disassembly_view, &DisassemblyView::onScroll, this, &DebuggerDialog::adjustDisassemblyScrollbar);
    connect(disassembly_view, &DisassemblyView::onSize, this, &DebuggerDialog::update_disassembly_scrollbar_max);
    connect(disassembly_view, &DisassemblyView::onOffsetUpdated, this, &DebuggerDialog::setDisassemblyScrollbar);
    connect(disassembly_view, &DisassemblyView::onAddBreakpoint, this, &DebuggerDialog::add_breakpoint);
    connect(disassembly_view, &DisassemblyView::onRemoveBreakpoint, this, &DebuggerDialog::remove_breakpoint);
    connect(disassembly_view, &DisassemblyView::onAddorRemoveBreakpoint, this, &DebuggerDialog::add_or_remove_breakpoint);

    return disassembly_groupBox;
}

QGroupBox *DebuggerDialog::create_memory_group()
{
    memory_scrollbar = new QScrollBar(Qt::Orientation::Vertical);

    QGroupBox *memory_groupBox = new QGroupBox("Memory", this);
    memory_groupBox->setMinimumWidth(360);

    memory_view = new MemoryView(memory_groupBox);

    memory_selector = new QComboBox(memory_groupBox);

    QWidget *inner_memory = new QWidget(memory_groupBox);

    QHBoxLayout *memory_groupBox_layout = new QHBoxLayout(inner_memory);
    memory_groupBox_layout->addWidget(memory_view);
    memory_groupBox_layout->addWidget(memory_scrollbar);
    memory_groupBox_layout->setMargin(0);
    inner_memory->setLayout(memory_groupBox_layout);

    QVBoxLayout *memory_groupBox_layout_v = new QVBoxLayout(memory_groupBox);
    memory_groupBox_layout_v->addWidget(memory_selector);
    memory_groupBox_layout_v->addWidget(inner_memory);
    memory_groupBox->setLayout(memory_groupBox_layout_v);

    connect(memory_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_memory_location);

    connect(memory_scrollbar, &QScrollBar::sliderMoved, this, &DebuggerDialog::memory_slider_moved);
    connect(memory_scrollbar, &QScrollBar::valueChanged, this, &DebuggerDialog::memory_slider_moved);

    connect(memory_view, &MemoryView::on_scroll, this, &DebuggerDialog::update_memory_scrollbar);
    connect(memory_view, &MemoryView::on_size, this, &DebuggerDialog::update_memory_scrollbar_max);

    return memory_groupBox;
}

QWidget *DebuggerDialog::create_labels_tab()
{

    return new QWidget(this);
}

QWidget *DebuggerDialog::create_breakpoints_tab()
{
    return new QWidget(this);
}

QWidget *DebuggerDialog::create_tab_panel()
{
    QWidget *panel = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    panel->setLayout(layout);

    QTabWidget *tab_widget = new QTabWidget(panel);
    layout->addWidget(tab_widget);

    tab_widget->addTab(create_labels_tab(), "Labels");
    tab_widget->addTab(create_breakpoints_tab(), "Breakpoints");

    return panel;
}

void DebuggerDialog::setupUI()
{
    QToolBar *toolbar = create_menu_toolbar();
    QToolBar *shortcut_toolbar = create_shortcuts_toolbar();

    status_groupBox = create_status_group();
    disassembly_groupBox = create_disassembly_group();
    memory_groupBox = create_memory_group();

    QWidget *panels = new QWidget(this);
    QHBoxLayout *panels_layout = new QHBoxLayout(this);

    panels_layout->addWidget(status_groupBox);
    panels_layout->addWidget(disassembly_groupBox);
    panels_layout->addWidget(memory_groupBox);
    panels_layout->setMargin(10);
    panels->setLayout(panels_layout);

    QWidget *tabs = create_tab_panel();


    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(shortcut_toolbar);
    mainLayout->addWidget(panels);
    mainLayout->addWidget(tabs);
    mainLayout->setMargin(0);
    mainLayout->setSpacing(0);

    setLayout(mainLayout);

    setSizeGripEnabled(true);

    breakpoint_handler_action = new QAction;
    connect(breakpoint_handler_action, &QAction::triggered, this, &DebuggerDialog::breakpoint_handler);

    resize(QSize(985, 721));

    // setFixedSize(QSize(985, 721));
    setWindowTitle("Debugger");
}

#endif // DEBUGGER_UI_H
