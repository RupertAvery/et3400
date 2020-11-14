#include "add_label.h"

AddLabelDialog::AddLabelDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

    setFixedSize(QSize(350, 150));
    setWindowTitle("Add Label");
}

void AddLabelDialog::setupUi(QDialog *Dialog)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setContentsMargins(-1, -1, -1, 0);

    comment_radio = new QRadioButton(this);

    horizontalLayout->addWidget(comment_radio);

    data_radio = new QRadioButton(this);

    horizontalLayout->addWidget(data_radio);

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

    text_label = new QLabel(this);
    text_label->setAlignment(Qt::AlignRight | Qt::AlignTrailing | Qt::AlignVCenter);

    gridLayout->addWidget(text_label, 0, 0, 1, 1);

    text_edit = new QLineEdit(this);
    gridLayout->addWidget(text_edit, 0, 2, 1, 1);

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

    gridLayout->addWidget(description_label, 3, 2, 1, 1);

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

    QObject::connect(comment_radio, &QRadioButton::clicked, this, &AddLabelDialog::set_comment);
    QObject::connect(data_radio, &QRadioButton::clicked, this, &AddLabelDialog::set_data);

    retranslateUi(Dialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(Dialog);

    comment_radio->setChecked(true);
    set_comment();
} // setupUi

void AddLabelDialog::set_comment(bool checked)
{
    start_label->setText(QApplication::translate("Dialog", "Address", nullptr));
    end_label->setText(QApplication::translate("Dialog", "End", nullptr));
    end_label->setVisible(false);
    end_edit->setVisible(false);
    description_label->setText("Add a comment at the specified address. This does not affect disassembly.");
}

void AddLabelDialog::set_data(bool checked)
{
    start_label->setText(QApplication::translate("Dialog", "Start", nullptr));
    end_label->setVisible(true);
    end_edit->setVisible(true);
    description_label->setText("Mark the range as data, preventing the disassembler from processing it as code");
}

void AddLabelDialog::retranslateUi(QDialog *Dialog)
{
    Dialog->setWindowTitle(QApplication::translate("Dialog", "Dialog", nullptr));

    comment_radio->setText(QApplication::translate("Dialog", "Comment", nullptr));
    data_radio->setText(QApplication::translate("Dialog", "Data", nullptr));

    start_label->setText(QApplication::translate("Dialog", "Start", nullptr));
    end_label->setText(QApplication::translate("Dialog", "End", nullptr));
    text_label->setText(QApplication::translate("Dialog", "Text", nullptr));

} // retranslateUi

void AddLabelDialog::setLabel(LabelInfo label)
{
    text_edit->setText(label.text);
    start_edit->setText(QString("$%1").arg(label.start, 4, 16, QChar('0')).toUpper());
    end_edit->setText(QString("$%1").arg(label.end, 4, 16, QChar('0')).toUpper());
}

LabelInfo AddLabelDialog::getLabel()
{
    bool ok;
    return LabelInfo{
        text_edit->text(),
        data_radio->isChecked() ? LabelType::DATA : LabelType::COMMENT,
        (offs_t)start_edit->text().replace("$", "0x").toInt(&ok, 0),
        (offs_t)end_edit->text().replace("$", "0x").toInt(&ok, 0),
    };
}