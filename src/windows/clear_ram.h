#ifndef CLEAR_RAM_H
#define CLEAR_RAM_H

#include "../common/common_defs.h"
#include "../util/label.h"

#include <QVariant>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

struct ClearRamSettings
{
    offs_t start;
    offs_t end;
    uint8_t value;
};


class ClearRamDialog : public QDialog
{
    Q_OBJECT

public:
    ClearRamDialog();

    void setSettings(ClearRamSettings settings);
    ClearRamSettings getSettings();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLabel *value_label;
    QLabel *start_label;
    QLabel *end_label;
    QLineEdit *value_edit;
    QLineEdit *start_edit;
    QLineEdit *end_edit;
    QLabel *description_label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
    void validate();
};

#endif // CLEAR_RAM_H