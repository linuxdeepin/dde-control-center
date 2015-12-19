#ifndef CONTROLCENTERPROXYINTERFACE_H
#define CONTROLCENTERPROXYINTERFACE_H

class QObject;
class ControlCenterProxyInterface
{
public:
    ControlCenterProxyInterface() {}

    virtual void hide(bool imme = false) = 0;
    virtual void setAutoHide(bool autoHide) = 0;
    virtual bool isVisible() const = 0;
    virtual QObject *dccObject() const = 0;
};

#endif // CONTROLCENTERPROXYINTERFACE_H
