#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>

struct srec_block
{
    int bytecount;
    int address;
    uint8_t *data;
};

class Srec
{
public:
    static bool Read(QString file, std::vector<srec_block> *blocks);
};