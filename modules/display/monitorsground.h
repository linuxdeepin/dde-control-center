#ifndef MONITORSGROUND_H
#define MONITORSGROUND_H

#include "monitor.h"

#include <QFrame>

class DisplayModel;
class MonitorProxyWidget;
class MonitorsGround : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorsGround(QWidget *parent = 0);
    ~MonitorsGround();

    void setDisplayModel(DisplayModel *model);

private slots:
    void monitorMoved(MonitorProxyWidget *pw);
    void adjust(MonitorProxyWidget *pw);

private:
    void ensureWidgetPerfect(MonitorProxyWidget *pw);
    int totalWidth() const;
    int totalHeight() const;
    double screenScale() const;

private:
    int m_viewPortWidth;
    int m_viewPortHeight;
    DisplayModel *m_model;
    QMap<MonitorProxyWidget *, Monitor *> m_monitors;
};

#endif // MONITORSGROUND_H
