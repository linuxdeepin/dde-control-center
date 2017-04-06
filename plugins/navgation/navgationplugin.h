#ifndef NAVGATIONPLUGIN_H
#define NAVGATIONPLUGIN_H

#include "plugininterface.h"
#include "navwidget.h"

#include <QLabel>

class NavgationPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "navgation.json")

public:
    explicit NavgationPlugin(QObject *parent = 0);

    void initialize(FrameProxyInterface *proxy);
    QWidget *centralWidget();

private slots:
    void showModule(const QString &module);

private:
    NavWidget *m_navWidget;
};

#endif // NAVGATIONPLUGIN_H
