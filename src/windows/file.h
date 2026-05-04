#ifndef FILE_H
#define FILE_H

#include "../emu/et3400.h"
#include <QObject>
#include <QString>
#include <QFileDialog>
#include <vector>

class File
{
public:
    static void load_ram(QWidget *parent, et3400emu *emu_ptr);
    static void save_ram(QWidget *parent, et3400emu *emu_ptr);
    static void load(et3400emu *emu_ptr, QString path);
};

#endif // FILE_H