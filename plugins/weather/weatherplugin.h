#ifndef WEATHERPLUGIN_H
#define WEATHERPLUGIN_H

#include "plugininterface.h"

class WeatherRequest;
class WeatherPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "weather.json")

public:
    explicit WeatherPlugin(QObject* parent = 0);

    void initialize(FrameProxyInterface *proxy);
    QWidget *centralWidget();

private:
    QWidget *m_view;
    WeatherRequest *m_requestManager;
};

#endif // WEATHERPLUGIN_H
