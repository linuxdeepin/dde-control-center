#ifndef WEATHERPLUGIN_H
#define WEATHERPLUGIN_H

#include "plugininterface.h"
#include "weatherwidget.h"

class WeatherPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "weather.json")

public:
    explicit WeatherPlugin(QObject* parent = 0);

    void initialize(FrameProxyInterface *proxy);
    QWidget *centeralWidget();

private:
    WeatherWidget *m_weatherView;
};

#endif // WEATHERPLUGIN_H
