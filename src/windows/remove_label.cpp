#include "remove_label.h"

RemoveLabelDialog::RemoveLabelDialog() : QDialog(0, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);

    this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

    setFixedSize(QSize(350, 150));
    setWindowTitle("Renove Label");
}

void RemoveLabelDialog::setupUi(QDialog *Dialog)
{
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(10);
    mainLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    text_label = new QLabel;
    text_label->setWordWrap(true);

    buttonBox = new QDialogButtonBox(this);
    buttonBox->setOrientation(Qt::Horizontal);
    buttonBox->setStandardButtons(QDialogButtonBox::Yes | QDialogButtonBox::No);

    mainLayout->addWidget(text_label);
    mainLayout->addWidget(buttonBox);

    mainLayout->setStretch(0, 1);
    mainLayout->setStretch(1, 5);
    mainLayout->setStretch(2, 1);
    mainLayout->setMargin(20);

    setLayout(mainLayout);

    retranslateUi(Dialog);
    QObject::connect(buttonBox, SIGNAL(accepted()), Dialog, SLOT(accept()));
    QObject::connect(buttonBox, SIGNAL(rejected()), Dialog, SLOT(reject()));

    QMetaObject::connectSlotsByName(Dialog);

} // setupUi

void RemoveLabelDialog::retranslateUi(QDialog *Dialog)
{

} // retranslateUi

void RemoveLabelDialog::setLabel(QString text)
{
    text_label->setText(QString("Are you sure you want to remove the label \"%1\"?").arg(text));
}
