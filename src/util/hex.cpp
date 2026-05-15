#include "hex.h"
#include "log.h"

/*
 * Functions for reading and writing the Intel Hex file format
 */

bool is_hex(QString filename)
{
    return filename.endsWith(".hex", Qt::CaseInsensitive);
}

/*
: Start code
00 - byte count
0000 - address
00 - type


*/

const int bytecount_start = 1;
const int address_start = 3;
const int type_start = 7;
const int data_start = 9;

bool HexFile::Read(QString path, std::vector<data_block> *blocks)
{
    QFile file(path);

    if (!file.open(QIODevice::ReadOnly))
    {
        LOG_DEBUG << "Failed to open";
        // QMessageBox::information(0, "error", file.errorString());
        return false;
    }

    int line_num = 0;
    bool success = true;

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        LOG_DEBUG << "Reading Line " << line_num;

        if (line.length() > 0)
        {
            // Check if start code exists
            if (line.left(1) != ":")
            {
                LOG_DEBUG << "Failed parsing start code";
                success = false;
                break;
            }

            uint16_t bytecount = line.mid(bytecount_start, 2).toUInt(&success, 16);

            LOG_DEBUG << "bytecount: " << bytecount;

            if (!success)
            {
                LOG_DEBUG << "Failed parsing byte count";
                break;
            }

            uint16_t address = line.mid(address_start, 4).toUInt(&success, 16);

            LOG_DEBUG << "address: " << address;

            if (!success)
            {
                LOG_DEBUG << "Failed parsing address";
                break;
            }

            int type = line.mid(type_start, 2).toUInt(&success, 16);

            LOG_DEBUG << "type: " << type;

            if (!success)
            {
                LOG_DEBUG << "Failed parsing type";
                break;
            }

            if (type == 0) // data type
            {
                uint8_t *buffer = (uint8_t *)malloc(bytecount);

                for (int idx = 0; idx < bytecount; idx++)
                {
                    int offset = data_start + (idx * 2);

                    buffer[idx] = line.mid(offset, 2).toUInt(&success, 16);

                    if (!success)
                    {
                        LOG_DEBUG << "error at offset " << offset;
                        break;
                    }
                }

                if (!success)
                {
                    LOG_DEBUG << "Failed parsing data";
                    break;
                }

                blocks->push_back(data_block{bytecount, address, buffer});

                uint8_t checksum = line.right(2).toUInt(&success, 16) & 0xFF;

                if (!success)
                {
                    LOG_DEBUG << "Failed parsing checksum";
                    break;
                }
            }
        }

        line_num++;
    }

    if (!success)
    {
        LOG_DEBUG << "Error reading HEX file at line " << line_num;
    }

    return true;
}

bool HexFile::Write(QString path, std::vector<data_block> *blocks)
{
    QFile file(path);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
    {
        // QMessageBox::information(0, "error", file.errorString());
        return false;
    }

    QTextStream out(&file);

    HexFile::WriteRecords(out, blocks);
    HexFile::WriteTerminator(out);

    return true;
}

uint16_t HexFile::WriteRecords(QTextStream &out, std::vector<data_block> *blocks)
{
    std::vector<data_block>::iterator block = blocks->begin();
    uint16_t recordCount = 0;

    while (block != blocks->end())
    {
        uint8_t checksum = ((*block).length)                  // byte count
                           + (((*block).address >> 8) & 0xFF) // address hi
                           + ((*block).address & 0xFF)        // address lorecord type (data)
                           + 0x00;                            // record type (data)

        out << ":"                                                              // write start code
            << QString("%1").arg((*block).length, 2, 16, QChar('0')).toUpper()  // write byte count
            << QString("%1").arg((*block).address, 4, 16, QChar('0')).toUpper() // write address
            << QString("%1").arg(0, 2, 16, QChar('0')).toUpper();               // write record type (0)

        for (int i = 0; i < (*block).length; i++)
        {
            checksum += (*block).data[i];                                            // sum data bytes
            out << QString("%1").arg((*block).data[i], 2, 16, QChar('0')).toUpper(); // write data bytes
        }

        checksum = (~checksum + 1) & 0xFF;                               // Two's complement
        out << QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper(); // write checksum
        out << "\r\n";

        block++;
        recordCount++;
    }

    return recordCount;
}

void HexFile::WriteTerminator(QTextStream &out)
{
    out << ":"
        << QString("%1").arg(0, 2, 16, QChar('0')).toUpper()  // byte count (0)
        << QString("%1").arg(0, 4, 16, QChar('0')).toUpper()  // address (0)
        << QString("%1").arg(1, 2, 16, QChar('0')).toUpper(); // record type (1)

    uint8_t checksum = (~1 + 1) & 0xFF;

    out << QString("%1").arg(checksum, 2, 16, QChar('0')).toUpper();
    out << "\r\n";
}