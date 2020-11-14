#include "srec.h"

srec_block make_block(int bytecount, int address, uint8_t *data)
{
    srec_block block = {bytecount, address, data};
    return block;
}

bool SrecReader::Read(QString path, std::vector<srec_block> *blocks)
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
            bool success = false;
            int bytecount = line.mid(2, 2).toUInt(&success, 16);
            QString addr;
            QString data;
            QString checksum;
            QString type = line.mid(0, 2);

            if (type == "S1")
            {
                int dataLength = bytecount - 3;

                addr = line.mid(4, 4);
                data = line.mid(8, dataLength * 2);

                int addrVal = addr.toUInt(&success, 16);

                uint8_t *buffer = (uint8_t *)malloc(bytecount - 3);

                for (int ptr = 0; ptr < dataLength; ptr++)
                {
                    buffer[ptr] = data.mid(ptr * 2, 2).toUInt(&success, 16);
                }

                blocks->push_back(make_block(bytecount, addrVal, buffer));

                checksum = line.mid(bytecount * 2 + 2, 2);
                int checksumActual = checksum.toUInt(&success, 16);
            }
        }
    }

    return true;
}

bool SrecReader::Write(QString path, std::vector<srec_block> *blocks)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        //QMessageBox::information(0, "error", file.errorString());
        return false;
    }

    QTextStream out(&file);

    std::vector<srec_block>::iterator block = blocks->begin();

    while (block != blocks->end())
    {
        uint8_t checksum = (*block).bytecount + (*block).address >> 8 & 0xFF + (*block).address & 0xFF;
        out << "S1"
            << QString("%1").arg((*block).bytecount, 2, 16, QChar('0'))
            << QString("%1").arg((*block).address, 4, 16, QChar('0'));
        for (int i = 0; i < (*block).bytecount; i++)
        {
            checksum += (*block).data[i];
            out << QString("%1").arg((*block).data[i], 2, 16, QChar('0'));
        }
        checksum = ~(checksum & 0xFF);
        out << QString("%1").arg(checksum, 2, 16, QChar('0'));
        out << "\r\n";
        block++;
    }

    return true;
}