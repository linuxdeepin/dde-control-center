#include "dbusdccnetworkservice.h"
#include "networkmainwidget.h"

DBusDCCNetworkService::DBusDCCNetworkService(NetworkMainWidget *parent) :
    QDBusAbstractAdaptor(parent)
{
    m_mainWidget = parent;

    connect(parent->stackWidget(), &DStackWidget::depthChanged, this, &DBusDCCNetworkService::activeChanged);
    connect(parent, &NetworkMainWidget::dccVisibleChanged, this, &DBusDCCNetworkService::activeChanged);
}

bool DBusDCCNetworkService::active() const
{
    return m_mainWidget->stackWidget()->depth() == 1 && m_mainWidget->dccIsVisible();
}
