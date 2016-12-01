#ifndef MONITORPROXYWIDGET_H
#define MONITORPROXYWIDGET_H

#include <QWidget>

class Monitor;
class MonitorProxyWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MonitorProxyWidget(Monitor *mon, QWidget *parent = 0);

    inline int x() const { return m_movedX; }
    inline int y() const { return m_movedY; }
    int w() const;
    int h() const;

protected:
    void paintEvent(QPaintEvent *);

private:
    Monitor *m_monitor;

    int m_movedX;
    int m_movedY;
};

#endif // MONITORPROXYWIDGET_H
