#include "map.h"

Map make_map(uint32_t start, uint32_t end, map_type type, QString comment)
{
    Map _map = {start, end, type, comment};
    return _map;
}

bool MapReader::Read(QString path, std::vector<Map> *maps)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        //QMessageBox::information(0, "error", file.errorString());
        return false;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.length() > 0)
        {
            QStringList parts = line.split(QLatin1Char(','));

            bool bStatus = false;
            uint32_t start = parts.at(0).toUInt(&bStatus, 16);
            uint32_t end = parts.at(1).toUInt(&bStatus, 16);

            map_type type = parts.at(2) == "ASSEMBLY" ? ASSEMBLY : parts.at(2) == "DATA" ? DATA : COMMENT;

            QString comment;
            if (type == COMMENT)
            {
                comment = parts.at(3);
            }

            maps->push_back(make_map(start, end, type, comment));
        }
    }

    return true;
}