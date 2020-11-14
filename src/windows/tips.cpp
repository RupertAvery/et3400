#include "QDebug"
#include "tips.h"

Tips::Tips(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QLabel *didYouKnow = new QLabel("Did you know?");
    didYouKnow->setStyleSheet("font-size: 20px; font-weight: 500;");

    QLabel *label = new QLabel;
    label->setWordWrap(true);

    QPushButton *previous_button = new QPushButton("Previous");
    previous_button->setFixedWidth(100);

    QPushButton *next_button = new QPushButton("Next");
    next_button->setFixedWidth(100);

    show_tip_checkbox = new QCheckBox("Show &tips on startup");


    connect(show_tip_checkbox, &QCheckBox::clicked, this, [this, label](bool checked) {
        qDebug() << "show_tip_checkbox";
        settings->showTips = checked;
        save_settings(settings);
    });


    connect(previous_button, &QPushButton::clicked, this, [this, label](bool checked) {
        tip--;
        if (tip < 0)
            tip = 14;
        label->setText(tips[tip]);
    });

    connect(
        next_button, &QPushButton::clicked, this, [this, label](bool checked) {
            tip++;
            if (tip > 14)
                tip = 0;
            label->setText(tips[tip]);
        });

    QWidget *buttons = new QWidget;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addWidget(previous_button);
    buttonLayout->addWidget(next_button);
    buttons->setLayout(buttonLayout);

    QSpacerItem *verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    mainLayout->addWidget(didYouKnow);
    mainLayout->addWidget(label);
    mainLayout->addItem(verticalSpacer);
    mainLayout->addWidget(buttons);
    mainLayout->addWidget(show_tip_checkbox);
    mainLayout->setMargin(20);
    setLayout(mainLayout);

    tip = rand() % 15;

    label->setText(tips[tip]);

    setFixedWidth(400);

    setWindowTitle("Tips");
}

void Tips::set_settings(Settings *settings)
{
    this->settings = settings;
    show_tip_checkbox->setChecked(settings->showTips);
}
