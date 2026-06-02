#ifndef DISPLAY_H
#define DISPLAY_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
// #include <QTimer>
#include <QAction>
#include "../dev/display_dev.h"
#include "../util/settings.h"
class Display : public QWidget
{
    Q_OBJECT

public:
    explicit Display(QWidget *parent = nullptr);
    ~Display();
    display_io *device = nullptr;
    void update_display();
    void set_settings(Settings *settings);

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
    uint8_t segment[6];

};

#endif // DISPLAY_H
