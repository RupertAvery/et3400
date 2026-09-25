#include "device.h"
#include "../common/util.h"
#include <QMessageBox>

DeviceDialog::DeviceDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    setFixedSize(QSize(350, 350));
    setWindowTitle("Add Device");
}

void DeviceDialog::setupUi(QDialog *Dialog)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setContentsMargins(-1, -1, -1, 0);

    horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    horizontalLayout->setStretch(0, 1);
    horizontalLayout->setStretch(1, 1);
    horizontalLayout->setStretch(2, 2);

    mainLayout->addLayout(horizontalLayout);

    gridLayout = new QGridLayout();
    gridLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    gridLayout->setContentsMargins(-1, -1, -1, 0);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(verticalSpacer, 5, 1, 1, 1);

    name_label = new QLabel(this);
    name_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(name_label, 0, 0, 1, 1);

    name_edit = new QLineEdit(this);
    gridLayout->addWidget(name_edit, 0, 1, 1, 1);

    address_label = new QLabel(this);
    address_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(address_label, 1, 0, 1, 1);

    address_edit = new QLineEdit(this);
    address_edit->setValidator(new QRegularExpressionValidator(QRegularExpression("[01Xx ]*"), address_edit));
    gridLayout->addWidget(address_edit, 1, 1, 1, 1);

    connect(address_edit, &QLineEdit::textChanged, this, [this](const QString &text)
            {
                BitPattern bp = parse_pattern(text.toUtf8().constData());
                start_value->setText(toHex(bp.start));
                end_value->setText(toHex(bp.end)); });

    description_label = new QLabel(this);
    description_label->setWordWrap(true);

    gridLayout->addWidget(description_label, 2, 1, 1, 1);

    start_label = new QLabel(this);
    start_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(start_label, 3, 0, 1, 1);

    start_value = new QLabel(this);
    gridLayout->addWidget(start_value, 3, 1, 1, 1);

    end_label = new QLabel(this);
    end_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(end_label, 4, 0, 1, 1);

    end_value = new QLabel(this);
    gridLayout->addWidget(end_value, 4, 1, 1, 1);

    gridLayout->setColumnMinimumWidth(0, 50);
    gridLayout->setRowMinimumHeight(0, 20);
    gridLayout->setRowMinimumHeight(1, 20);
    gridLayout->setRowMinimumHeight(2, 20);
    gridLayout->setRowMinimumHeight(3, 20);
    gridLayout->setRowMinimumHeight(4, 20);

    mainLayout->addLayout(gridLayout);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Ok);

    mainLayout->addWidget(buttonBox);

    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 5);
    mainLayout->setStretch(2, 1);
    mainLayout->setMargin(20);

    setLayout(mainLayout);

    retranslateUi(Dialog);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &DeviceDialog::validate);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &DeviceDialog::reject);

    QMetaObject::connectSlotsByName(Dialog);

} // setupUi

void DeviceDialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));

    name_label->setText(QApplication::translate("Dialog", "Name", nullptr));
    address_label->setText(QApplication::translate("Dialog", "Address Pattern", nullptr));
    start_label->setText(QApplication::translate("Dialog", "Start", nullptr));
    end_label->setText(QApplication::translate("Dialog", "End", nullptr));
    description_label->setText(QApplication::translate("Dialog", "16-bit address bit pattern:\n\n0 - Active Low\n1 - Active High\nX - Don't Care", nullptr));

} // retranslateUi

void DeviceDialog::setDeviceInfo(DeviceInfo info, DeviceDialogMode mode)
{
    if (mode == DeviceDialogMode::EditDevice)
    {
        setWindowTitle("Edit Device");
    }

    name_edit->setText(info.name);

    QString pattern = QString::fromLatin1(info.bit_pattern.pattern);

    for (int i = pattern.length() - 4; i > 0; i -= 4)
        pattern.insert(i, ' ');

    address_edit->setText(pattern);
    if (mode == DeviceDialogMode::AddDevice)
    {
        BitPattern bp = parse_pattern(address_edit->text().toUtf8().constData());
        info.bit_pattern.start = bp.start;
        info.bit_pattern.end = bp.end;
    }

    start_value->setText(toHex(info.bit_pattern.start));
    end_value->setText(toHex(info.bit_pattern.end));

    name_edit->setFocus();
    name_edit->setSelection(0, info.name.length());
}

void DeviceDialog::validate()
{
    bool ok1;
    bool ok2;

    ok1 = is_pattern_valid(address_edit->text().toUtf8().constData());

    if (ok1)
    {
        BitPattern bp = parse_pattern(address_edit->text().toUtf8().constData());
        ok2 = !has_collision(bp.start, bp.end);

        if (!ok2)
        {
            auto result = QMessageBox::critical(this, "Address Mapping Error", "The start and end addresses overlap with an existing device.", QMessageBox::StandardButton::Ok);
        }
    }
    else
    {
        auto result = QMessageBox::critical(this, "Address Mapping Error", "The address pattern is invalid or incomplete.", QMessageBox::StandardButton::Ok);
    }

    if (ok1 && ok2)
    {
        accept();
    }
}

DeviceInfo DeviceDialog::getDeviceInfo()
{
    BitPattern bp = parse_pattern(address_edit->text().toUtf8().constData());

    return DeviceInfo({name_edit->text(),
                       bp});
}