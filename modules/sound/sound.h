#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QtPlugin>
#include "interfaces.h"

#include <libdui/dswitchlineexpand.h>
#include <libdui/dslider.h>
#include <libdui/dbuttonlist.h>
#include <libdui/dbaseexpand.h>

#include "dbus/dbusaudio.h"
#include "dbus/dbusaudiosink.h"
#include "dbus/dbusaudiosource.h"

DUI_USE_NAMESPACE

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

    DSwitchLineExpand * m_speakerExpand;
    DSwitchLineExpand * m_microphoneExpand;
    DSlider * m_outputVolumeSlider;
    DSlider * m_leftRightBalanceSlider;
    DSlider * m_inputVolumeSlider;
    DSlider * m_inputFeedbackSlider;
    DBaseExpand * m_outputPortsExpand;
    DButtonList * m_outputPortsList;
    DBaseExpand * m_inputPortsExpand;
    DButtonList * m_inputPortsList;


    DBusAudio * m_dbusAudio;
    DBusAudioSink * m_sink;
    DBusAudioSource * m_source;
    QList<DBusAudioSink *> m_sinks;
    QList<DBusAudioSource *> m_sources;
    QDBusInterface *m_dbusMeter;
    QTimer m_meterTimer;

    void initBackend();
    void initUI();

    com::deepin::daemon::Audio::Sink * getDefaultSink();
    com::deepin::daemon::Audio::Source * getDefaultSource();

    void updateSpeakerUI();
    void updateMicrophoneUI();
    void updateOutputPorts();
    void updateInputPorts();
    Q_SLOT void meterVolumeChanged(const QDBusMessage& msg);
};

#endif
