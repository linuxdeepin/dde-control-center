#ifndef CONTROLCENTERPROXY_H
#define CONTROLCENTERPROXY_H

#include <QObject>

#include "controlcenterproxyinterface.h"

class Frame;
class ControlCenterProxy : public QObject, public ControlCenterProxyInterface
{
    Q_OBJECT

public:
    explicit ControlCenterProxy(Frame *parent);

    static ControlCenterProxy *getInstance();

    Frame* parent() const;

    void hide(bool imme = false)  Q_DECL_OVERRIDE;
    void setAutoHide(bool autoHide) Q_DECL_OVERRIDE;

    bool isVisible() const Q_DECL_OVERRIDE;

    QObject *dccObject() const Q_DECL_OVERRIDE;

signals:
    void frameSideChanged(bool inLeft);
    void visibleChanged(bool visible);
};

#endif // CONTROLCENTERPROXY_H
