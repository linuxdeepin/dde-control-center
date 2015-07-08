#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

class QFrame;
class Power: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "/tmp/power.json")
    Q_INTERFACES(ModuleInterface)

public:
    ~Power() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;
};

#endif
