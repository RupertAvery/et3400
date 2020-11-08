#ifndef MAP_H
#define MAP_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>

enum map_type {
    CODE,
    DATA,
    COMMENT
};

struct Map
{
    uint32_t start;
    uint32_t end;
    map_type type;
    QString comment;
};

class MapReader
{
public:
    static bool Read(QString file, std::vector<Map> *maps);
};

#endif // MAP_H
