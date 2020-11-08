#ifndef MEMORY_LOCATION_H
#define MEMORY_LOCATION_H

#include <QString>
#include <QMetaType>
#include "../cpu/defs.h"

struct MyStruct
{
    int i;
};

Q_DECLARE_METATYPE(MyStruct)

class MemoryLocation
{
public:
    MemoryLocation();
    ~MemoryLocation();
    MemoryLocation(const MemoryLocation &memorylocation)
    {
        this->name = memorylocation.name;
        this->start = memorylocation.start;
    }
    //MemoryLocation &operator=(const MemoryLocation &);

    MemoryLocation(QString name, offs_t start);

    QString get_name();
    offs_t get_start();

private:
    QString name;
    offs_t start;
};

MemoryLocation::MemoryLocation(QString name, offs_t start)
{
    this->name = name;
    this->start = start;
}

QString MemoryLocation::get_name()
{
    return this->name;
}

offs_t MemoryLocation::get_start()
{
    return this->start;
}

//Q_DECLARE_METATYPE(MemoryLocation)
Q_DECLARE_METATYPE(MemoryLocation)

#endif // MEMORY_LOCATION_H