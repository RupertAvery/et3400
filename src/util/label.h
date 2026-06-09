#ifndef LABEL_READER_H
#define LABEL_READER_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>

enum LabelType
{
    COMMENT = 0,
    ASSEMBLY = 1,
    DATA = 2,
    LABEL_ERROR = -1
};

struct Label
{
    uint32_t start;
    uint32_t end;
    LabelType type;
    QString comment;
};

class LabelReader
{
public:
    static void Write(QString path, std::vector<Label> *labels, bool &success);
    static std::vector<Label> *Read(QString file, bool &success);
};



#endif // LABEL_H
