#ifndef REGISTER_H
#define REGISTER_H

#include <QWidget>
#include <QFrame>
#include <QFont>
#include <QFontMetrics>
#include <QGridLayout>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QTimer>
#include <QFrame>
#include "../emu/et3400.h"

class RegisterView : public QFrame
{
    Q_OBJECT

public:
    enum RegisterType
    {
        BYTE,
        WORD,
        FLAGS,
    };

    RegisterView(RegisterType type, QWidget *parent = nullptr);
    ~RegisterView();

    void set_value(uint16_t value);
    void start_editing();
    void stop_editing();
    void set_enabled(bool enabled) { 
        this->enabled = enabled;
        if(!enabled)
        {
            stop_editing();
        }
        update(); 
    }

signals:
    void on_value_changed(uint16_t new_value);
    void on_edit_abort(QString message);

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

private:
    RegisterType m_type;
    QFont m_font;
    QFontMetrics *m_fm = nullptr;

    uint16_t value;

    int item_height;
    void bufferDraw();




    QPixmap *buffer;

    bool enabled = false;
    bool is_selected = false;
    bool is_editing = false;
    int editing_nibble = 0;
    uint16_t editing_value = 0;
    int blink_state = 0;
    QTimer *blink_timer;    
};

#endif // REGISTER_H