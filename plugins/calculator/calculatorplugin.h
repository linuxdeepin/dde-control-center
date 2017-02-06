#ifndef CALCULATORPLUGIN_H
#define CALCULATORPLUGIN_H

#include "plugininterface.h"

#include <QLabel>

class CalculatorPlugin : public QObject, public PluginInterface
{
    Q_OBJECT
    Q_INTERFACES(PluginInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "calculator.json")

public:
    explicit CalculatorPlugin(QObject *parent = 0);

    void initialize(FrameProxyInterface *proxy);
    QWidget *centralWidget();

private:
    QLabel *m_exampleLbl;
};

#endif // CALCULATORPLUGIN_H
