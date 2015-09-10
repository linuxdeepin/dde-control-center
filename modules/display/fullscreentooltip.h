#ifndef FULLSCREENTOOLTIP_H
#define FULLSCREENTOOLTIP_H

#include <QFrame>

class MonitorInterface;
class FullScreenTooltip : public QFrame
{
public:
    explicit FullScreenTooltip(MonitorInterface *dbus);

    void showToCenter();
    void showToTopLeft();

protected:
    void paintEvent(QPaintEvent *e);

private:
    MonitorInterface *m_dbusMonitor;
    bool m_showCenter = true;
    QRect m_monitorRect;
};

#endif // FULLSCREENTOOLTIP_H
