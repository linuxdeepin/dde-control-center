#ifndef MONITORGROUND_H
#define MONITORGROUND_H

#include <QFrame>

class Monitor;
class DisplayInterface;
class DisplayModeItem;
class MonitorGround : public QFrame
{
    Q_OBJECT
public:
    explicit MonitorGround(DisplayInterface * display, QWidget *parent = 0);

    void addMonitor(Monitor * monitor);
    void removeMonitor(Monitor * monitor);
    void clear();
    void beginEdit();
    void endEdit();

private slots:
    void relayout();
    void onMonitorMouseMove();
    void onMonitorMouseRelease();
    void updateOpenedCount();

private:
    bool m_editing = false;
    QList<Monitor*> m_monitors;
    DisplayInterface* m_dbusDisplay;
    DisplayModeItem *m_recognize;
    DisplayModeItem *m_edit;
    DisplayModeItem *m_split;
};

#endif // MONITORGROUND_H
