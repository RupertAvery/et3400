#include "QDebug"
#include <QScrollArea>
#include "tips.h"

Tips::Tips(QWidget *parent) : QDialog(parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint | Qt::WindowCloseButtonHint)
{
    setupUi(this);
}

void Tips::setupUi(QDialog *Dialog)
{
    QVBoxLayout *mainLayout = new QVBoxLayout;

    QWidget *container = new QWidget;
    QVBoxLayout *containerLayout = new QVBoxLayout;
    label = new QLabel;
    label->setTextFormat(Qt::RichText);
    label->setWordWrap(true);
    label->setStyleSheet("font-size: 14px;");
    label->setMinimumWidth(340);
    label->setTextInteractionFlags(Qt::TextBrowserInteraction);
    label->setOpenExternalLinks(true);

    // TODO: ScrollAreaa does not show scrollbar when label is too large

    containerLayout->addWidget(label);
    containerLayout->addStretch();
    containerLayout->setAlignment(Qt::AlignTop);

    container->setLayout(containerLayout);
    container->setMinimumHeight(150);

    QScrollArea *scrollArea = new QScrollArea;
    scrollArea->setBackgroundRole(QPalette::Light);
    scrollArea->setWidgetResizable(true);
    scrollArea->setWidget(container);

    QPushButton *previous_button = new QPushButton("Previous");
    previous_button->setFixedWidth(80);

    QPushButton *next_button = new QPushButton("Next");
    next_button->setFixedWidth(80);

    show_tip_checkbox = new QCheckBox("Show &tips on startup");

    connect(show_tip_checkbox, &QCheckBox::clicked, this, [this](bool checked)
            {
        settings->showTips = checked;
        save_settings(settings); });

    connect(previous_button, &QPushButton::clicked, this, [this](bool checked)
            {
        tip--;
        if (tip < 0)
            tip = TIPCOUNT - 1;
        label->setText(tips[tip]); });

    connect(
        next_button, &QPushButton::clicked, this, [this](bool checked)
        {
            tip++;
            if (tip > TIPCOUNT - 1)
                tip = 0;
            label->setText(tips[tip]); });

    QWidget *buttons = new QWidget;
    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->addStretch(1);
    buttonLayout->addWidget(previous_button);
    buttonLayout->addWidget(next_button);
    buttonLayout->addStretch(1);
    buttons->setLayout(buttonLayout);

    mainLayout->addWidget(scrollArea);
    // mainLayout->addStretch(1);
    mainLayout->addWidget(buttons);
    mainLayout->addWidget(show_tip_checkbox);
    mainLayout->setMargin(20);
    setLayout(mainLayout);

    setFixedWidth(500);
    setMinimumHeight(200);

    setWindowTitle("Tips");
}

void Tips::random_tip()
{
    tip = rand() % TIPCOUNT;

    label->setText(tips[tip]);
}

void Tips::set_tip(int index)
{
    tip = index;
    label->setText(tips[tip]);
}

void Tips::set_settings(Settings *settings)
{
    this->settings = settings;
    show_tip_checkbox->setChecked(settings->showTips);
}
