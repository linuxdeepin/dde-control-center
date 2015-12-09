#include "controlcenterproxy.h"

#include <QDebug>

static ControlCenterProxy *m_instance = nullptr;

ControlCenterProxy::ControlCenterProxy(Frame *controlCenterPtr)
    : QObject(controlCenterPtr),
      m_controlCenterPtr(controlCenterPtr)
{
    m_instance = this;

    connect(controlCenterPtr, &Frame::hideInLeftChanged, this, &ControlCenterProxy::frameSideChanged);
}

ControlCenterProxy *ControlCenterProxy::getInstance()
{
    if (!m_instance)
        qWarning() << "dde-control-center instance is null";

    return m_instance;
}

