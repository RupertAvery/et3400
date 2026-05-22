#ifndef DEBUGGER_UI_H
#define DEBUGGER_UI_H

#include "debugger.h"
#include "label.h"

QToolBar *DebuggerDialog::create_menu_toolbar()
{
    QToolBar *toolbar = new QToolBar(this);

    QToolButton *file_button = create_file_menu(toolbar);
    QToolButton *debug_button = create_debug_menu(toolbar);
    QToolButton *view_button = create_view_menu(toolbar);
    // QToolButton *settings_button = create_settings_menu(toolbar);

    toolbar->addWidget(file_button);
    toolbar->addWidget(debug_button);
    toolbar->addWidget(view_button);
    // toolbar->addWidget(settings_button);

    return toolbar;
}

QToolBar *DebuggerDialog::create_shortcuts_toolbar()
{
    QToolBar *shortcut_toolbar = new QToolBar(this);

    MakeToolButton(shortcut_toolbar, start_button, "Run (F5)", ":/buttons/Run.png", Qt::Key_F5, start);
    MakeToolButton(shortcut_toolbar, stop_button, "Stop (F4)", ":/buttons/Stop.png", Qt::Key_F4, stop);
    MakeToolButton(shortcut_toolbar, step_over_button, "Step Over (F10)", ":/buttons/StepOver.png", Qt::Key_F10, step_over);
    MakeToolButton(shortcut_toolbar, step_into_button, "Step Into (F11)", ":/buttons/StepInto.png", Qt::Key_F11, step_into);
    MakeToolButton(shortcut_toolbar, step_out_button, "Step Out (Shift+F11)", ":/buttons/StepOut.png", Qt::SHIFT + Qt::Key_F11, step_out);
    MakeToolButton(shortcut_toolbar, reset_button, "Reset (ESC)", ":/buttons/Restart.png", Qt::Key_Escape, reset);

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
    connect(saveMap_action, &QAction::triggered, this, &DebuggerDialog::save_ram_labels);

    return file_button;
}

QToolButton *DebuggerDialog::create_debug_menu(QToolBar *toolbar)
{
    QToolButton *debug_button = new QToolButton(toolbar);
    debug_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    debug_button->setText("Debug   ");
    debug_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *debug_menu = new QMenu(debug_button);

    debug_run_action = new QAction(QIcon(":/buttons/Run.png"), "Run\tF5", this);
    debug_stop_action = new QAction(QIcon(":/buttons/Stop.png"), "Stop\tF4", this);
    debug_step_over_action = new QAction(QIcon(":/buttons/StepOver.png"), "Step Over\tF10", this);
    debug_step_into_action = new QAction(QIcon(":/buttons/StepInto.png"), "Step Into\tF11", this);
    debug_step_out_action = new QAction(QIcon(":/buttons/StepOut.png"), "Step Out\tShift+F11", this);
    debug_reset_action = new QAction(QIcon(":/buttons/Restart.png"), "Reset\tEsc", this);

    debug_menu->addAction(debug_run_action);
    debug_menu->addAction(debug_stop_action);
    debug_menu->addSeparator();
    debug_menu->addAction(debug_step_over_action);
    debug_menu->addAction(debug_step_into_action);
    debug_menu->addAction(debug_step_out_action);
    debug_menu->addSeparator();
    debug_menu->addAction(debug_reset_action);
    debug_menu->addSeparator();
    MakeTriggeredActionNS(clear_ram_action, "Clear RAM", clear_ram);
    debug_menu->addAction(clear_ram_action);

    debug_button->setMenu(debug_menu);

    connect(debug_run_action, &QAction::triggered, this, &DebuggerDialog::start);
    connect(debug_stop_action, &QAction::triggered, this, &DebuggerDialog::stop);
    connect(debug_step_over_action, &QAction::triggered, this, &DebuggerDialog::step_over);
    connect(debug_step_into_action, &QAction::triggered, this, &DebuggerDialog::step_into);
    connect(debug_step_out_action, &QAction::triggered, this, &DebuggerDialog::step_out);
    connect(debug_reset_action, &QAction::triggered, this, &DebuggerDialog::reset);

    return debug_button;
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

    QAction *goto_label_action = new QAction("Goto Address/Label\tCtrl+G", this);
    goto_label_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    connect(goto_label_action, &QAction::triggered, this, &DebuggerDialog::goto_label);

    QAction *show_labels_action = new QAction("Labels\tCtrl+L", this);
    show_labels_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    connect(show_labels_action, &QAction::triggered, this, &DebuggerDialog::show_labels_dialog);

    QAction *show_breakpoints_action = new QAction("Breakpoints\tCtrl+B", this);
    show_breakpoints_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    connect(show_breakpoints_action, &QAction::triggered, this, &DebuggerDialog::show_breakpoints_dialog);

    view_menu->addAction(show_labels_action);
    view_menu->addAction(show_breakpoints_action);
    view_menu->addAction(goto_label_action);
    view_menu->addSeparator();
    view_menu->addAction(toggle_disassembly_action);
    view_menu->addAction(refresh_disassembly_action);
    view_menu->addAction(toggle_autorefresh_disassembly_action);
    view_menu->addSeparator();
    view_menu->addAction(toggle_memory_action);
    view_menu->addAction(toggle_heat_map_action);

    view_button->setMenu(view_menu);

    return view_button;
}

QToolButton *DebuggerDialog::create_settings_menu(QToolBar *toolbar)
{
    QToolButton *settings_button = new QToolButton(toolbar);
    settings_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    settings_button->setText("Settings   ");
    settings_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *settings_menu = new QMenu(settings_button);

    MakeToggledActionNS(toggle_load_default_labels_action, "Load Default Labels (RAM)", toggle_load_default_labels);
    settings_menu->addAction(toggle_load_default_labels_action);

    settings_button->setMenu(settings_menu);

    return settings_button;
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
    connect(disassembly_view, &DisassemblyView::onBreakpointChanged, this, &DebuggerDialog::populate_breakpoints_table);

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

// QDialog *DebuggerDialog::create_labels_dialog()
// {
//     QDialog *tab = new QDialog(this, Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
//     tab->setWindowTitle("Labels");
//     tab->resize(600, 400);
//     QVBoxLayout *layout = new QVBoxLayout(tab);
//     layout->setSpacing(0);
//     layout->setMargin(0);

//     QToolBar *toolbar = new QToolBar(tab);
//     toolbar->setMovable(false);

//     QAction *add_action = toolbar->addAction(QIcon(":/buttons/Add.png"), "Add");
//     tab_edit_label_action = toolbar->addAction(QIcon(":/buttons/Edit.png"), "Edit");
//     tab_remove_label_action = toolbar->addAction(QIcon(":/buttons/Remove.png"), "Remove");
//     tab_edit_label_action->setEnabled(false);
//     tab_remove_label_action->setEnabled(false);

//     toolbar->addSeparator();

//     QAction *load_labels_action = toolbar->addAction(QIcon(":/buttons/OpenFile.png"), "Load Labels");
//     QAction *save_labels_action = toolbar->addAction(QIcon(":/buttons/Save.png"), "Save Labels");
//     toolbar->addSeparator();
//     QAction *load_default_labels_action = toolbar->addAction(QIcon(":/buttons/Restart.png"), "Load Defaults");
//     toolbar->addSeparator();
//     tab_clear_ram_labels_action = toolbar->addAction(QIcon(":/buttons/Trash.png"), "Clear Labels");

//     toolbar->addSeparator();
//     tab_goto_label_action = toolbar->addAction(QIcon(":/buttons/GotoRow.png"), "Goto");

//     labels_table = new QTableWidget(tab);
//     labels_table->setColumnCount(4);
//     labels_table->setHorizontalHeaderLabels({"Start", "End", "Type", "Text"});
//     labels_table->setSelectionBehavior(QAbstractItemView::SelectRows);
//     labels_table->setSelectionMode(QAbstractItemView::SingleSelection);
//     labels_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
//     labels_table->horizontalHeader()->setStretchLastSection(true);
//     labels_table->verticalHeader()->setVisible(false);

//     layout->addWidget(toolbar);
//     layout->addWidget(labels_table);
//     tab->setLayout(layout);

//     connect(add_action, &QAction::triggered, this, &DebuggerDialog::add_label_from_table);
//     connect(tab_edit_label_action, &QAction::triggered, this, &DebuggerDialog::edit_label_from_table);
//     connect(tab_remove_label_action, &QAction::triggered, this, &DebuggerDialog::delete_label_from_table);
//     connect(tab_goto_label_action, &QAction::triggered, this, &DebuggerDialog::goto_label);
//     connect(load_labels_action, &QAction::triggered, this, &DebuggerDialog::load_labels);
//     connect(save_labels_action, &QAction::triggered, this, &DebuggerDialog::save_labels);
//     connect(load_default_labels_action, &QAction::triggered, this, &DebuggerDialog::load_default_labels);
//     connect(tab_clear_ram_labels_action, &QAction::triggered, this, &DebuggerDialog::clear_labels);
//     connect(labels_table, &QTableWidget::itemSelectionChanged, this, &DebuggerDialog::labels_table_selection_changed);
//     connect(labels_table, &QTableWidget::cellDoubleClicked, this, [this](int, int) { goto_label_from_table(); });

//     labels_table->setContextMenuPolicy(Qt::CustomContextMenu);
//     connect(labels_table, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos) {
//         if (labels_table->selectedItems().isEmpty())
//             return;
//         QMenu menu(this);
//         menu.addAction(tab_edit_label_action);
//         menu.addAction(tab_remove_label_action);
//         menu.exec(labels_table->viewport()->mapToGlobal(pos));
//     });

//     return tab;
// }

void DebuggerDialog::goto_address(offs_t address)
{
    memory_mapped_device *device = emu_ptr->get_block_device(address);
    if (!device)
        return;
    selectByAddress(device->get_start());
    disassembly_view->setSelected(address);
    disassembly_scrollbar->setValue(disassembly_view->offset);
}

// QDialog *DebuggerDialog::create_breakpoints_dialog()
// {
//     QDialog *tab = new QDialog(this, Qt::Window | Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint | Qt::WindowMaximizeButtonHint);
//     tab->setWindowTitle("Breakpoints");
//     tab->resize(400, 300);
//     QVBoxLayout *layout = new QVBoxLayout(tab);
//     layout->setSpacing(0);
//     layout->setMargin(0);

//     QToolBar *toolbar = new QToolBar(tab);
//     toolbar->setMovable(false);

//     QAction *add_action = toolbar->addAction(QIcon(":/buttons/Add.png"), "Add");
//     tab_remove_breakpoint_action = toolbar->addAction(QIcon(":/buttons/Remove.png"), "Remove");
//     tab_remove_breakpoint_action->setEnabled(false);

//     toolbar->addSeparator();

//     QAction *load_breakpoints_action = toolbar->addAction(QIcon(":/buttons/OpenFile.png"), "Load Breakpoints");
//     QAction *save_breakpoints_action = toolbar->addAction(QIcon(":/buttons/Save.png"), "Save Breakpoints");
//     toolbar->addSeparator();
//     QAction *clear_breakpoints_action = toolbar->addAction(QIcon(":/buttons/Trash.png"), "Clear Breakpoints");

//     breakpoints_table = new QTableWidget(tab);
//     breakpoints_table->setColumnCount(3);
//     breakpoints_table->setHorizontalHeaderLabels({"", "Address", "Type"});
//     breakpoints_table->setSelectionBehavior(QAbstractItemView::SelectRows);
//     breakpoints_table->setSelectionMode(QAbstractItemView::SingleSelection);
//     breakpoints_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
//     breakpoints_table->horizontalHeader()->setStretchLastSection(true);
//     breakpoints_table->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Fixed);
//     breakpoints_table->setColumnWidth(0, 24);
//     breakpoints_table->verticalHeader()->setVisible(false);

//     layout->addWidget(toolbar);
//     layout->addWidget(breakpoints_table);
//     tab->setLayout(layout);

//     connect(add_action, &QAction::triggered, this, &DebuggerDialog::add_breakpoint_from_table);
//     connect(tab_remove_breakpoint_action, &QAction::triggered, this, &DebuggerDialog::remove_breakpoint_from_table);
//     connect(load_breakpoints_action, &QAction::triggered, this, &DebuggerDialog::load_breakpoints);
//     connect(save_breakpoints_action, &QAction::triggered, this, &DebuggerDialog::save_breakpoints);
//     connect(clear_breakpoints_action, &QAction::triggered, this, &DebuggerDialog::clear_breakpoints);
//     connect(breakpoints_table, &QTableWidget::itemSelectionChanged, this, &DebuggerDialog::breakpoints_table_selection_changed);
//     connect(breakpoints_table, &QTableWidget::itemChanged, this, &DebuggerDialog::breakpoint_item_changed);
//     connect(breakpoints_table, &QTableWidget::cellDoubleClicked, this, [this](int, int)
//             {
//         int row = breakpoints_table->currentRow();
//         if (row < 0) return;
//         offs_t address = (offs_t)breakpoints_table->item(row, 0)->data(Qt::UserRole).toUInt();
//         memory_mapped_device *device = emu_ptr->get_block_device(address);
//         if (!device) return;
//         selectByAddress(device->get_start());
//         disassembly_view->setSelected(address);
//         disassembly_scrollbar->setValue(disassembly_view->offset); });

//     breakpoints_table->setContextMenuPolicy(Qt::CustomContextMenu);
//     connect(breakpoints_table, &QTableWidget::customContextMenuRequested, this, [this](const QPoint &pos)
//             {
//         if (breakpoints_table->selectedItems().isEmpty())
//             return;
//         QMenu menu(this);
//         menu.addAction(tab_remove_breakpoint_action);
//         menu.exec(breakpoints_table->viewport()->mapToGlobal(pos)); });

//     return tab;
// }

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

    labels_dialog = new LabelsDialog(this);
    breakpoints_dialog = new BreakpointsDialog(this);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(toolbar);
    mainLayout->addWidget(shortcut_toolbar);
    mainLayout->addWidget(panels);
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
