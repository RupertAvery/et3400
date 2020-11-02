#include "about.h"

AboutDialog::AboutDialog()
{
    QWidget *mainwidget = new QWidget;
    QVBoxLayout *textLayout = new QVBoxLayout(mainwidget);
    QLabel *link = new QLabel;

    link->setText("<a href=\"https://github.com/RupertAvery/et3400/\">https://github.com/RupertAvery/et3400!</a>");
    link->setTextFormat(Qt::RichText);
    link->setTextInteractionFlags(Qt::TextBrowserInteraction);
    link->setOpenExternalLinks(true);
    QLabel *title = new QLabel("ET-3400 Trainer Emulator");
    title->setStyleSheet("QLabel { font-size:20px; font-weight: bold }");
    textLayout->addWidget(title);
    textLayout->addWidget(new QLabel("©2020 David Khristepher Santos"));
    textLayout->addWidget(new QLabel("rupert.avery@gmail.com"));
    textLayout->addWidget(link);
    mainwidget->setFixedHeight(150);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addStretch(1);
    mainLayout->addWidget(mainwidget);
    mainLayout->addStretch(1);
    mainLayout->setMargin(10);

    setLayout(mainLayout);

    this->setStyleSheet("QLabel { font-size:12px; height: 20px }");

    setFixedSize(QSize(350, 250));
    setWindowTitle("About ET-3400");
}
