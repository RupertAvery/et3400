#ifndef STATUS_VIEW_H
#define STATUS_VIEW_H

#include "../common/common_defs.h"
//#include <thread>
#include <QTimer>
#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QAction>
#include <QLabel>

class StatusView : public QFrame
{
    Q_OBJECT

public:
    StatusView();
    StatusView(QWidget *parent);
    ~StatusView();
    void set_emulator(et3400emu *emu);

private:
    QAction *action;
    QTimer *m_paintTimer;
    QLabel *pc_label;
    QLabel *sp_label;
    QLabel *ix_label;
    QLabel *acca_label;
    QLabel *accb_label;
    QLabel *cc_label;

    et3400emu *emu_ptr;
    bool is_emulator_set;

    void update();
};

#endif // STATUS_VIEW_H
