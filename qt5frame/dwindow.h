#ifndef DWINDOW_H
#define DWINDOW_H

#include <QQuickItem>
#include <QQuickWindow>

class DWindow: public QQuickWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(DWindow)

public:
    DWindow(QQuickWindow *parent = 0);
    ~DWindow();
};

class DOverrideWindow: public QQuickWindow
{
    Q_OBJECT
    Q_DISABLE_COPY(DOverrideWindow)

public:
    DOverrideWindow(QQuickWindow *parent = 0);
    ~DOverrideWindow();
protected:
    void mousePressEvent(QMouseEvent *ev);
signals:
    void mousePressed(QPointF point);
};


#endif // DWINDOW_H
