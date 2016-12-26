#include "weatherplugin.h"

WeatherPlugin::WeatherPlugin(QObject *parent)
    :QObject(parent),
      m_weatherView(nullptr)
{

}

void WeatherPlugin::initialize(FrameProxyInterface *proxy)
{
    m_proxyInter = proxy;

    m_weatherView = new WeatherWidget;
}

QWidget *WeatherPlugin::centeralWidget()
{
    return m_weatherView;
}
