#include "load.h"
#include "../common/util.h"

LoadDialog::LoadDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    setFixedSize(QSize(350, 250));
    setWindowTitle("Load Memory Options");
}

void LoadDialog::setupUi(QDialog *Dialog)
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

    text_label = new QLabel(this);
    text_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    gridLayout->addWidget(text_label, 0, 0, 1, 1);

    text_edit = new QLineEdit(this);
    gridLayout->addWidget(text_edit, 0, 2, 1, 1);

    override_start_check = new QCheckBox(this);
    gridLayout->addWidget(override_start_check, 1, 0, 1, 3);

    start_label = new QLabel(this);
    start_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    gridLayout->addWidget(start_label, 2, 0, 1, 1);

    start_edit = new QLineEdit(this);
    gridLayout->addWidget(start_edit, 2, 2, 1, 1);

    end_label = new QLabel(this);
    end_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);
    gridLayout->addWidget(end_label, 3, 0, 1, 1);

    end_edit = new QLineEdit(this);
    gridLayout->addWidget(end_edit, 3, 2, 1, 1);

    description_label = new QLabel(this);
    description_label->setWordWrap(true);
    gridLayout->addWidget(description_label, 4, 2, 1, 1);

    verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    gridLayout->addItem(verticalSpacer, 5, 2, 1, 1);

    gridLayout->setColumnMinimumWidth(0, 50);
    gridLayout->setRowMinimumHeight(0, 20);
    gridLayout->setRowMinimumHeight(1, 20);
    gridLayout->setRowMinimumHeight(2, 20);
    gridLayout->setRowMinimumHeight(3, 20);
    gridLayout->setRowMinimumHeight(4, 20);
    gridLayout->setRowMinimumHeight(5, 20);

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
    connect(buttonBox, &QDialogButtonBox::accepted, this, &LoadDialog::validate);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &LoadDialog::reject);
    connect(override_start_check, &QCheckBox::stateChanged, this, [this](int state) {
        bool enabled = (state == Qt::Checked);
        start_edit->setEnabled(enabled);
        end_edit->setEnabled(enabled);
    });

    start_edit->setEnabled(false);
    end_edit->setEnabled(false);

    QMetaObject::connectSlotsByName(Dialog);

} // setupUi

void LoadDialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
    start_label->setText(QApplication::translate("Dialog", "Start", nullptr));
    end_label->setText(QApplication::translate("Dialog", "End", nullptr));
    override_start_check->setText(QApplication::translate("Dialog", "Override Start", nullptr));
    text_label->setText(QApplication::translate("Dialog", "Device Name", nullptr));

} // retranslateUi

void LoadDialog::setSettings(LoadSettings settings)
{
    text_edit->setText(settings.device_name);
    start_edit->setText(QString("$%1").arg(settings.start, 4, 16, QChar('0')).toUpper());
    end_edit->setText(QString("$%1").arg(settings.end, 4, 16, QChar('0')).toUpper());
    override_start_check->setChecked(!settings.auto_detect_start);
    start_edit->setEnabled(!settings.auto_detect_start);
    end_edit->setEnabled(!settings.auto_detect_start);
}

void LoadDialog::validate()
{
    bool ok1;
    bool ok2;

    toInt(start_edit, ok1);
    toInt(end_edit, ok2);

    if (ok1)
    {
        accept();
    }
}

LoadSettings LoadDialog::getSettings()
{
    bool ok;
    return LoadSettings{
        text_edit->text(),
        (offs_t)toInt(start_edit, ok),
        (offs_t)toInt(end_edit, ok),
        !override_start_check->isChecked(),
    };
}