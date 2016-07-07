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

enum PortStatus {
    Unavailable = 0,
    Unknow = 1,
    Available = 2,
};

enum PortDirection {
    Output = 1,
    Input = 2,
};

struct SoundPort: public SourcePortStruct {
    uint            card;
    QString         description;
    PortStatus      available;
    PortDirection   direction;
};

struct SinkModel {
    bool            defaultPortActive;
    QString         defaultSink;
    QString         defaultSinkPath;
    double          volume;
    double          balance;
    bool            mute;
};

struct SourceModel {
    bool                defaultSourceActive;
    QString             defaultSource;
    QString             defaultSourcePath;
    double              volume;
    double              balance;
    bool                mute;
};

struct SoundEffect {
    bool    enabled;
};

// model should be pure data
struct SoundModel {
    bool                initialized;
    SinkModel           sink;
    SourceModel         source;
    SoundEffect         effect;
    QList<SoundPort>    sinkPorts;
    QList<SoundPort>    sourcePorts;
    QString             activeSinkPort;
    QString             activeSourcePort;
};

class SoundControl: public QObject
{
    Q_OBJECT

public:
    SoundControl(QObject *parent = nullptr);
    ~SoundControl() Q_DECL_OVERRIDE;

    void prepareModel(SoundModel &model);
    void updatePorts(SoundModel &model);
    void updateSinks(SoundModel &model);
    void updateSources(SoundModel &model);

public slots:
    void prepare();
    void init();

    void setPort(uint card, const QString &name, int direction);
    void setEffectsEnabled(bool);
    void setDefaultDeviceByIndex(int index);
    void setDefaultOutputPortByIndex(int index);

    void setDefaultInputPortByIndex(int index);
    void setDefaultInputDeviceByIndex(int index);

    void setMute(bool);
    void setOutputVolume(double, bool);
    void setBalance(double, bool);

    void setInputVolume(double, bool);
    void setInputMute(bool);

    void reset();

signals:
    void initialized(const Plugin::Sound::SoundModel &model);
    void effectsEnabledChanged(bool);

//    void sinksChanged(const Plugin::Sound::SoundModel &model);
//    void defaultSinkChanged(const Plugin::Sound::SoundModel &model);

//    void sourcesChanged(const Plugin::Sound::SoundModel &model);
//    void defaultSourceChanged(const Plugin::Sound::SoundModel &model);

    void outputActivePortChanged(const Plugin::Sound::SoundModel &model);
    void cardsChanged(const Plugin::Sound::SoundModel &model);
    // update spker
    void outputChanged(const Plugin::Sound::SoundModel &model);
    void outputVolumeChanged(double);
    void outputBalanceChanged(double);
    void outputMuteChanged(bool);

    void inputActivePortChanged(const Plugin::Sound::SoundModel &model);
    void inputChanged(const Plugin::Sound::SoundModel &model);
    void inputVolumeChanged(double);
    void inputMuteChanged(bool);
    void inputMeterVolumeChanged(int);

private slots:
    void handleDBusCardsChanged();
    void handleDBusSinksChanged();
    void handleDBusDefaultSinkChanged();
    void handleDBusSourcesChanged();
    void handleDBusDefaultSourceChanged();

    void handleVolumeChnaged();
    void handleBalanceChanged();
    void handleMuteChanged();
    void handleMeterVolumeChanged(const QDBusMessage &msg);

    void handleInputVolumeChnaged();
    void handleInputMuteChnaged();

private:
    SoundModel          m_Model;

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
