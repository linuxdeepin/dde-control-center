#include <QLabel>
#include <QDebug>

#include "network.h"
#include "networkmainwidget.h"
#include "controlcenterproxyinterface.h"

Network::Network()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Network"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_mainWidget = new NetworkMainWidget(this);
}

Network::~Network()
{
    qDebug() << "~Network()";

    m_mainWidget->hide();
    m_mainWidget->setParent(nullptr);
    delete m_mainWidget;
}

QFrame* Network::getContent()
{
    return m_mainWidget;
}

ControlCenterProxyInterface *Network::getInterface() const
{
    return m_controlCenterProxy;
}

void Network::setProxy(ControlCenterProxyInterface *proxy)
{
    ModuleInterface::setProxy(proxy);

    connect(proxy->dccObject(), SIGNAL(visibleChanged(bool)), this, SIGNAL(dccVisibleChanged(bool)));
}
