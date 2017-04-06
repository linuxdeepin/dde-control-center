#ifndef PLUGINSCONTROLLER_H
#define PLUGINSCONTROLLER_H

#include "frameproxyinterface.h"

#include <QObject>

using namespace dcc;

class PluginsController : public QObject, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit PluginsController(QObject *parent = 0);

signals:
    void pluginAdded(QWidget * const w);
    void requestModulePage(const QString &module, const QString &page);

public slots:
    void loadPlugins();

private:
    void pushWidget(ModuleInterface * const, ContentWidget * const) {}
    void setFrameAutoHide(ModuleInterface * const, const bool) {}
    void setModuleVisible(ModuleInterface * const, const bool) {}
    void showModulePage(const QString &module, const QString &page);
};

#endif // PLUGINSCONTROLLER_H
