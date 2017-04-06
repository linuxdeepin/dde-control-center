#include "navgationplugin.h"

NavgationPlugin::NavgationPlugin(QObject *parent)
    : QObject(parent),

      m_navWidget(nullptr)
{

}

void NavgationPlugin::initialize(FrameProxyInterface *proxy)
{
    m_proxyInter = proxy;

    m_navWidget = new NavWidget;
    connect(m_navWidget, &NavWidget::requestModule, this, &NavgationPlugin::showModule);
}

QWidget *NavgationPlugin::centralWidget()
{
    return m_navWidget;
}

void NavgationPlugin::showModule(const QString &module)
{
    m_proxyInter->showModulePage(module, QString());
}
