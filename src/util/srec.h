#ifndef SREC_H
#define SREC_H

#include "io.h"
#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>

bool is_srec(QString filename);


class SrecFile
{
public:
    static bool Read(QString file, std::vector<data_block> *blocks);
    static bool Write(QString file, QString header, std::vector<data_block> *blocks, uint16_t startAddress);

private:
    static void WriteHeader(QTextStream &out, QString header);
    static uint16_t WriteRecords(QTextStream &out, std::vector<data_block> *blocks);
    static void WriteRecordCount(QTextStream &out, uint16_t recordCount);
    static void WriteTerminator(QTextStream &out, uint16_t startAddress);
};

#endif // SREC_H
