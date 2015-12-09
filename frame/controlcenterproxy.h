#ifndef CONTROLCENTERPROXY_H
#define CONTROLCENTERPROXY_H

#include "frame.h"

#include <QObject>

class ControlCenterProxy : public QObject
{
    Q_OBJECT

public:
    ControlCenterProxy(Frame *controlCenterPtr);

    static ControlCenterProxy *getInstance();

    inline void hide(bool imme = false) {m_controlCenterPtr->hide(imme);}
    inline void setAutoHide(bool autoHide) {m_controlCenterPtr->setAutoHide(autoHide);}

signals:
    void frameSideChanged(bool inLeft);

private:
    Frame *m_controlCenterPtr = nullptr;
};

#endif // CONTROLCENTERPROXY_H
