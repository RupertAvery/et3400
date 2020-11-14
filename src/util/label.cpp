#include "label.h"
#include "../util/csv.h"

void LabelReader::Write(QString path, std::vector<Label> *labels, bool &success)
{
    std::vector<Label>::iterator current = labels->begin();

    QFile file(path);

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        success = false;
        return;
    }

    QTextStream out(&file);

    while (current != labels->end())
    {
        out << QString("%1").arg((*current).start, 0, 16).toUpper() << ","
            << QString("%1").arg((*current).end, 0, 16).toUpper() << ","
            << ((*current).type == LabelType::COMMENT ? QString("COMMENT") : ((*current).type == LabelType::DATA ? QString("DATA") : QString("ASSEMBLY")))
            << QString("\"%1\"").arg((*current).comment);

        current++;
    }

    out.flush();
    file.close();
    success = true;

}
std::vector<Label> *LabelReader::Read(QString path, bool &success)
{
    std::vector<Label> *labels = new std::vector<Label>;
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        success = false;
        return labels;
    }

    QTextStream in(&file);
    QStringList parts;

    while (readCSVRow(in, &parts))
    {
        bool bStatus = false;
        uint32_t start = parts.at(0).toUInt(&bStatus, 16);
        uint32_t end = parts.at(1).toUInt(&bStatus, 16);
        LabelType type = parts.at(2) == "ASSEMBLY" ? ASSEMBLY : parts.at(2) == "DATA" ? DATA : COMMENT;
        QString comment = parts.at(3);
        labels->push_back(Label{start, end, type, comment});
    }

    success = true;
    return labels;
}