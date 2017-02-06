#ifndef EXAMPLEPLUGIN_H
#define EXAMPLEPLUGIN_H

#include "plugininterface.h"

#include <QLabel>

using namespace dcc;

class ExamplePlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "example.json")

public:
    explicit ExamplePlugin(QObject *parent = 0);

    void initialize(FrameProxyInterface *proxy);
    QWidget *centralWidget();

private:
    QLabel *m_exampleLbl;
};

#endif // EXAMPLEPLUGIN_H
