#ifndef BREAKPOINT_H
#define BREAKPOINT_H

#include "../common/common_defs.h"
#include "../util/breakpoint.h"

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

struct BreakpointInfo
{
    offs_t address;
};

class BreakpointDialog : public QDialog
{
    Q_OBJECT

public:
    BreakpointDialog();

    void addBreakpoint(offs_t address);
    void editBreakpoint(Breakpoint *breakpoint);

    BreakpointInfo getBreakpoint();
    // std::function<bool(Breakpoint *label, offs_t start)> hasCollision;

private:
    QVBoxLayout *mainLayout;
    QHBoxLayout *horizontalLayout;
    QSpacerItem *horizontalSpacer;
    QGridLayout *gridLayout;
    QSpacerItem *verticalSpacer;
    QLabel *address_label;
    QLineEdit *address_edit;
    QDialogButtonBox *buttonBox;

    Breakpoint *ref_breakpoint;

    void setupUi(QDialog *Dialog);
    void retranslateUi(QDialog *Dialog);
    void validate();
};

#endif // BREAKPOINT_H