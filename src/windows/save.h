#ifndef SAVE_H
#define SAVE_H

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

struct SaveSettings
{
    QString header;
    offs_t start;
    offs_t end;
};

class SaveDialog : public QDialog
{
    Q_OBJECT

public:
    SaveDialog();

    void setSettings(SaveSettings settings);
    SaveSettings getSettings();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    //QLabel *text_label;
    QLabel *start_label;
    QLabel *end_label;
    //QLineEdit *text_edit;
    QLineEdit *start_edit;
    QLineEdit *end_edit;
    QLabel *description_label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
    void validate();
};

#endif // SAVE_H