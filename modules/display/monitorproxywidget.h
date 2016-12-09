#ifndef MONITORPROXYWIDGET_H
#define MONITORPROXYWIDGET_H

#include <QWidget>

namespace dcc {

namespace display {

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

    inline void setMovedX(const int x) { m_movedX = x; }
    inline void setMovedY(const int y) { m_movedY = y; }

    const QString name() const;

signals:
    void requestApplyMove(MonitorProxyWidget *self) const;

protected:
    void paintEvent(QPaintEvent *);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mouseReleaseEvent(QMouseEvent *);

private:
    Monitor *m_monitor;

    int m_movedX;
    int m_movedY;

    QPoint m_lastPos;
};

} // namespace display

} // namespace dcc

#endif // MONITORPROXYWIDGET_H
