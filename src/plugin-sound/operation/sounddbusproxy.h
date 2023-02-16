//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDDBUSPROXY_H
#define SOUNDDBUSPROXY_H

#include "interface/namespace.h"
#include "audioport.h"
#include <QDBusObjectPath>
#include <QObject>

typedef QMap<QString, bool> SoundEffectQuestions;

namespace DCC_NAMESPACE {
class DCCDBusInterface;
}

class QDBusMessage;
class SoundDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit SoundDBusProxy(QObject *parent = nullptr);

    // Audio
    bool isServiceRegistered();

    void SetPortEnabled(uint in0, const QString &in1, bool in2);

    void SetPort(uint in0, const QString &in1, int in2);

    void SetBluetoothAudioMode(const QString &in0);

    // SoundEffect
    void GetSoundEnabledMap();
    void EnableSound(const QString &name, bool enabled, QObject *receiver, const char *member, const char *errorSlot);
    QString GetSoundFile(const QString &name);

    // Power

    // Sink
    void setSinkDevicePath(const QString &path);

    void SetMuteSink(bool in0);

    void SetBalanceSink(double in0, bool in1);

    void SetVolumeSink(double in0, bool in1);

    // Source
    void setSourceDevicePath(const QString &path);

    void SetSourceMute(bool in0);

    void SetSourceVolume(double in0, bool in1);

    QDBusObjectPath GetMeter();

    // SourceMeter
    void setMeterDevicePath(const QString &path);
    void Tick();

    // Audio
    Q_PROPERTY(double MaxUIVolume READ maxUIVolume NOTIFY MaxUIVolumeChanged)
    double maxUIVolume();
    Q_PROPERTY(bool IncreaseVolume READ increaseVolume WRITE setIncreaseVolume NOTIFY IncreaseVolumeChanged)
    bool increaseVolume();
    void setIncreaseVolume(bool value);
    Q_PROPERTY(bool ReduceNoise READ reduceNoise WRITE setReduceNoise NOTIFY ReduceNoiseChanged)
    bool reduceNoise();
    void setReduceNoise(bool value);
    Q_PROPERTY(QString BluetoothAudioMode READ bluetoothAudioMode NOTIFY BluetoothAudioModeChanged)
    QString bluetoothAudioMode();
    Q_PROPERTY(QStringList BluetoothAudioModeOpts READ bluetoothAudioModeOpts NOTIFY BluetoothAudioModeOptsChanged)
    QStringList bluetoothAudioModeOpts();
    Q_PROPERTY(QString CardsWithoutUnavailable READ cardsWithoutUnavailable NOTIFY CardsWithoutUnavailableChanged)
    QString cardsWithoutUnavailable();
    Q_PROPERTY(QDBusObjectPath DefaultSource READ defaultSource NOTIFY DefaultSourceChanged)
    QDBusObjectPath defaultSource();
    Q_PROPERTY(QDBusObjectPath DefaultSink READ defaultSink NOTIFY DefaultSinkChanged)
    QDBusObjectPath defaultSink();
    Q_PROPERTY(QList<QDBusObjectPath> SinkInputs READ sinkInputs NOTIFY SinkInputsChanged)
    QList<QDBusObjectPath> sinkInputs();
    Q_PROPERTY(QList<QDBusObjectPath> Sinks READ sinks NOTIFY SinksChanged)
    QList<QDBusObjectPath> sinks();
    Q_PROPERTY(QList<QDBusObjectPath> Sources READ sources NOTIFY SourcesChanged)
    QList<QDBusObjectPath> sources();
    // Sink
    Q_PROPERTY(bool MuteSink READ muteSink NOTIFY MuteSinkChanged)
    bool muteSink();
    Q_PROPERTY(double BalanceSink READ balanceSink NOTIFY BalanceSinkChanged)
    double balanceSink();
    Q_PROPERTY(double BaseVolumeSink READ baseVolumeSink NOTIFY BaseVolumeSinkChanged)
    double baseVolumeSink();
    Q_PROPERTY(uint CardSink READ cardSink NOTIFY CardSinkChanged)
    uint cardSink();
    Q_PROPERTY(double VolumeSink READ volumeSink NOTIFY VolumeSinkChanged)
    double volumeSink();
    Q_PROPERTY(AudioPort ActivePortSink READ activePortSink NOTIFY ActivePortSinkChanged)
    AudioPort activePortSink();
    // Source
    Q_PROPERTY(bool MuteSource READ muteSource NOTIFY MuteSourceChanged)
    bool muteSource();
    Q_PROPERTY(uint CardSource READ cardSource NOTIFY CardSourceChanged)
    uint cardSource();
    Q_PROPERTY(double VolumeSource READ volumeSource NOTIFY VolumeSourceChanged)
    double volumeSource();
    Q_PROPERTY(AudioPort ActivePortSource READ activePortSource NOTIFY ActivePortSourceChanged)
    AudioPort activePortSource();
    // Power
    Q_PROPERTY(bool HasBattery READ hasBattery NOTIFY HasBatteryChanged)
    bool hasBattery();
    // SoundEffect
    Q_PROPERTY(bool Enabled READ enabled WRITE setEnabled NOTIFY EnabledChanged)
    bool enabled();
    void setEnabled(bool value);
    // Audio.Meter
    Q_PROPERTY(double VolumeMeter READ volumeMeter NOTIFY VolumeMeterChanged)
    double volumeMeter();

Q_SIGNALS:
    // Audio SIGNALS
    void PortEnabledChanged(uint in0, const QString &in1, bool in2);
    void BluetoothAudioModeChanged(const QString &value) const;
    void BluetoothAudioModeOptsChanged(const QStringList &value) const;
    void CardsChanged(const QString &value) const;
    void CardsWithoutUnavailableChanged(const QString &value) const;
    void DefaultSinkChanged(const QDBusObjectPath &value) const;
    void DefaultSourceChanged(const QDBusObjectPath &value) const;
    void IncreaseVolumeChanged(bool value) const;
    void MaxUIVolumeChanged(double value) const;
    void ReduceNoiseChanged(bool value) const;
    void SinkInputsChanged(const QList<QDBusObjectPath> &value) const;
    void SinksChanged(const QList<QDBusObjectPath> &value) const;
    void SourcesChanged(const QList<QDBusObjectPath> &value) const;

    // SoundEffect SIGNALS
    void EnabledChanged(bool value) const;
    void pendingCallWatcherFinished(QMap<QString, bool> map);

    // Power SIGNALS
    void HasBatteryChanged(bool value) const;

    // Sink SIGNALS
    void MuteSinkChanged(bool value) const;
    void BalanceSinkChanged(double value) const;
    void BaseVolumeSinkChanged(double value) const;
    void CardSinkChanged(uint value) const;
    void VolumeSinkChanged(double value) const;
    void ActivePortSinkChanged(AudioPort value) const;

    // Source SIGNALS
    void MuteSourceChanged(bool value) const;
    void VolumeSourceChanged(double value) const;
    void ActivePortSourceChanged(AudioPort value) const;
    void CardSourceChanged(uint value) const;

    // Meter SIGNALS
    void VolumeMeterChanged(double value) const;

private:
    DCC_NAMESPACE::DCCDBusInterface *m_audioInter;
    DCC_NAMESPACE::DCCDBusInterface *m_soundEffectInter;
    DCC_NAMESPACE::DCCDBusInterface *m_powerInter;

    DCC_NAMESPACE::DCCDBusInterface *m_defaultSink;
    DCC_NAMESPACE::DCCDBusInterface *m_defaultSource;
    DCC_NAMESPACE::DCCDBusInterface *m_sourceMeter;
};

#endif // SOUNDDBUSPROXY_H
