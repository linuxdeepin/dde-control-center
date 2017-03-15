
#include "plugininterface.h"
#include "notifyplugin.h"

NotifyPlugin::NotifyPlugin(QObject *parent)
    : QObject(parent)
{
    m_notifymanager = new NotifyManager;
}

void NotifyPlugin::initialize(FrameProxyInterface *proxy) {
    m_proxyInter    = proxy;
    m_notifymanager->setContentsMargins(0, 0, 0, 0);
    m_notifymanager->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

QWidget *NotifyPlugin::centralWidget() {
    return m_notifymanager;
}
