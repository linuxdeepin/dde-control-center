#ifndef MONITORSGROUND_H
#define MONITORSGROUND_H

#include "monitor.h"

#include <QFrame>

namespace dcc {

namespace display {

class DisplayModel;
class MonitorProxyWidget;
class MonitorsGround : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorsGround(QWidget *parent = 0);
    ~MonitorsGround();

    void setDisplayModel(DisplayModel *model);

signals:
    void requestApplySettings(Monitor *mon, const int x, const int y);

private slots:
    void resetMonitorsView();
    void monitorMoved(MonitorProxyWidget *pw);
    void adjust(MonitorProxyWidget *pw);

private:
    void ensureWidgetPerfect(MonitorProxyWidget *pw);
    void reloadViewPortSize();
    void applySettings();
    bool isScreenPerfect() const;
    double screenScale() const;
    const QPoint bestMoveOffset(MonitorProxyWidget *pw0, MonitorProxyWidget *pw1) const;

private:
    int m_viewPortWidth;
    int m_viewPortHeight;
    DisplayModel *m_model;
    QMap<MonitorProxyWidget *, Monitor *> m_monitors;

    QTimer *m_refershTimer;
};

} // namespace display

} // namespace dcc

#endif // MONITORSGROUND_H
