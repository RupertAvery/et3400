#ifndef CSV_H
#define CSV_H

#include <QString>
#include <QFile>
#include <QTextStream>
#include <vector>

bool readCSVRow(QTextStream &in, QStringList *row);

#endif // CSV_H
