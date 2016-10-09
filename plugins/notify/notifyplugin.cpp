
#include "plugininterface.h"
#include "notifyplugin.h"

NotifyPlugin::NotifyPlugin(QObject *parent)
    : QObject(parent),
      m_notifymanager(nullptr) {

}

void NotifyPlugin::initialize(FrameProxyInterface *proxy) {
    m_proxyInter    = proxy;
    m_notifymanager = new NotifyManager();
    m_scrollarea    = new QScrollArea();
    m_notifymanager->setContentsMargins(0, 0, 0, 0);
    m_notifymanager->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollarea->setWidget(m_notifymanager);
    m_scrollarea->setObjectName("c_scrollarea");
    m_scrollarea->setWidgetResizable(true);
    m_scrollarea->setFocusPolicy(Qt::NoFocus);
    m_scrollarea->setFrameStyle(QFrame::NoFrame);
    m_scrollarea->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_scrollarea->setContentsMargins(0, 0, 0, 0);
    m_scrollarea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollarea->setStyleSheet("background-color: rgba(25, 25, 26, 0.5);");
}

QWidget *NotifyPlugin::centeralWidget() {
    return m_scrollarea;
}
