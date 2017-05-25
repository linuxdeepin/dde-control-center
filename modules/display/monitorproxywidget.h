#ifndef MONITORPROXYWIDGET_H
#define MONITORPROXYWIDGET_H

#include "monitorindicator.h"
#include <QWidget>
#include <memory>

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

public slots:
    void setIndicatorGeometry();

signals:
    void requestApplyMove(MonitorProxyWidget *self) const;

protected:
    void paintEvent(QPaintEvent *) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *e) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *) Q_DECL_OVERRIDE;

private:
    Monitor *m_monitor;

    int m_movedX;
    int m_movedY;

    QPoint m_lastPos;

    bool m_mouseState;
    std::unique_ptr<MonitorIndicator> m_fullIndication;
};

} // namespace display

} // namespace dcc

#endif // MONITORPROXYWIDGET_H
