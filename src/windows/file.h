#ifndef FILE_H
#define FILE_H

#include "../emu/et3400.h"
#include <QObject>
#include <QString>
#include <QFileDialog>
#include <QMessageBox>
#include <vector>
#include "load.h"
#include "save.h"
#include "../util/snap.h"

class File
{
public:
    static void load_rom_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings, QString &dir);

    static void load_ram_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings, QString &dir, bool clear_ram);
    static void save_ram_dialog(QWidget *parent, et3400emu *emu_ptr, SaveSettings &settings, QString &dir);

    static void load_labels_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir);
    static void save_labels_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir);
    static void save_labels_dialog(QWidget *parent, et3400emu *emu_ptr, uint32_t start, uint32_t end, QString &dir);

    static void load_breakpoint_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir);
    static void save_breakpoint_dialog(QWidget *parent, et3400emu *emu_ptr, QString &dir);

    static void save_memory_mapped_devices_dialog(QWidget *parent, et3400emu *emu_ptr, memory_mapped_device *dasm, memory_mapped_device *mem, SnapFileSettings settings);

    static void load_labels(QString fileName, et3400emu *emu_ptr, bool &success);
    static size_t load_memory(QString path, QString device_name, et3400emu *emu_ptr, uint16_t address, bool &success);

    static QString error;

private:
    static void clear_ram(et3400emu *emu_ptr);
};

#endif // FILE_H
