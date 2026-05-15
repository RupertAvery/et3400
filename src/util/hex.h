#ifndef HEX_H
#define HEX_H

#include "io.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>

bool is_hex(QString filename);

class HexFile
{
public:
    static bool Read(QString file, std::vector<data_block> *blocks);
    static bool Write(QString file, std::vector<data_block> *blocks);

private:
    static uint16_t WriteRecords(QTextStream &out, std::vector<data_block> *blocks);
    static void WriteTerminator(QTextStream &out);
};

#endif // HEX_H
