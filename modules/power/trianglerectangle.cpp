#include <QtGui/QPainter>
#include "trianglerectangle.h"

TriangleRectangle::TriangleRectangle(QWidget *parent)
    : QFrame(parent)
{
    topFromRight = this->width()/8-8;
    halfwidth = 4;
    fillColor = Qt::cyan;
    strokeColor = Qt::red;
}

TriangleRectangle::~TriangleRectangle()
{

}
void TriangleRectangle::paintEvent(QPaintEvent *e) {
    QPainter painter(this);
    QPen pen;
    pen.setColor(strokeColor);

    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(pen);
    painter.setBrush(fillColor);
    QPainterPath trianglePath;
    QPoint triangleTop(rect().x() + rect().width() - topFromRight, rect().y()+2);
    QPoint triangleBottomLeft(rect().x() + rect().width() - topFromRight - halfwidth, rect().y() + rect().height());
    QPoint triangleBottomRight(rect().x() + rect().width() - topFromRight + halfwidth, rect().y() + rect().height());
    trianglePath.moveTo(triangleTop.x(), triangleTop.y());
    trianglePath.lineTo(triangleBottomLeft.x(), triangleBottomLeft.y());
    trianglePath.lineTo(triangleBottomRight.x(), triangleBottomRight.y());
    trianglePath.lineTo(triangleTop.x(), triangleTop.y());
    painter.drawPath(trianglePath);
    Q_UNUSED(e);
}
