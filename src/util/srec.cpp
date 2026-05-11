#include "srec.h"

bool is_srec(QString filename)
{
    return filename.endsWith(".obj", Qt::CaseInsensitive) || filename.endsWith(".s19", Qt::CaseInsensitive);
}

void SrecFile::Free(std::vector<srec_block> *blocks)
{
    for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
    {
        free(it->data);
    }

    delete blocks;
}

bool SrecFile::Read(QString path, std::vector<srec_block> *blocks)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        // QMessageBox::information(0, "error", file.errorString());
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
                uint16_t length = bytecount - 3;

                addr = line.mid(4, 4);
                data = line.mid(8, length * 2);

                uint16_t address = addr.toUInt(&success, 16);

                uint8_t *buffer = (uint8_t *)malloc(length);

                for (int ptr = 0; ptr < length; ptr++)
                {
                    buffer[ptr] = data.mid(ptr * 2, 2).toUInt(&success, 16);
                }

                blocks->push_back(srec_block{length, address, buffer});

                checksum = line.mid(bytecount * 2 + 2, 2);

                int checksumActual = checksum.toUInt(&success, 16);
            }
        }
    }

    return true;
}

bool SrecFile::Write(QString path, QString header, std::vector<srec_block> *blocks, uint16_t startAddress)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        // QMessageBox::information(0, "error", file.errorString());
        return false;
    }

    QTextStream out(&file);

    SrecFile::WriteHeader(out, header);
    uint16_t recordCount = SrecFile::WriteRecords(out, blocks);
    SrecFile::WriteRecordCount(out, recordCount);
    SrecFile::WriteTerminator(out, startAddress);

    return true;
}

void SrecFile::WriteHeader(QTextStream &out, QString header)
{
    size_t headerLength = header.length();

    out << "S0"
        << QString("%1").arg(headerLength + 3, 2, 16, QChar('0')).toUpper()
        << QString("%1").arg(0, 4, 16, QChar('0')).toUpper();

    uint8_t checksum = headerLength + 3;

    for (int i = 0; i < headerLength; i++)
    {
        checksum += header.at(i).unicode();
        out << QString("%1").arg(header.at(i).unicode(), 2, 16, QChar('0')).toUpper();
    }

    checksum = ~(checksum & 0xFF);
    out << QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();
    out << "\r\n";
}

uint16_t SrecFile::WriteRecords(QTextStream &out, std::vector<srec_block> *blocks)
{
    std::vector<srec_block>::iterator block = blocks->begin();
    uint16_t recordCount = 0;

    while (block != blocks->end())
    {
        uint8_t checksum = ((*block).length + 3) + (((*block).address >> 8) & 0xFF) + ((*block).address & 0xFF);

        out << "S1"
            << QString("%1").arg((*block).length + 3, 2, 16, QChar('0')).toUpper()
            << QString("%1").arg((*block).address, 4, 16, QChar('0')).toUpper();

        for (int i = 0; i < (*block).length; i++)
        {
            checksum += (*block).data[i];
            out << QString("%1").arg((*block).data[i], 2, 16, QChar('0')).toUpper();
        }

        checksum = ~(checksum & 0xFF);
        out << QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();
        out << "\r\n";
        block++;
        recordCount++;
    }

    return recordCount;
}

void SrecFile::WriteRecordCount(QTextStream &out, uint16_t recordCount)
{
    out << "S5"
        << QString("%1").arg(3, 2, 16, QChar('0')).toUpper()
        << QString("%1").arg(recordCount, 4, 16, QChar('0')).toUpper();

    uint8_t checksum = 3 + ((recordCount >> 8) & 0xFF) + (recordCount & 0xFF);
    checksum = ~(checksum & 0xFF);

    out << QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();
    out << "\r\n";
}

void SrecFile::WriteTerminator(QTextStream &out, uint16_t startAddress)
{
    out << "S9"
        << QString("%1").arg(3, 2, 16, QChar('0')).toUpper()
        << QString("%1").arg(startAddress, 4, 16, QChar('0')).toUpper();

    uint8_t checksum = 3 + ((startAddress >> 8) & 0xFF) + (startAddress & 0xFF);
    checksum = ~(checksum & 0xFF);

    out << QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();
    out << "\r\n";
}