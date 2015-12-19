#include "controlcenterproxy.h"
#include "frame.h"

static ControlCenterProxy *m_instance = nullptr;

ControlCenterProxy::ControlCenterProxy(Frame *parent) :
    QObject(parent)
{
    m_instance = this;

    connect(parent, &Frame::hideInLeftChanged, this, &ControlCenterProxy::frameSideChanged);
    connect(parent, &Frame::visibleChanged, this, &ControlCenterProxy::visibleChanged);
}

ControlCenterProxy *ControlCenterProxy::getInstance()
{
    if (!m_instance)
        qWarning() << "dde-control-center instance is null";

    return m_instance;
}

Frame *ControlCenterProxy::parent() const
{
    return qobject_cast<Frame*>(QObject::parent());
}

void ControlCenterProxy::hide(bool imme)
{
    parent()->hide(imme);
}

void ControlCenterProxy::setAutoHide(bool autoHide)
{
    parent()->setAutoHide(autoHide);
}

bool ControlCenterProxy::isVisible() const
{
    return parent()->isVisible();
}

QObject *ControlCenterProxy::dccObject() const
{
    return m_instance;
}
