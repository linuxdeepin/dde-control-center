#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

class QLabel;
class QFrame;
class Sound: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "sound.json")
    Q_INTERFACES(ModuleInterface)

public:
    Sound();
    ~Sound() Q_DECL_OVERRIDE;
    QFrame* getContent() Q_DECL_OVERRIDE;

private:
    QLabel * m_label;
};

#endif
