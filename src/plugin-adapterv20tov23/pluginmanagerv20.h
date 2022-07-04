
#ifndef PLUGINMANAGERV20_H
#define PLUGINMANAGERV20_H
#include "frameproxyinterface.h"
#include "interface/moduleobject.h"

class AdapterV20toV23Module;

class PluginManagerV20
{
public:
    explicit PluginManagerV20();
    ~PluginManagerV20();

    QStringList pluginPath();
    void loadPlugin(QString path, dccV20::FrameProxyInterface *frameProxy);
    QList<AdapterV20toV23Module *> modules() { return m_modules; }

private:
    QList<AdapterV20toV23Module *> m_modules;
};
#endif // PLUGINMANAGERV20_H
