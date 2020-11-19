#ifndef LABEL_H
#define LABEL_H

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

enum LabelDialogMode
{
    Add,
    Edit
};

struct LabelInfo
{
    QString text;
    LabelType type;
    offs_t start;
    offs_t end;
};

class LabelDialog : public QDialog
{
    Q_OBJECT

public:
    LabelDialog();

    void setLabel(LabelInfo label, LabelDialogMode mode);
    LabelInfo getLabel();

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QRadioButton *comment_radio;
    QRadioButton *data_radio;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLabel *text_label;
    QLabel *start_label;
    QLabel *end_label;
    QLineEdit *text_edit;
    QLineEdit *start_edit;
    QLineEdit *end_edit;
    QLabel *description_label;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *Dialog);
    void set_comment(bool checked = false);
    void set_data(bool checked = false);
    void retranslateUi(QDialog *Dialog);
    void validate();
};

#endif // LABEL_H