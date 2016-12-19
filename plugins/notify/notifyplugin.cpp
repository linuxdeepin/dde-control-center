
#include "plugininterface.h"
#include "notifyplugin.h"

NotifyPlugin::NotifyPlugin(QObject *parent)
    : QObject(parent)
{
    m_scrollarea    = new QScrollArea;
    m_notifymanager = new NotifyManager;
}

void NotifyPlugin::initialize(FrameProxyInterface *proxy) {
    m_proxyInter    = proxy;
    m_notifymanager->setContentsMargins(0, 0, 0, 0);
    m_notifymanager->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollarea->setWidget(m_notifymanager);
    m_scrollarea->setObjectName("scrollarea");
    m_scrollarea->setWidgetResizable(true);
    m_scrollarea->setFocusPolicy(Qt::NoFocus);
    m_scrollarea->setFrameStyle(QFrame::NoFrame);
    m_scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollarea->setContentsMargins(0, 0, 0, 0);
    m_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setStyleSheet("background-color:transparent;");
}

QWidget *NotifyPlugin::centeralWidget() {
    return m_scrollarea;
}
