#include "io_settings.h"
#include "../common/util.h"
#include <QMessageBox>

IOSettingsDialog::IOSettingsDialog(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    setFixedSize(QSize(350, 220));
    setWindowTitle("I/O Settings");
}

void IOSettingsDialog::setupUi(QDialog *Dialog)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    gridLayout = new QGridLayout();
    gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    gridLayout->setContentsMargins(-1, -1, -1, 0);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(verticalSpacer, 3, 1, 1, 1);

    led_label = new QLabel(this);
    led_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(led_label, 0, 0, 1, 1);

    led_edit = new QLineEdit(this);
    led_edit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\$?[0-9A-Fa-f]{0,4}"), led_edit));
    gridLayout->addWidget(led_edit, 0, 1, 1, 1);

    dip_label = new QLabel(this);
    dip_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(dip_label, 1, 0, 1, 1);

    dip_edit = new QLineEdit(this);
    dip_edit->setValidator(new QRegularExpressionValidator(QRegularExpression("\\$?[0-9A-Fa-f]{0,4}"), dip_edit));
    gridLayout->addWidget(dip_edit, 1, 1, 1, 1);

    description_label = new QLabel(this);
    description_label->setWordWrap(true);

    gridLayout->addWidget(description_label, 2, 1, 1, 1);

    gridLayout->setColumnMinimumWidth(0, 80);
    gridLayout->setRowMinimumHeight(0, 20);
    gridLayout->setRowMinimumHeight(1, 20);
    gridLayout->setRowMinimumHeight(2, 20);

    mainLayout->addLayout(gridLayout);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    mainLayout->addWidget(buttonBox);

    mainLayout->setStretch(0, 5);
    mainLayout->setStretch(1, 1);
    mainLayout->setMargin(20);

    setLayout(mainLayout);

    retranslateUi(Dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &IOSettingsDialog::validate);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &IOSettingsDialog::reject);

    QMetaObject::connectSlotsByName(Dialog);

} // setupUi

void IOSettingsDialog::retranslateUi(QDialog *Dialog)
{
    led_label->setText(QApplication::translate("Dialog", "LED Address", nullptr));
    dip_label->setText(QApplication::translate("Dialog", "DIP Address", nullptr));
    description_label->setText(QApplication::translate("Dialog", "Each array occupies a single byte. The LED array is write-only, the DIP array is read-only.", nullptr));

} // retranslateUi

void IOSettingsDialog::setIOSettings(IOSettingsInfo info)
{
    led_edit->setText(toHex(info.led_address));
    dip_edit->setText(toHex(info.dip_address));

    led_edit->setFocus();
    led_edit->setSelection(0, led_edit->text().length());
}

IOSettingsInfo IOSettingsDialog::getIOSettings()
{
    bool ok;

    return IOSettingsInfo({(offs_t)toInt(led_edit, ok),
                           (offs_t)toInt(dip_edit, ok)});
}

void IOSettingsDialog::validate()
{
    bool ok1;
    bool ok2;

    int led_address = toInt(led_edit, ok1);
    int dip_address = toInt(dip_edit, ok2);

    if (!ok1 || !ok2)
    {
        QMessageBox::critical(this, "I/O Settings Error", "Invalid address", QMessageBox::Ok);
        return;
    }

    if (led_address < 0 || led_address > 0xFFFF || dip_address < 0 || dip_address > 0xFFFF)
    {
        QMessageBox::critical(this, "I/O Settings Error", "Address is out of bounds", QMessageBox::Ok);
        return;
    }

    if (led_address == dip_address)
    {
        QMessageBox::critical(this, "I/O Settings Error", "The LED and DIP arrays cannot share the same address.", QMessageBox::Ok);
        return;
    }

    if (address_in_use && address_in_use(led_address))
    {
        QMessageBox::critical(this, "I/O Settings Error", "The LED address overlaps with an existing device.", QMessageBox::Ok);
        return;
    }

    if (address_in_use && address_in_use(dip_address))
    {
        QMessageBox::critical(this, "I/O Settings Error", "The DIP address overlaps with an existing device.", QMessageBox::Ok);
        return;
    }

    accept();
}
