#ifndef DEVICE_H
#define DEVICE_H

#include "../dev/address_decoder.h"

#include <QVariant>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRegularExpressionValidator>
#include <QRadioButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>
#include "../dev/memory_map.h"

enum DeviceDialogMode
{
    AddDevice,
    EditDevice
};

struct DeviceInfo
{
    QString name;
    BitPattern bit_pattern;
};

class DeviceDialog : public QDialog
{
    Q_OBJECT

public:
    DeviceDialog();

    void setDeviceInfo(DeviceInfo label, DeviceDialogMode mode);
    DeviceInfo getDeviceInfo();
	std::function<bool(offs_t, offs_t)> has_collision;

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;

    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLabel *name_label;
    QLabel *address_label;
    QLabel *start_label;
    QLabel *end_label;
    QLineEdit *name_edit;
    QLineEdit *address_edit;
    QLabel *start_value;
    QLabel *end_value;
    QLabel *description_label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog);
    void set_comment(bool checked = false);
    void set_data(bool checked = false);
    void retranslateUi(QDialog *Dialog);
    void validate();
};

#endif // DEVICE_H