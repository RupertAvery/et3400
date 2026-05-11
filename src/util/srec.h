#ifndef SREC_H
#define SREC_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>

bool is_srec(QString filename);

struct srec_block
{
    uint16_t length;
    uint16_t address;
    uint8_t *data;
};

class SrecFile
{
public:
    static bool Read(QString file, std::vector<srec_block> *blocks);
    static bool Write(QString file, QString header, std::vector<srec_block> *blocks, uint16_t startAddress);
    static void Free(std::vector<srec_block> *blocks);

private:
    static void WriteHeader(QTextStream &out, QString header);
    static uint16_t WriteRecords(QTextStream &out, std::vector<srec_block> *blocks);
    static void WriteRecordCount(QTextStream &out, uint16_t recordCount);
    static void WriteTerminator(QTextStream &out, uint16_t startAddress);
};

#endif // SREC_H
