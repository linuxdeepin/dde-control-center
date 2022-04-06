#ifndef SOUNDDBUSPROXY_H
#define SOUNDDBUSPROXY_H

#include "audioport.h"
#include <QDBusObjectPath>
#include <QObject>

typedef QMap<QString, bool>  SoundEffectQuestions;

class QDBusInterface;
class QDBusMessage;
class SoundDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit SoundDBusProxy(QObject *parent = nullptr);

    // Audio
    bool isServiceRegistered();

    QDBusObjectPath defaultSink();
    QDBusObjectPath defaultSource();
    QString cardsWithoutUnavailable();
    QStringList bluetoothAudioModeOpts();
    QString bluetoothAudioMode();
    double maxUIVolume();

    bool increaseVolume();
    void setIncreaseVolume(bool value);

    bool reduceNoise();
    void setReduceNoise(bool value);

    void SetPortEnabled(uint in0, const QString &in1, bool in2);

    void SetPort(uint in0, const QString &in1, int in2);

    void SetBluetoothAudioMode(const QString &in0);

    // SoundEffect
    bool enabled();
    void setEnabled(bool value);
    void GetSoundEnabledMap();
    void EnableSound(const QString &name, bool enabled);
    QString GetSoundFile(const QString &name);

    // Power
    bool hasBattery();


    // Sink
    void setSinkDevicePath(const QString &path);

    bool muteSink();
    void SetSinkMute(bool in0);

    double balanceSink();
    void SetBalanceSink(double in0, bool in1);

    double volumeSink();
    void SetVolumeSink(double in0, bool in1);

    AudioPort activePortSink();
    uint cardSink();

    // Source
    void setSourceDevicePath(const QString &path);

    bool muteSource();
    void SetSourceMute(bool in0);

    double volumeSource();
    void SetSourceVolume(double in0, bool in1);

    AudioPort activeSourcePort();
    uint cardSource();

    QDBusObjectPath GetMeter();

    // SourceMeter
    void setMeterDevicePath(const QString &path);
    double volumeSourceMeter();
    void Tick();

Q_SIGNALS:
    // Audio SIGNALS
    void PortEnabledChanged(uint in0, const QString &in1, bool in2);
    void BluetoothAudioModeChanged(const QString & value) const;
    void BluetoothAudioModeOptsChanged(const QStringList & value) const;
    void CardsChanged(const QString & value) const;
    void CardsWithoutUnavailableChanged(const QString & value) const;
    void DefaultSinkChanged(const QDBusObjectPath & value) const;
    void DefaultSourceChanged(const QDBusObjectPath & value) const;
    void IncreaseVolumeChanged(bool  value) const;
    void MaxUIVolumeChanged(double  value) const;
    void ReduceNoiseChanged(bool  value) const;
    void SinkInputsChanged(const QList<QDBusObjectPath> & value) const;
    void SinksChanged(const QList<QDBusObjectPath> & value) const;
    void SourcesChanged(const QList<QDBusObjectPath> & value) const;

    // SoundEffect SIGNALS
    void EnabledChanged(bool value) const;
    void pendingCallWatcherFinished(QMap<QString, bool> map);

    // Power SIGNALS
    void HasBatteryChanged(bool  value) const;

    // Sink SIGNALS
    void MuteChangedSink(bool  value) const;
    void CardChangedSink(uint  value) const;
    void VolumeChangedSink(double  value) const;
    void ActivePortChangedSink(AudioPort  value) const;
    void BalanceChangedSink(double  value) const;

    // Source SIGNALS
    void MuteChangedSource(bool  value) const;
    void VolumeChangedSource(double  value) const;
    void ActivePortChangedSource(AudioPort  value) const;
    void CardChangedSource(uint  value) const;

    // Meter SIGNALS
    void VolumeChangedMeter(double  value) const;

private slots:
    void onPropertiesChanged(const QDBusMessage &message);
    void onSinkPropertiesChanged(const QDBusMessage &message);
    void onSourcePropertiesChanged(const QDBusMessage &message);
    void onMeterPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_audioInter;
    QDBusInterface *m_soundEffectInter;
    QDBusInterface *m_powerInter;

    QDBusInterface *m_defaultSink;
    QDBusInterface *m_defaultSource;
    QDBusInterface *m_sourceMeter;
};

#endif // SOUNDDBUSPROXY_H
