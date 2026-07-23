#include "breakpoint.h"
#include "../common/util.h"
#include <QMessageBox>

BreakpointDialog::BreakpointDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    setFixedSize(QSize(250, 100));
    setWindowTitle("Add Breakpoint");
}

void BreakpointDialog::setupUi(QDialog *Dialog)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // horizontalLayout = new QHBoxLayout();
    // horizontalLayout->setSpacing(6);
    // horizontalLayout->setContentsMargins(-1, -1, -1, 0);

    // horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    // horizontalLayout->addItem(horizontalSpacer);

    // horizontalLayout->setStretch(0, 1);
    // horizontalLayout->setStretch(1, 1);
    // horizontalLayout->setStretch(2, 2);

    // mainLayout->addLayout(horizontalLayout);

    gridLayout = new QGridLayout();
    gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    gridLayout->setContentsMargins(-1, -1, -1, 0);

    // verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    // gridLayout->addItem(verticalSpacer, 4, 2, 1, Qt::AlignLeft);

    address_label = new QLabel(this);
    address_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(address_label, 0, 0, 1, Qt::AlignLeft);

    address_edit = new QLineEdit(this);
    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(address_edit->sizePolicy().hasHeightForWidth());
    address_edit->setSizePolicy(sizePolicy);
    gridLayout->addWidget(address_edit, 0, 2, 1, Qt::AlignLeft);

    gridLayout->setColumnMinimumWidth(0, 50);
    gridLayout->setRowMinimumHeight(0, 20);

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    mainLayout->addWidget(buttonBox);

    mainLayout->setMargin(20);

    setLayout(mainLayout);

    retranslateUi(Dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &BreakpointDialog::validate);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &BreakpointDialog::reject);

    QMetaObject::connectSlotsByName(Dialog);
} // setupUi

void BreakpointDialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));

    address_label->setText(QApplication::translate("Dialog", "Address", nullptr));

} // retranslateUi

void BreakpointDialog::addBreakpoint(offs_t address)
{
    address_edit->setText(toHex(address));

    ref_breakpoint = nullptr;
}

void BreakpointDialog::editBreakpoint(Breakpoint *breakpoint)
{
    setWindowTitle("Edit Breakpoint");

    address_edit->setText(toHex(breakpoint->address));

    ref_breakpoint = breakpoint;
}

void BreakpointDialog::validate()
{
    bool ok1 = true;

    int start = toInt(address_edit, ok1);

    if (!ok1)
    {
        QMessageBox::critical(this, "Breakpoint Error", "Invalid address", QMessageBox::Ok);
        return;
    }

    if (start > 0xFFFF)
    {
        QMessageBox::critical(this, "Breakpoint Error", "Address is out of bounds", QMessageBox::Ok);
        return;
    }

    // if (hasCollision(ref_label, start, end))
    // {
    //     QMessageBox::critical(this, "Breakpoint Error", "The specified address range overlaps with an existing label.", QMessageBox::Ok);
    //     return;
    // }

    if (ok1)
    {
        accept();
    }
}

BreakpointInfo BreakpointDialog::getBreakpoint()
{
    bool ok = true;
    int start = toInt(address_edit, ok);

    return BreakpointInfo{
        (offs_t)start,
    };
}