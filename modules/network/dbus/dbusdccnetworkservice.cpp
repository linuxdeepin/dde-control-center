#include "dbusdccnetworkservice.h"
#include "networkmainwidget.h"

DBusDCCNetworkService::DBusDCCNetworkService(NetworkMainWidget *parent) :
    QDBusAbstractAdaptor(parent)
{
    m_mainWidget = parent;

    connect(parent->stackWidget(), &DStackWidget::depthChanged, this, &DBusDCCNetworkService::activeChanged);

    /// TODO
    //connect(parent->window(), SIGNAL(visibleChanged()), SLOT(updateActive()));
}

bool DBusDCCNetworkService::active() const
{
    /// TODO
    return m_mainWidget->stackWidget()->depth() == 1 && m_mainWidget->window()->property("visible").toBool();
}
