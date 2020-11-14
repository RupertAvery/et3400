#include <QDebug>
#include "status_view.h"
#include "../emu/et3400.h"

StatusView::StatusView(QWidget *parent)
    : QFrame(parent)
{
    setFrameStyle(QFrame::StyledPanel | QFrame::Sunken);
    setLineWidth(3);

    is_emulator_set = false;

    m_paintTimer = new QTimer(this);
    m_paintTimer->start(36); // 38ms, or every 1/30th of a second
    connect(this->m_paintTimer, &QTimer::timeout, this, &StatusView::update);

    QString style = "border: 1px solid black; font-size: 12pt; font-family: Courier";
    QString bits_style = "padding-right: 10px; font-size: 12pt; font-family: Courier";

    pc_label = new QLabel;
    sp_label = new QLabel;
    ix_label = new QLabel;
    acca_label = new QLabel;
    accb_label = new QLabel;
    cc_label = new QLabel;
    QLabel *bits_label = new QLabel("--HINZVC");
    pc_label->setStyleSheet(style);
    sp_label->setStyleSheet(style);
    ix_label->setStyleSheet(style);
    acca_label->setStyleSheet(style);
    accb_label->setStyleSheet(style);
    cc_label->setStyleSheet(style);
    bits_label->setStyleSheet(bits_style);

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->setColumnStretch(0, 1);
    mainLayout->setColumnStretch(1, 1);
    mainLayout->setRowStretch(0, 1);
    mainLayout->setRowStretch(1, 1);
    mainLayout->setRowStretch(2, 1);
    mainLayout->setRowStretch(3, 1);
    mainLayout->setRowStretch(4, 1);
    mainLayout->setRowStretch(5, 1);
    mainLayout->setRowStretch(6, 1);
    mainLayout->setRowStretch(7, 80);
    mainLayout->addWidget(new QLabel("PC"), 0, 0, 1, 1);
    mainLayout->addWidget(new QLabel("SP"), 1, 0, 1, 1);
    mainLayout->addWidget(new QLabel("IX"), 2, 0, 1, 1);
    mainLayout->addWidget(new QLabel("ACCA"), 3, 0, 1, 1);
    mainLayout->addWidget(new QLabel("ACCB"), 4, 0, 1, 1);
    mainLayout->addWidget(new QLabel("CC"), 6, 0, 1, 1);
    mainLayout->addWidget(pc_label, 0, 1, 1, 1);
    mainLayout->addWidget(sp_label, 1, 1, 1, 1);
    mainLayout->addWidget(ix_label, 2, 1, 1, 1);
    mainLayout->addWidget(acca_label, 3, 1, 1, 1);
    mainLayout->addWidget(accb_label, 4, 1, 1, 1);
    mainLayout->addWidget(bits_label, 5, 1, 1, 1, Qt::AlignRight);
    mainLayout->addWidget(cc_label, 6, 1, 1, 1);

    // action = new QAction;
    // connect(action, &QAction::triggered, this, &Display::redraw);

    // this->setFixedSize(QSize(320, 85));
    setLayout(mainLayout);
    setLineWidth(3);
}

StatusView::~StatusView()
{
    m_paintTimer->stop();
    delete m_paintTimer;
}

void StatusView::update()
{
    if (is_emulator_set)
    {
        QChar filler = QLatin1Char('0');
        CpuStatus status = emu_ptr->get_status();
        pc_label->setText(QString("%1").arg(status.pc, 4, 16, filler).toUpper());
        sp_label->setText(QString("%1").arg(status.sp, 4, 16, filler).toUpper());
        ix_label->setText(QString("%1").arg(status.ix, 4, 16, filler).toUpper());
        acca_label->setText(QString("%1").arg(status.acca, 2, 16, filler).toUpper());
        accb_label->setText(QString("%1").arg(status.accb, 2, 16, filler).toUpper());
        cc_label->setText(QString("11%1%2%3%4%5%6")
                              .arg(status.cc >> 5 & 1)
                              .arg(status.cc >> 4 & 1)
                              .arg(status.cc >> 3 & 1)
                              .arg(status.cc >> 2 & 1)
                              .arg(status.cc >> 1 & 1)
                              .arg(status.cc >> 0 & 1))

            ;
    }
}

void StatusView::set_emulator(et3400emu *emu)
{
    emu_ptr = emu;
    is_emulator_set = true;
}