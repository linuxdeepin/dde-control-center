#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include "plugininterface.h"

#include <QLabel>
#include "notifymanager.h"

using namespace dcc;

class NotifyPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "notify.json")

public:
    explicit NotifyPlugin(QObject *parent = 0);

    void initialize(FrameProxyInterface *proxy);
    QWidget *centralWidget();

private:
    NotifyManager *m_notifymanager;
};

#endif // EXAMPLEPLUGIN_H
