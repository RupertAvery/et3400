#include "io.h"
#include "../common/util.h"
#include <QMessageBox>

IODialog::IODialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    setFixedSize(QSize(280, 180));
    setWindowTitle("Input/Output");
}

void IODialog::setEmu(et3400emu *emu, Settings *settings)
{
    this->emu = emu;
    this->settings = settings;

    create_devices();
}

void IODialog::create_devices()
{
    led_device = new io_device("LED Array", settings->ioLEDAddress, 1, false);
    dip_device = new io_device("DIP Array", settings->ioDIPAddress, 1, true);

    // the widgets seed the new devices with the state they are currently showing
    led_array->set_device(led_device);
    dip_array->set_device(dip_device);

    emu->memory_map->map(led_device);
    emu->memory_map->map(dip_device);
}

void IODialog::destroy_devices()
{
    if (emu == nullptr)
        return;

    emu->memory_map->unmap(led_device);
    emu->memory_map->unmap(dip_device);

    led_array->set_device(nullptr);
    dip_array->set_device(nullptr);

    delete led_device;
    delete dip_device;

    led_device = nullptr;
    dip_device = nullptr;
}

bool IODialog::address_in_use(offs_t address)
{
    for (auto *device : emu->memory_map->get_block_devices())
    {
        if (device == led_device || device == dip_device)
            continue;

        if (address >= device->get_start() && address <= device->get_end())
            return true;
    }

    return false;
}

void IODialog::refresh()
{
    led_array->update_display();
}

void IODialog::closeEvent(QCloseEvent *event)
{
    destroy_devices();

    emu = nullptr;

    QDialog::closeEvent(event);
}

void IODialog::setupUi(QDialog *Dialog)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(0);
    mainLayout->setMargin(0);

    toolbar = new QToolBar(this);
    toolbar->setMovable(false);
    toolbar->setToolButtonStyle(Qt::ToolButtonTextOnly);

    settings_action = toolbar->addAction("Settings");

    led_array = new LEDArray(this);
    dip_array = new DIPArray(this);

    QVBoxLayout *device_layout = new QVBoxLayout;
    device_layout->setSpacing(10);
    device_layout->setContentsMargins(10, 10, 10, 10);
    device_layout->addWidget(led_array, 0, Qt::AlignLeft | Qt::AlignTop);
    device_layout->addWidget(dip_array, 0, Qt::AlignLeft | Qt::AlignTop);
    device_layout->addStretch(1);

    mainLayout->addWidget(toolbar);
    mainLayout->addLayout(device_layout);

    connect(settings_action, &QAction::triggered, this, &IODialog::show_io_settings);

} // setupUi

void IODialog::show_io_settings()
{
    if (emu == nullptr)
        return;

    IOSettingsDialog *io_settings_dialog = new IOSettingsDialog(this);

    io_settings_dialog->address_in_use = [this](offs_t address)
    {
        return address_in_use(address);
    };

    io_settings_dialog->setIOSettings(IOSettingsInfo({(offs_t)settings->ioLEDAddress, (offs_t)settings->ioDIPAddress}));

    if (io_settings_dialog->exec() == QDialog::Accepted)
    {
        IOSettingsInfo info = io_settings_dialog->getIOSettings();

        if (info.led_address != settings->ioLEDAddress || info.dip_address != settings->ioDIPAddress)
        {
            settings->ioLEDAddress = info.led_address;
            settings->ioDIPAddress = info.dip_address;

            destroy_devices();
            create_devices();

            save_settings(settings);

            emit devices_changed();
        }
    }

    io_settings_dialog->deleteLater();
}

void IODialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));

} // retranslateUi
