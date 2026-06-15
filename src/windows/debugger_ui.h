#ifndef DEBUGGER_UI_H
#define DEBUGGER_UI_H

#include "debugger.h"
#include "label.h"
#include "../common/util.h"
#include "../util/log.h"

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
    // MakeToolButton(shortcut_toolbar, step_out_button, "Step Out (Shift+F11)", ":/buttons/StepOut.png", Qt::SHIFT + Qt::Key_F11, step_out);
    MakeToolButtonNS(shortcut_toolbar, reset_button, "Reset", ":/buttons/Restart.png", reset);

    shortcut_toolbar->addWidget(start_button);
    shortcut_toolbar->addWidget(stop_button);
    shortcut_toolbar->addWidget(step_over_button);
    shortcut_toolbar->addWidget(step_into_button);
    // shortcut_toolbar->addWidget(step_out_button);
    shortcut_toolbar->addWidget(reset_button);

    // shortcut_toolbar->addSeparator();
    // shortcut_toolbar->addAction(set_heat_map_off_action);
    // shortcut_toolbar->addAction(set_heat_map_fade_action);
    // shortcut_toolbar->addAction(set_heat_map_fade_slow_action);
    // shortcut_toolbar->addAction(set_heat_map_persist_action);
    // shortcut_toolbar->addAction(clear_heat_map_action);

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

    QAction *exit_action = new QAction("E&xit", this);
    exit_action->setShortcut(Qt::CTRL + Qt::Key_X);

    QToolButton *file_button = new QToolButton(toolbar);
    file_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    file_button->setText("&File   ");
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
    file_menu->addSeparator();
    file_menu->addAction(exit_action);

    file_button->setMenu(file_menu);

    connect(openRam_action, &QAction::triggered, this, &DebuggerDialog::load_ram);
    connect(saveRam_action, &QAction::triggered, this, &DebuggerDialog::save_ram);

    connect(openRom_action, &QAction::triggered, this, &DebuggerDialog::load_rom);

    connect(openBrk_action, &QAction::triggered, this, &DebuggerDialog::load_breakpoints);
    connect(saveBrk_action, &QAction::triggered, this, &DebuggerDialog::save_breakpoints);

    connect(openMap_action, &QAction::triggered, this, &DebuggerDialog::load_labels);
    connect(saveMap_action, &QAction::triggered, this, &DebuggerDialog::save_ram_labels);

    connect(exit_action, &QAction::triggered, this, &DebuggerDialog::exit);

    return file_button;
}

QToolButton *DebuggerDialog::create_debug_menu(QToolBar *toolbar)
{
    QToolButton *debug_button = new QToolButton(toolbar);
    debug_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    debug_button->setText("&Debug   ");
    debug_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *debug_menu = new QMenu(debug_button);

    debug_run_action = new QAction(QIcon(":/buttons/Run.png"), "Run\tF5", this);
    debug_stop_action = new QAction(QIcon(":/buttons/Stop.png"), "Stop\tF4", this);
    debug_step_over_action = new QAction(QIcon(":/buttons/StepOver.png"), "Step Over\tF10", this);
    debug_step_into_action = new QAction(QIcon(":/buttons/StepInto.png"), "Step Into\tF11", this);
    // debug_step_out_action = new QAction(QIcon(":/buttons/StepOut.png"), "Step Out\tShift+F11", this);
    debug_reset_action = new QAction(QIcon(":/buttons/Restart.png"), "Reset", this);

    debug_menu->addAction(debug_run_action);
    debug_menu->addAction(debug_stop_action);
    debug_menu->addSeparator();
    debug_menu->addAction(debug_step_over_action);
    debug_menu->addAction(debug_step_into_action);
    // debug_menu->addAction(debug_step_out_action);
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
    // connect(debug_step_out_action, &QAction::triggered, this, &DebuggerDialog::step_out);
    connect(debug_reset_action, &QAction::triggered, this, &DebuggerDialog::reset);

    return debug_button;
}

QToolButton *DebuggerDialog::create_view_menu(QToolBar *toolbar)
{
    QToolButton *view_button = new QToolButton(toolbar);
    view_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    view_button->setText("&View   ");
    view_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *view_menu = new QMenu(view_button);

    MakeToggledAction(toggle_disassembly_action, "&Disassembly", Qt::CTRL + Qt::Key_D, toggle_disassembly_panel);
    MakeToggledActionNS(toggle_autorefresh_disassembly_action, "Auto Refresh Disassembly", toggle_auto_refresh_disassembly_panel);

    MakeTriggeredAction(refresh_disassembly_action, "&Refresh", Qt::CTRL + Qt::Key_R, diassembly_refresh);
    MakeToggledAction(toggle_memory_action, "&Memory", Qt::CTRL + Qt::Key_M, toggle_memory_panel);

    QMenu *heat_map_menu = new QMenu("&Heat Map", view_menu);
    set_heat_map_off_action = new QAction(QIcon(":/buttons/HeatOff.png"), "&Off\tCtrl+H, O", this);
    set_heat_map_off_action->setCheckable(true);
    set_heat_map_off_action->setIconVisibleInMenu(false);
    set_heat_map_off_action->setToolTip("Heat Map - Off");
    connect(set_heat_map_off_action, &QAction::toggled, this, [this](bool checked)
            { if (checked) set_heat_map_off(); });

    set_heat_map_fade_action = new QAction(QIcon(":/buttons/HeatFade.png"), "&Fade\tCtrl+H, F", this);
    set_heat_map_fade_action->setCheckable(true);
    set_heat_map_fade_action->setIconVisibleInMenu(false);
    set_heat_map_fade_action->setToolTip("Heat Map - Fade");
    connect(set_heat_map_fade_action, &QAction::toggled, this, [this](bool checked)
            { if (checked) set_heat_map_fade(); });

    // set_heat_map_fade_slow_action = new QAction(QIcon(":/buttons/HeatFadeSlow.png"), "&Fade (Slow)", this);
    // set_heat_map_fade_slow_action->setCheckable(true);
    // set_heat_map_fade_slow_action->setIconVisibleInMenu(false);
    // set_heat_map_fade_slow_action->setToolTip("Heat Map - Fade (Slow)");
    // connect(set_heat_map_fade_slow_action, &QAction::toggled, this, [this](bool checked)
    //         { if (checked) set_heat_map_fade_slow(); });

    set_heat_map_persist_action = new QAction(QIcon(":/buttons/HeatPersist.png"), "&Persist\tCtrl+H, P", this);
    set_heat_map_persist_action->setCheckable(true);
    set_heat_map_persist_action->setIconVisibleInMenu(false);
    set_heat_map_persist_action->setToolTip("Heat Map - Persist");
    connect(set_heat_map_persist_action, &QAction::toggled, this, [this](bool checked)
            { if (checked) set_heat_map_persist(); });

    QShortcut *offShortcut = new QShortcut(QKeySequence("Ctrl+H, O"), this);
    connect(offShortcut, &QShortcut::activated, this, [this]()
            { this->set_heat_map_off_action->setChecked(true); });

    QShortcut *fadeShortcut = new QShortcut(QKeySequence("Ctrl+H, F"), this);
    connect(fadeShortcut, &QShortcut::activated, this, [this]()
            { this->set_heat_map_fade_action->setChecked(true); });

    QShortcut *persistShortcut = new QShortcut(QKeySequence("Ctrl+H, P"), this);
    connect(persistShortcut, &QShortcut::activated, this, [this]()
            { this->set_heat_map_persist_action->setChecked(true); });

    QShortcut *clearShortcut = new QShortcut(QKeySequence("Ctrl+H, C"), this);
    connect(clearShortcut, &QShortcut::activated, this, &DebuggerDialog::clear_heat_map);

    // MakeTriggeredAction(toggle_heat_map_action, "&Toggle", Qt::CTRL + Qt::Key_H, toggle_heat_map);
    MakeTriggeredActionNS(clear_heat_map_action, "&Clear\tCtrl+H, C", clear_heat_map);

    clear_heat_map_action->setIcon(QIcon(":/buttons/Eraser.png"));
    clear_heat_map_action->setIconVisibleInMenu(false);
    clear_heat_map_action->setToolTip("Clear Heat Map");

    QActionGroup *heat_map_group = new QActionGroup(this);
    heat_map_group->setExclusive(true);
    heat_map_group->addAction(set_heat_map_off_action);
    heat_map_group->addAction(set_heat_map_fade_action);
    // heat_map_group->addAction(set_heat_map_fade_slow_action);
    heat_map_group->addAction(set_heat_map_persist_action);

    heat_map_menu->addAction(set_heat_map_off_action);
    heat_map_menu->addAction(set_heat_map_fade_action);
    // heat_map_menu->addAction(set_heat_map_fade_slow_action);
    heat_map_menu->addAction(set_heat_map_persist_action);
    heat_map_menu->addSeparator();
    heat_map_menu->addAction(clear_heat_map_action);

    QAction *goto_label_action = new QAction("&Goto Address/Label", this);
    goto_label_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_G));
    connect(goto_label_action, &QAction::triggered, this, &DebuggerDialog::goto_label);

    QAction *show_labels_action = new QAction("&Labels", this);
    show_labels_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_L));
    connect(show_labels_action, &QAction::triggered, this, &DebuggerDialog::show_labels_dialog);

    QAction *show_breakpoints_action = new QAction("&Breakpoints", this);
    show_breakpoints_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_B));
    connect(show_breakpoints_action, &QAction::triggered, this, &DebuggerDialog::show_breakpoints_dialog);

    QAction *save_view_action = new QAction("&Save Views as Text", this);
    connect(save_view_action, &QAction::triggered, this, &DebuggerDialog::show_save_view_dialog);

    // QShortcut *swapShortcut = new QShortcut(QKeySequence("F8"), this);
    // connect(swapShortcut, &QShortcut::activated, this, &DebuggerDialog::swap_views);

    QAction *swap_view_action = new QAction("Switch &Views at Current Address", this);
    swap_view_action->setShortcut(QKeySequence(Qt::Key_F8));
    connect(swap_view_action, &QAction::triggered, this, &DebuggerDialog::swap_views);

    view_menu->addAction(show_labels_action);
    view_menu->addAction(show_breakpoints_action);
    view_menu->addAction(goto_label_action);
    view_menu->addSeparator();
    view_menu->addAction(toggle_disassembly_action);
    view_menu->addAction(refresh_disassembly_action);
    view_menu->addAction(toggle_autorefresh_disassembly_action);
    view_menu->addSeparator();
    view_menu->addAction(toggle_memory_action);
    view_menu->addMenu(heat_map_menu);
    view_menu->addSeparator();
    view_menu->addAction(swap_view_action);
    view_menu->addSeparator();
    view_menu->addAction(save_view_action);

    view_button->setMenu(view_menu);

    return view_button;
}

QToolButton *DebuggerDialog::create_settings_menu(QToolBar *toolbar)
{
    QToolButton *settings_button = new QToolButton(toolbar);
    settings_button->setToolButtonStyle(Qt::ToolButtonTextOnly);
    settings_button->setText("&Settings   ");
    settings_button->setPopupMode(QToolButton::ToolButtonPopupMode::InstantPopup);

    QMenu *settings_menu = new QMenu(settings_button);

    settings_button->setMenu(settings_menu);

    return settings_button;
}

QString DebuggerDialog::get_error_message(int reason)
{
    switch (reason)
    {
    case REGISTER_DISABLED:
        return "Cannot edit registers while CPU is running.";
        break;
    case MEMORY_READ_ONLY:
        return "Cannot edit read-only memory";
        break;
    }

    return "Unknown error";
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

    connect(status_view, &StatusView::on_edit_abort, this, [this](int reason)
            { QMessageBox::warning(this, "Edit Register", get_error_message(reason)); });

    return status_groupBox;
}

QGroupBox *DebuggerDialog::create_disassembly_group()
{
    QGroupBox *disassembly_groupBox = new QGroupBox("Disassembly", this);
    disassembly_groupBox->setMinimumWidth(400);

    QVBoxLayout *disassembly_groupBox_layout_v = new QVBoxLayout(this);

    disassembly_selector = new QComboBox(disassembly_groupBox);
    disassembly_view = new DisassemblyView(disassembly_groupBox);

    disassembly_groupBox_layout_v->addWidget(disassembly_selector);
    disassembly_groupBox_layout_v->addWidget(disassembly_view);

    disassembly_groupBox->setLayout(disassembly_groupBox_layout_v);

    connect(disassembly_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_disassembly_location);

    connect(disassembly_view, &DisassemblyView::onAddBreakpoint, this, &DebuggerDialog::add_breakpoint);
    connect(disassembly_view, &DisassemblyView::onRemoveBreakpoint, this, &DebuggerDialog::remove_breakpoint);
    connect(disassembly_view, &DisassemblyView::onAddorRemoveBreakpoint, this, &DebuggerDialog::add_or_remove_breakpoint);
    connect(disassembly_view, &DisassemblyView::onBreakpointChanged, this, &DebuggerDialog::populate_breakpoints_table);
    connect(disassembly_view, &DisassemblyView::onAutorefreshChanged, this, [this](bool value)
            {
        toggle_autorefresh_disassembly_action->blockSignals(true);
        toggle_autorefresh_disassembly_action->setChecked(value);
        toggle_autorefresh_disassembly_action->blockSignals(false); });

    connect(disassembly_view, &DisassemblyView::onShowInMemory, this, [this](offs_t address)
            {
                if(selectMemoryDeviceByAddress(address))
                {
                    memory_view->goToAddress(address); 
                } });

    return disassembly_groupBox;
}

QGroupBox *DebuggerDialog::create_memory_group()
{
    QGroupBox *memory_groupBox = new QGroupBox("Memory", this);
    memory_groupBox->setMinimumWidth(360);

    QVBoxLayout *memory_groupBox_layout_v = new QVBoxLayout(this);

    memory_selector = new QComboBox(memory_groupBox);
    memory_view = new MemoryView(memory_groupBox);

    memory_groupBox_layout_v->addWidget(memory_selector);
    memory_groupBox_layout_v->addWidget(memory_view);

    memory_groupBox->setLayout(memory_groupBox_layout_v);

    connect(memory_view, &MemoryView::on_heat_map_enabled_change, this, [this](bool enabled)
            {
                set_heat_map_off_action->blockSignals(true);
                set_heat_map_fade_action->blockSignals(true);
                set_heat_map_persist_action->blockSignals(true);
    
                set_heat_map_off_action->setChecked(!enabled);
                set_heat_map_fade_action->setChecked(enabled);
                set_heat_map_persist_action->setChecked(enabled);

                set_heat_map_off_action->blockSignals(false);
                set_heat_map_fade_action->blockSignals(false);
                set_heat_map_persist_action->blockSignals(false);

                settings->showHeatMap = enabled;
                save_settings(settings); });

    connect(memory_view, &MemoryView::on_heat_map_change, this, [this](bool enabled, int decay)
            {
                set_heat_map_off_action->blockSignals(true);
                set_heat_map_fade_action->blockSignals(true);
                set_heat_map_persist_action->blockSignals(true);

                set_heat_map_off_action->setChecked(!enabled);
                set_heat_map_fade_action->setChecked(enabled && (decay == FADE_SPEED));
                set_heat_map_persist_action->setChecked(enabled && (decay == PERSIST_SPEED));

                set_heat_map_off_action->blockSignals(false);
                set_heat_map_fade_action->blockSignals(false);
                set_heat_map_persist_action->blockSignals(false);

                settings->showHeatMap = enabled;
                settings->heatMapDecay = decay;

                save_settings(settings); });

    connect(memory_view, &MemoryView::on_show_in_disassembly, this, [this](offs_t address)
            {
                if(selectDisassemblyDeviceByAddress(address))
                {
                    disassembly_view->setSelected(address); 
                } });

    connect(memory_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_memory_location);

    connect(memory_view, &MemoryView::on_edit_abort, this, [this](int reason)
            { QMessageBox::warning(this, "Edit Memory", get_error_message(reason)); });

    return memory_groupBox;
}

void DebuggerDialog::swap_views()
{
    if (memory_view->hasFocus())
    {
        int address = memory_view->getSelectedAddress();
        if (address > -1)
        {
            if (selectDisassemblyDeviceByAddress(address))
            {
                disassembly_view->setSelected(address);
                disassembly_view->setFocus();
            }
        }
    }
    else if (disassembly_view->hasFocus())
    {
        int address = disassembly_view->getSelectedAddress();
        if (address > -1)
        {
            if (selectMemoryDeviceByAddress(address))
            {
                memory_view->goToAddress(address);
                memory_view->setFocus();
            }
        }
    }
}

void DebuggerDialog::goto_address(offs_t address)
{
    memory_mapped_device *device = emu_ptr->get_block_device(address);
    if (!device)
        return;
    selectDisassemblyDeviceByAddress(device->get_start());
    disassembly_view->setSelected(address);
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

    resize(QSize(985, 721));

    // setFixedSize(QSize(985, 721));
    setWindowTitle("Debugger");
}

#endif // DEBUGGER_UI_H
