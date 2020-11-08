#ifndef DISPLAY_H
#define DISPLAY_H

#include "../util/common.h"
#include "../dev/display_dev.h"
//#include <thread>
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
// #include <QTimer>
#include <QAction>

class Display : public QWidget
{
    Q_OBJECT

public:
    explicit Display(QWidget *parent = nullptr);
    ~Display();
    display_io *device;
    void update_display();

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap hr[2];
    QPixmap vt[2];
    QPixmap dp[2];
    QAction *action;
    bool running;
    //QTimer *m_paintTimer;
};

#endif // DISPLAY_H
