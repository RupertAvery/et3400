#include "util.h"

int toInt(QLineEdit *edit, bool &ok)
{
    QString text = edit->text();

    if (!text.startsWith("$"))
    {
        text = QString("$").append(text);
    }

    return text.replace("$", "0x").toInt(&ok, 0);
}

