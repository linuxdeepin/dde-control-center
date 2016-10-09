#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include "plugininterface.h"

#include <QLabel>
#include "notifymanager.h"
#include <QScrollArea>
using namespace dde;

class NotifyPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "notify.json")

public:
    explicit NotifyPlugin(QObject *parent = 0);

    void initialize(FrameProxyInterface *proxy);
    QWidget *centeralWidget();

private:
    NotifyManager *m_notifymanager;
    QScrollArea *m_scrollarea;
};

#endif // EXAMPLEPLUGIN_H
