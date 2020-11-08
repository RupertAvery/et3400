#include "debugger.h"

DebuggerDialog::DebuggerDialog() : DebuggerDialog(nullptr)
{
}

DebuggerDialog::DebuggerDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    // label = new QLabel("Clock Rate");

    // QWidget *mainwidget = new QWidget;
    // QVBoxLayout *layout = new QVBoxLayout(mainwidget);

    // slider = new QSlider(Qt::Horizontal);
    // slider->setFocusPolicy(Qt::StrongFocus);
    // slider->setTickPosition(QSlider::TicksBothSides);
    // slider->setTickInterval(10);
    // slider->setSingleStep(1);
    // slider->setMinimum(1);
    // slider->setMaximum(400);

    // connect(slider, &QSlider::valueChanged, this, &SettingsDialog::setClockRate);

    // layout->addWidget(label);
    // layout->addWidget(slider);
    // mainwidget->setFixedHeight(150);
    // MemoryLocation ram("RAM", 0x0000);
    // MemoryLocation rom("ROM", 0xFC00);

    QToolBar *toolbar = new QToolBar(this);
    QLabel *memory_label = new QLabel("Memory");

    memory_selector = new QComboBox(toolbar);
    memory_selector->addItem("RAM");
    memory_selector->addItem("ROM");

    QGroupBox *memory_groupBox = new QGroupBox("Memory", this);
    QGroupBox *disassembly_groupBox = new QGroupBox("Disassembly", this);

    toolbar->addWidget(memory_label);
    toolbar->addWidget(memory_selector);

    memmory_scrollbar = new QScrollBar(Qt::Orientation::Vertical);
    disassembly_scrollbar = new QScrollBar(Qt::Orientation::Vertical);

    memory_view = new MemoryView(this);
    disassembler_view = new DisassemblyView(this);

    QHBoxLayout *memory_groupBox_layout = new QHBoxLayout(this);
    memory_groupBox_layout->addWidget(memory_view);
    memory_groupBox_layout->addWidget(memmory_scrollbar);
    memory_groupBox_layout->setMargin(10);
    memory_groupBox->setLayout(memory_groupBox_layout);

    QHBoxLayout *disassembly_groupBox_layout = new QHBoxLayout(this);
    disassembly_groupBox_layout->addWidget(disassembler_view);
    disassembly_groupBox_layout->addWidget(disassembly_scrollbar);
    disassembly_groupBox_layout->setMargin(10);
    disassembly_groupBox->setLayout(disassembly_groupBox_layout);

    QWidget *panels = new QWidget(this);
    QHBoxLayout *panels_layout = new QHBoxLayout(this);
    panels_layout->addWidget(memory_groupBox);
    panels_layout->addWidget(disassembly_groupBox);
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
    connect(disassembler_view, &DisassemblyView::on_scroll, this, &DebuggerDialog::update_disassembly_scrollbar);
    connect(memory_view, &MemoryView::on_size, this, &DebuggerDialog::update_memory_scrollbar_max);
    connect(memory_selector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &DebuggerDialog::select_memory_location);

    setFixedSize(QSize(985, 721));
    setWindowTitle("Debugger");
}

DebuggerDialog::~DebuggerDialog()
{
    qDebug() << "Destroying debugger";
    delete memory_view;
    delete memmory_scrollbar;
    delete memory_selector;
    delete disassembler_view;
    delete disassembly_scrollbar;
    qDebug() << "Destroying debugger done";
}

void DebuggerDialog::select_memory_location(int index)
{
    QVariant v = memory_selector->itemText(index);
    QString s = v.value<QString>();

    qDebug() << v;

    int sstart = 0;
    if (s == "RAM")
    {
        sstart = 0x0000;
    }
    else if (s == "ROM")
    {
        sstart = 0xFC00;
    }

    memory_mapped_device *device = emu_ptr->memory_map->get_block_device(sstart);
    int start = device->get_start();
    int end = device->get_end();
    uint8_t *memory = device->get_mapped_memory();

    memory_view->set_range(start, end, memory);
}

void DebuggerDialog::update_memory_scrollbar(int value)
{
    memmory_scrollbar->setValue(memmory_scrollbar->value() - value);
}

void DebuggerDialog::update_memory_scrollbar_max(int value)
{
    memmory_scrollbar->setMinimum(0);
    memmory_scrollbar->setMaximum(value);
}

void DebuggerDialog::update_disassembly_scrollbar(int value)
{
    disassembly_scrollbar->setValue(disassembly_scrollbar->value() - value);
}

void DebuggerDialog::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;
    memory_view->set_emulator(emu);
    disassembler_view->set_emulator(emu);

    select_memory_location(0);

    memory_mapped_device *device = emu_ptr->memory_map->get_block_device(0xFC00);
    int start = device->get_start();
    int end = device->get_end();
    uint8_t *memory = device->get_mapped_memory();

    disassembler_view->set_range(start, end, memory);
}

void DebuggerDialog::memory_slider_moved(int value)
{
    memory_view->scrollTo(value);
}

void DebuggerDialog::disassembly_slider_moved(int value)
{
    disassembler_view->scrollTo(value);
}

void DebuggerDialog::keyPressEvent(QKeyEvent *event)
{

    switch (event->key())
    {
    case Qt::Key_F4:
        emu_ptr->halt();
        break;
    case Qt::Key_F5:
        emu_ptr->resume();
        break;
    case Qt::Key_F10:
        emu_ptr->step();
        break;
    case Qt::Key_Escape:
        emu_ptr->reset();
        break;
    }
};

void DebuggerDialog::keyReleaseEvent(QKeyEvent *event){

};