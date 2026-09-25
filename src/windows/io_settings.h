#ifndef IO_SETTINGS_DIALOG_H
#define IO_SETTINGS_DIALOG_H

#include <QVariant>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>
#include <functional>
#include "../dev/memory_map.h"

struct IOSettingsInfo
{
    offs_t led_address;
    offs_t dip_address;
};

class IOSettingsDialog : public QDialog
{
    Q_OBJECT

public:
    IOSettingsDialog(QWidget *parent = nullptr);

    void setIOSettings(IOSettingsInfo info);
    IOSettingsInfo getIOSettings();

    // returns true if the address is claimed by a device other than the LED/DIP arrays
    std::function<bool(offs_t)> address_in_use;

private:
    QVBoxLayout *mainLayout;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLabel *led_label;
    QLineEdit *led_edit;
    QLabel *dip_label;
    QLineEdit *dip_edit;
    QLabel *description_label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
    void validate();
};

#endif // IO_SETTINGS_DIALOG_H
