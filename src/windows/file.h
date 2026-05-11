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

class File
{
public:
    static void load_rom_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings);
    static void load_ram_dialog(QWidget *parent, et3400emu *emu_ptr, LoadSettings &settings);
    static void save_ram_dialog(QWidget *parent, et3400emu *emu_ptr, SaveSettings &settings);

    static size_t load_memory(QString path, QString device_name, et3400emu *emu_ptr, uint16_t address, bool &success);

    static QString error;
};

#endif // FILE_H