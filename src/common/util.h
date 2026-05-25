#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H
#include <QLineEdit>
#include <QString>

QString getVersion();
int toInt(QLineEdit *edit, bool &ok);
QString toHex(int value, int width = 4);
QString toBin(int value, int width = 8);
char *load_bin(QString path, size_t &size, bool &success);
char *load_srec(QString path, size_t &size, bool &success);
char *load_file(QString path, size_t &size, bool &success);

#endif // COMMON_UTIL_H
