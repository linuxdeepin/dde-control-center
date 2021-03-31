#include "customwidget.h"
#include "QPainter"

CustomWidget::CustomWidget(QWidget *parent) : QWidget(parent)
{

}

void CustomWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    QColor color(255,255,255);
    color.setAlphaF(0.9);
    painter.fillRect(rect(),color);
    return QWidget::paintEvent(event);
}
