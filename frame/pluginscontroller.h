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

private:
    void pushWidget(QString mid, QWidget *w);

private slots:
    void loadPlugins();
};

#endif // PLUGINSCONTROLLER_H
