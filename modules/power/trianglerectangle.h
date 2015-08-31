#ifndef WIDGET_H
#define WIDGET_H

#include <QtWidgets/QFrame>
#include <QtGui/QColor>

class TriangleRectangle : public QFrame
{
    Q_OBJECT

public:
    TriangleRectangle(QWidget *parent = 0);
    ~TriangleRectangle();
    void paintEvent(QPaintEvent* e);
    QColor fillColor;
    QColor strokeColor;
    int topFromRight;
    int halfwidth;
};

#endif // WIDGET_H
