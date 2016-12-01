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
    void adjust(MonitorProxyWidget *pw);

private:
    DisplayModel *m_model;
    QMap<Monitor *, MonitorProxyWidget *> m_monitors;
};

#endif // MONITORSGROUND_H
