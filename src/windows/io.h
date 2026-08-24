#ifndef IO_DIALOG_H
#define IO_DIALOG_H

#include <QVariant>
#include <QApplication>
#include <QAction>
#include <QCloseEvent>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QToolBar>
#include <QVBoxLayout>
#include <QWidget>
#include "../dev/memory_map.h"
#include "../widgets/dip_array.h"
#include "../widgets/led_array.h"
#include "../emu/et3400.h"
#include "../dev/io_dev.h"
#include "../util/settings.h"
#include "io_settings.h"

class IODialog : public QDialog
{
    Q_OBJECT

public:
    IODialog(QWidget *parent = nullptr);
    void setEmu(et3400emu *emu, Settings *settings);
    void refresh();

signals:
    // emitted after the LED/DIP devices have been re-registered at new addresses
    void devices_changed();

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    QVBoxLayout *mainLayout = nullptr;
    QToolBar *toolbar = nullptr;
    QAction *settings_action = nullptr;
    LEDArray *led_array = nullptr;
    DIPArray *dip_array = nullptr;
    et3400emu *emu = nullptr;
    Settings *settings = nullptr;
    io_device *led_device = nullptr;
    io_device *dip_device = nullptr;

    void create_devices();
    void destroy_devices();
    bool address_in_use(offs_t address);
    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
    void show_io_settings();
};

#endif // IO_DIALOG_H
