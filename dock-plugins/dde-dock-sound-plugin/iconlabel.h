#ifndef ICONLABEL_H
#define ICONLABEL_H

#include <QWidget>
#include <QLabel>

class IconLabel : public QLabel
{
    Q_OBJECT
public:
    explicit IconLabel(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent *);
    void mouseReleaseEvent(QMouseEvent *);
    void wheelEvent(QWheelEvent *);

signals:
    void pressed();
    void released();
    void clicked();
};

#endif // ICONLABEL_H
