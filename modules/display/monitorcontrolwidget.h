#ifndef MONITORCONTROLWIDGET_H
#define MONITORCONTROLWIDGET_H

#include <QWidget>
#include <QPushButton>

class Monitor;
class DisplayModel;
class MonitorsGround;
class MonitorControlWidget : public QWidget
{
    Q_OBJECT

public:
    explicit MonitorControlWidget(QWidget *parent = 0);

    void setDisplayModel(DisplayModel *model);
    void setScreensMerged(const bool merged);

signals:
    void requestRecognize() const;
    void requestMerge() const;
    void requestSetMonitorPosition(Monitor *mon, const int x, const int y) const;

private:
    MonitorsGround *m_screensGround;
    QPushButton *m_recognize;
    QPushButton *m_split;
    QPushButton *m_join;
};

#endif // MONITORCONTROLWIDGET_H
