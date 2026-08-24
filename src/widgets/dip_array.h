#ifndef DIP_ARRAY_H
#define DIP_ARRAY_H

#include <QBrush>
#include <QPen>
#include <QPixmap>
#include <QWidget>
#include <QMouseEvent>
#include <QAction>
#include "../dev/io_dev.h"
#include "../util/settings.h"

class DIPArray : public QWidget
{
    Q_OBJECT

public:
    explicit DIPArray(QWidget *parent = nullptr);
    ~DIPArray();
    io_device *device = nullptr;
    void update_display();
    void set_device(io_device *device);

public slots:
    void redraw();

protected:
    void paintEvent(QPaintEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;

private:
    QPixmap dip[2];
    QAction *action;
    bool running;
    uint8_t byte = 0;
    QFont m_font;
    
};

#endif // DIP_ARRAY_H
