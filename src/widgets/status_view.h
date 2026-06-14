#ifndef STATUS_VIEW_H
#define STATUS_VIEW_H

#include "../common/common_defs.h"
// #include <thread>
#include <QTimer>
#include <QWidget>
#include <QFrame>
#include <QGridLayout>
#include <QAction>
#include <QLabel>
#include "register.h"

class StatusView : public QFrame
{
    Q_OBJECT

public:
    StatusView();
    StatusView(QWidget *parent);
    ~StatusView();
    void set_emulator(et3400emu *emu);
    void set_enabled(bool enabled);

    signals:
    void on_edit_abort(QString message);

private:
    QAction *action;
    QTimer *m_paintTimer;
    RegisterView *pc_label;
    RegisterView *sp_label;
    RegisterView *ix_label;
    RegisterView *acca_label;
    RegisterView *accb_label;
    RegisterView *cc_label;

    et3400emu *emu_ptr;
    bool is_emulator_set;

    void update();
};

#endif // STATUS_VIEW_H
