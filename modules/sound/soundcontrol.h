#ifndef SOUNDCONTROL_H
#define SOUNDCONTROL_H

#include <QObject>

#include "dbus/dbusaudio.h"
#include "dbus/dbusaudiosink.h"
#include "dbus/dbusaudiosource.h"
#include "dbus/dbussoundeffects.h"

namespace Plugin
{
namespace Sound
{

struct SinkModel {
    bool            defaultPortActive;

    QString         activePort;
    QStringList     portList;

    QString         defaultDevices;
    QStringList     devicesList;
    double          volume;
    double          balance;
    bool            mute;
};

struct SourcePort {
    struct SourcePortStruct;
    QString description;
};

struct SourceModel {
    bool                defaultSourceActive;
    SourcePortStruct    activeSourcePort;
    SourcePortList      sourcePortList;

    QString             activePort;
    QStringList         portList;

    QString             defaultDevices;
    QStringList         devicesList;
    double              volume;
    double              balance;
    bool                mute;
};

struct SoundEffect {
    bool    enabled;
};

// model should be pure data
struct SoundModel {
    bool        initialized;
    SinkModel   sink;
    SourceModel source;
    SoundEffect effect;
};

class SoundControl: public QObject
{
    Q_OBJECT

public:
    SoundControl(QObject *parent = nullptr);
    ~SoundControl() Q_DECL_OVERRIDE;

    void prepareModel(SoundModel &model);
    void updateSinks(SoundModel &model);
    void updateSources(SoundModel &model);

public slots:
    void prepare();
    void init();

    void setEffectsEnabled(bool);
    void setDefaultDeviceByIndex(int index);
    void setDefaultPortByIndex(int index);

    void setDefaultInputPortByIndex(int index);
    void setDefaultInputDeviceByIndex(int index);

    void setMute(bool);
    void setOutputVolume(double, bool);
    void setBalance(double, bool);

    void setInputVolume(double, bool);
    void setInputMute(bool);

signals:
    void initialized(const Plugin::Sound::SoundModel &model);
    void effectsEnabledChanged(bool);

    void sinksChanged(const Plugin::Sound::SoundModel &model);
    void defaultSinkChanged(const Plugin::Sound::SoundModel &model);

    void sourcesChanged(const Plugin::Sound::SoundModel &model);
    void defaultSourceChanged(const Plugin::Sound::SoundModel &model);

    void activePortChanged(const Plugin::Sound::SoundModel &model);
    void inputActivePortChanged(const Plugin::Sound::SoundModel &model);

    void meterVolumeChanged(int);
    void volumeChanged(double);
    void balanceChanged(double);
    void muteChanged(bool);

    void inputVolumeChanged(double);
    void inputMuteChanged(bool);

private slots:
    void handleDBusSinksChanged();
    void handleDBusDefaultSinkChanged();
    void handleDBusSourcesChanged();
    void handleDBusDefaultSourceChanged();

    void handleVolumeChnaged();
    void handleBalanceChanged();
    void handleMuteChanged();
    void handleActivePortChanged();
    void handleMeterVolumeChanged(const QDBusMessage &msg);

    void handleInputVolumeChnaged();
    void handleInputMuteChnaged();
    void handleInputActivePortChanged();

private:
    SoundModel          m_prepareModel;

    //DBus interface;
    DBusSoundEffects    *m_effects      = nullptr;
    DBusAudio           *m_dbusAudio    = nullptr;
    DBusAudioSink       *m_defaultsink  = nullptr;
    DBusAudioSource     *m_defaultSource = nullptr;
    QDBusInterface      *m_dbusMeter    = nullptr;
    QTimer              *m_meterTimer   = nullptr;

    QList<DBusAudioSink *>      m_sinks;
    QList<DBusAudioSource *>    m_sources;
};




}
}

Q_DECLARE_METATYPE(Plugin::Sound::SoundModel)

#endif // SOUNDCONTROL_H
