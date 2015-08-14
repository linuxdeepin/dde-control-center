#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

#include "dbus/dbusaudiosink.h"
#include "dbus/dbusaudiosource.h"

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
    QFrame * m_frame;

    com::deepin::daemon::Audio::Sink * m_sink;
    com::deepin::daemon::Audio::Source * m_source;

    void initBackend();
    void initUI();
};

#endif
