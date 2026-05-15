#include "save.h"
#include "../common/util.h"

SaveDialog::SaveDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    setFixedSize(QSize(350, 250));
    setWindowTitle("Save RAM Options");
}

void SaveDialog::setupUi(QDialog *Dialog)
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
    gridLayout->addItem(verticalSpacer, 4, 2, 1, 1);

    start_label = new QLabel(this);
    start_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(start_label, 1, 0, 1, 1);

    // text_label = new QLabel(this);
    // text_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    // gridLayout->addWidget(text_label, 0, 0, 1, 1);

    // text_edit = new QLineEdit(this);
    // gridLayout->addWidget(text_edit, 0, 2, 1, 1);

    start_edit = new QLineEdit(this);

    QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    sizePolicy.setHorizontalStretch(0);
    sizePolicy.setVerticalStretch(0);
    sizePolicy.setHeightForWidth(start_edit->sizePolicy().hasHeightForWidth());
    start_edit->setSizePolicy(sizePolicy);

    gridLayout->addWidget(start_edit, 1, 2, 1, 1);

    end_label = new QLabel(this);
    end_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(end_label, 2, 0, 1, 1);

    end_edit = new QLineEdit(this);
    gridLayout->addWidget(end_edit, 2, 2, 1, 1);

    description_label = new QLabel(this);
    description_label->setWordWrap(true);

    gridLayout->addWidget(description_label, 0, 2, 1, 1);

    gridLayout->setColumnMinimumWidth(0, 50);
    gridLayout->setRowMinimumHeight(0, 20);
    gridLayout->setRowMinimumHeight(1, 20);
    gridLayout->setRowMinimumHeight(2, 20);
    gridLayout->setRowMinimumHeight(3, 20);

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
    connect(buttonBox, &QDialogButtonBox::accepted, this, &SaveDialog::validate);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &SaveDialog::reject);

    QMetaObject::connectSlotsByName(Dialog);

} // setupUi

void SaveDialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));
    description_label->setText(QApplication::translate("Dialog", "Select the range of RAM to save", nullptr));
    start_label->setText(QApplication::translate("Dialog", "Start", nullptr));
    end_label->setText(QApplication::translate("Dialog", "End", nullptr));
    //text_label->setText(QApplication::translate("Dialog", "Header", nullptr));

} // retranslateUi

void SaveDialog::setSettings(SaveSettings settings)
{
    //text_edit->setText(settings.header);
    start_edit->setText(QString("$%1").arg(settings.start, 4, 16, QChar('0')).toUpper());
    end_edit->setText(QString("$%1").arg(settings.end, 4, 16, QChar('0')).toUpper());
}

void SaveDialog::validate()
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

SaveSettings SaveDialog::getSettings()
{
    bool ok;
    return SaveSettings{
        "", //text_edit->text(),
        (offs_t)toInt(start_edit, ok),
        (offs_t)toInt(end_edit, ok),
    };
}