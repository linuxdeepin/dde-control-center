#ifndef PLUGINSCONTROLLER_H
#define PLUGINSCONTROLLER_H

#include "frameproxyinterface.h"

#include <QObject>

using namespace dde;

class PluginsController : public QObject, public FrameProxyInterface
{
    Q_OBJECT

public:
    explicit PluginsController(QObject *parent = 0);

signals:
    void pluginAdded(QWidget * const w);

public slots:
    void loadPlugins();

private:
    void pushWidget(ContentWidget * const) {}
};

#endif // PLUGINSCONTROLLER_H
