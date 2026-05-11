#ifndef LOAD_H
#define LOAD_H

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
#include <QCheckBox>
#include <QSpacerItem>
#include <QVBoxLayout>
#include <QWidget>

struct LoadSettings
{
    QString device_name;
    offs_t start;
    offs_t end;
    bool auto_detect_start;
};

class LoadDialog : public QDialog
{
    Q_OBJECT

public:
    LoadDialog();

    void setSettings(LoadSettings settings);
    LoadSettings getSettings();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLabel *text_label;
    QLabel *start_label;
    QLabel *end_label;
    QLineEdit *text_edit;
    QCheckBox *override_start_check;
    QLineEdit *start_edit;
    QLineEdit *end_edit;
    QLabel *description_label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
    void validate();
};

#endif // LOAD_H