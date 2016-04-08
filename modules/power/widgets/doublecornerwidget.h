#ifndef DOUBLECORNERWIDGET_H
#define DOUBLECORNERWIDGET_H

#include <QWidget>
#include <QPaintEvent>
#include <QPainter>

#include "constants.h"

class DoubleCornerWidget: public QWidget
{
    Q_OBJECT

public:
    DoubleCornerWidget(QWidget *parent = 0);
    ~DoubleCornerWidget();
    enum Direction {
        Top,
        Bottom,
        All,
        None,
    };
public slots:
    void setDirection(Direction direct);
    void setRadiusMargins(int rad, int margins = 5);
    void setContentLayout(QLayout* l);
protected:
    void paintEvent(QPaintEvent* e);
private:
    Direction m_direct = All;
    int m_margins = 5;
    int m_radius = 4;
};

#endif // DOUBLECORNERWIDGET_H
