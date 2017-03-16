
#include "plugininterface.h"
#include "notifyplugin.h"

NotifyPlugin::NotifyPlugin(QObject *parent)
    : QObject(parent)
{
    m_notifymanager = new NotifyManager;
}

void NotifyPlugin::initialize(FrameProxyInterface *proxy) {
    m_proxyInter    = proxy;
}

QWidget *NotifyPlugin::centralWidget() {
    return m_notifymanager;
}
