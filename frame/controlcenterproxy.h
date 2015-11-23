#ifndef CONTROLCENTERPROXY_H
#define CONTROLCENTERPROXY_H

#include "frame.h"

class ControlCenterProxy
{
public:
    ControlCenterProxy(Frame *controlCenterPtr);

    inline void hide(bool imme = false) {m_controlCenterPtr->hide(imme);}
    inline void setAutoHide(bool autoHide) {m_controlCenterPtr->setAutoHide(autoHide);}

private:
    Frame *m_controlCenterPtr = nullptr;
};

#endif // CONTROLCENTERPROXY_H
