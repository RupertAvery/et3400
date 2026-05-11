#include "util.h"
#include <QString>
#include <QFile>
#include <filesystem>
#include <algorithm>
#include "../util/srec.h"
#include "../util/log.h"

int toInt(QLineEdit *edit, bool &ok)
{
    QString text = edit->text();

    if (!text.startsWith("$"))
    {
        text = QString("$").append(text);
    }

    return text.replace("$", "0x").toInt(&ok, 0);
}

char *load_bin(QString path, size_t &size, bool &success)
{
    char *buffer = nullptr;

    QFile file(path);
    if (!file.open(QIODevice::ReadOnly))
    {
        success = false;
        // throw -10010;
    }
    else
    {
        size = file.size();
        buffer = (char *)malloc(size);
        file.read(buffer, size);
    }
    success = true;

    return buffer;
}

char *load_srec(QString path, size_t &size, bool &success)
{
    char *buffer = nullptr;

    std::vector<srec_block> *blocks = new std::vector<srec_block>;

    LOG_DEBUG << "Loading SREC";

    if (SrecFile::Read(path, blocks))
    {
        uint16_t lowest_address = 0xFFFF;
        uint16_t highest_address = 0x0000;

        // determine bounds
        for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
        {
            uint16_t end = (it->address + it->length - 1);

            LOG_DEBUG << " start: " << it->address;
            LOG_DEBUG << " end: " << end;

            lowest_address = std::min(lowest_address, it->address);
            highest_address = std::max(end, highest_address);
        }

        size = highest_address - lowest_address + 1;

        LOG_DEBUG << " highest_address: " << highest_address;
        LOG_DEBUG << " lowest_address: " << lowest_address;
        LOG_DEBUG << " size: " << size;

        buffer = (char *)malloc(size);

        // copy blocks to buffer
        for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
        {
            uint16_t offset = it->address - lowest_address;
            memcpy(&buffer[offset], it->data, it->length);
        }
    }

    // clean up
    for (std::vector<srec_block>::iterator it = blocks->begin(); it != blocks->end(); ++it)
    {
        free(it->data);
    }

    delete blocks;

    return buffer;
}

char *load_file(QString path, size_t &size, bool &success)
{
    char *buffer = nullptr;

    if (is_srec(path))
    {
        buffer = load_srec(path, size, success);
    }
    else
    {
        buffer = load_bin(path, size, success);
    }

    if (success)
    {
        LOG_DEBUG << "Loaded " << size << " bytes from " << path;
    }

    return buffer;
}