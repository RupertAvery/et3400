#ifndef REMOVE_LABEL_H
#define REMOVE_LABEL_H

#include <QVariant>
#include <QApplication>
#include <QDialog>
#include <QDialogButtonBox>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidget>

class RemoveLabelDialog : public QDialog
{
    Q_OBJECT

public:
    RemoveLabelDialog();

    QVBoxLayout *mainLayout;
    QLabel *text_label;
    QDialogButtonBox *buttonBox;

    void setLabel(QString text);

private:
    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
};

#endif // REMOVE_LABEL_H