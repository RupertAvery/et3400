#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H
#include <QLineEdit>

int toInt(QLineEdit *edit, bool &ok);
char *load_bin(QString path, size_t &size, bool &success);
char *load_srec(QString path, size_t &size, bool &success);
char *load_file(QString path, size_t &size, bool &success);

#endif // COMMON_UTIL_H
