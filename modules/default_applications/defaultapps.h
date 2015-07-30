#ifndef DEFAULTAPPS_H
#define DEFAULTAPPS_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

class QLabel;
class QFrame;
class DefaultApps: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "default_applications.json")
    Q_INTERFACES(ModuleInterface)

public:
    DefaultApps();
    ~DefaultApps() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

private:
    QLabel * m_label;
};

#endif
