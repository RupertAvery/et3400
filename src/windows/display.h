#ifndef DISPLAY_H
#define DISPLAY_H

#include "../util/common.h"
#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <thread>
#include <QTimer>

class Display : public QWidget
{
    Q_OBJECT

public:
    explicit Display(QWidget *parent = nullptr);
    void set_memory(uint8_t *ptr);

public slots:
    void redraw();

protected:
    ~Display();
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap segment_a;
    QPixmap segment_b;
    QPixmap segment_c;
    QPixmap segment_d;
    QPixmap segment_e;
    QPixmap segment_f;
    QPixmap segment_g;
    QPixmap segment_dp;
    QPixmap hr[2];
    QPixmap vt[2];
    QPixmap dp[2];
    bool running;
    std::thread thread;
    uint8_t *memptr;
    QTimer *m_paintTimer;
};

#endif // DISPLAY_H
