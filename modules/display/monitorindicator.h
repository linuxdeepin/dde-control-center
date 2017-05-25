#ifndef MONITORFULLWIDGET_H
#define MONITORFULLWIDGET_H

#include <DPlatformWindowHandle>
#include <DWindowManagerHelper>
#include <QFrame>

DWIDGET_USE_NAMESPACE

namespace dcc {
namespace display {

class MonitorIndicator : public QFrame
{
    Q_OBJECT

public:
    explicit MonitorIndicator(QWidget *parent = 0);

protected:
    void resizeEvent(QResizeEvent *e);
};

}
}

#endif // MONITORFULLWIDGET_H
