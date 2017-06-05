#ifndef MONITORCONTROLWIDGET_H
#define MONITORCONTROLWIDGET_H

#include "iconbutton.h"
#include <QFrame>
class QPushButton;

namespace dcc {

namespace display {

class Monitor;
class DisplayModel;
class MonitorsGround;
class MonitorControlWidget : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorControlWidget(QWidget *parent = 0);

    void setDisplayModel(DisplayModel *model);
    void setScreensMerged(const bool merged);

signals:
    void requestRecognize() const;
    void requestMerge() const;
    void requestSplit() const;
    void requestSetMonitorPosition(Monitor *mon, const int x, const int y) const;

private:
    MonitorsGround *m_screensGround;
    IconButton *m_recognize;
    IconButton *m_split;
    IconButton *m_join;
};

} // namespace display

} // namespace dcc

#endif // MONITORCONTROLWIDGET_H
