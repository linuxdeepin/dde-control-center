#ifndef MONITORGROUND_H
#define MONITORGROUND_H

#include <QFrame>

class Monitor;
class DisplayInterface;
class DisplayModeItem;
class FullScreenTooltip;
class MonitorGround : public QFrame
{
    Q_OBJECT

    Q_PROPERTY(bool editable READ editable WRITE setEditable)
    Q_PROPERTY(bool editing READ editing WRITE setEditing NOTIFY editingChanged)

public:
    explicit MonitorGround(DisplayInterface * display, QWidget *parent = 0);

    void addMonitor(Monitor * monitor);
    void removeMonitor(Monitor * monitor);
    void clear();
    bool editable() const;
    bool editing() const;

public slots:
    void setEditable(bool editable);
    void setEditing(bool editing);

signals:
    void editingChanged(bool editing);

private slots:
    void beginEdit();
    void endEdit();
    void relayout();
    void onMonitorMouseMove();
    void onMonitorMouseRelease();
    void updateOpenedCount();

private:
    bool m_editing = false;
    bool m_editable;
    QList<Monitor*> m_monitors;
    DisplayInterface* m_dbusDisplay;
    DisplayModeItem *m_recognize;
    DisplayModeItem *m_edit;
    DisplayModeItem *m_split;
    QList<FullScreenTooltip*> m_tooltipList;
};

#endif // MONITORGROUND_H
