#include "sounddbusproxy.h"
#include "widgets/dccdbusinterface.h"

#include "audioport.h"
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDBusReply>

const static QString AudioService = QStringLiteral("com.deepin.daemon.Audio");
const static QString AudioPath = QStringLiteral("/com/deepin/daemon/Audio");
const static QString AudioInterface = QStringLiteral("com.deepin.daemon.Audio");

const static QString SoundEffectService = QStringLiteral("com.deepin.daemon.SoundEffect");
const static QString SoundEffectPath = QStringLiteral("/com/deepin/daemon/SoundEffect");
const static QString SoundEffectInterface = QStringLiteral("com.deepin.daemon.SoundEffect");

const static QString PowerService = QStringLiteral("com.deepin.system.Power");
const static QString PowerPath = QStringLiteral("/com/deepin/system/Power");
const static QString PowerInterface = QStringLiteral("com.deepin.system.Power");

const static QString SinkInterface = QStringLiteral("com.deepin.daemon.Audio.Sink");
const static QString SourceInterface = QStringLiteral("com.deepin.daemon.Audio.Source");
const static QString MeterInterface = QStringLiteral("com.deepin.daemon.Audio.Meter");

const static QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");
const static QString PropertiesChanged = QStringLiteral("PropertiesChanged");

DCC_USE_NAMESPACE
SoundDBusProxy::SoundDBusProxy(QObject *parent)
    : QObject(parent)
    , m_audioInter(new DCCDBusInterface(AudioService, AudioPath, AudioInterface, QDBusConnection::sessionBus(), this))
    , m_soundEffectInter(new DCCDBusInterface(SoundEffectService, SoundEffectPath, SoundEffectInterface, QDBusConnection::sessionBus(), this))
    , m_powerInter(new DCCDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::systemBus(), this))
    , m_defaultSink(nullptr)
    , m_defaultSource(nullptr)
    , m_sourceMeter(nullptr)
{
    qRegisterMetaType<AudioPort>("AudioPort");
    qDBusRegisterMetaType<AudioPort>();

    qRegisterMetaType<SoundEffectQuestions>("SoundEffectQuestions");
    qDBusRegisterMetaType<SoundEffectQuestions>();
}

QDBusObjectPath SoundDBusProxy::defaultSink()
{
    return qvariant_cast<QDBusObjectPath>(m_audioInter->internalPropGet("DefaultSink"));
}

QDBusObjectPath SoundDBusProxy::defaultSource()
{
    return qvariant_cast<QDBusObjectPath>(m_audioInter->internalPropGet("DefaultSource"));
}

QString SoundDBusProxy::cardsWithoutUnavailable()
{
    return qvariant_cast<QString>(m_audioInter->internalPropGet("CardsWithoutUnavailable"));
}

QStringList SoundDBusProxy::bluetoothAudioModeOpts()
{
    return qvariant_cast<QStringList>(m_audioInter->internalPropGet("BluetoothAudioModeOpts"));
}

QString SoundDBusProxy::bluetoothAudioMode()
{
    return qvariant_cast<QString>(m_audioInter->internalPropGet("BluetoothAudioMode"));
}

double SoundDBusProxy::maxUIVolume()
{
    return qvariant_cast<double>(m_audioInter->internalPropGet("MaxUIVolume"));
}

bool SoundDBusProxy::increaseVolume()
{
    return qvariant_cast<bool>(m_audioInter->internalPropGet("IncreaseVolume"));
}

void SoundDBusProxy::setIncreaseVolume(bool value)
{
    m_audioInter->internalPropSet("IncreaseVolume", QVariant::fromValue(value));
}

bool SoundDBusProxy::reduceNoise()
{
    return qvariant_cast<bool>(m_audioInter->internalPropGet("ReduceNoise"));
}

void SoundDBusProxy::setReduceNoise(bool value)
{
    m_audioInter->internalPropSet("ReduceNoise", QVariant::fromValue(value));
}

void SoundDBusProxy::SetPortEnabled(uint in0, const QString &in1, bool in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    m_audioInter->asyncCallWithArgumentList(QStringLiteral("SetPortEnabled"), argumentList);
}

void SoundDBusProxy::SetPort(uint in0, const QString &in1, int in2)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1) << QVariant::fromValue(in2);
    m_audioInter->asyncCallWithArgumentList(QStringLiteral("SetPort"), argumentList);
}

void SoundDBusProxy::SetBluetoothAudioMode(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_audioInter->asyncCallWithArgumentList(QStringLiteral("SetBluetoothAudioMode"), argumentList);
}

bool SoundDBusProxy::enabled()
{
    return qvariant_cast<bool>(m_soundEffectInter->internalPropGet("Enabled"));
}

void SoundDBusProxy::setEnabled(bool value)
{
    m_soundEffectInter->internalPropSet("Enabled", QVariant::fromValue(value));
}

void SoundDBusProxy::GetSoundEnabledMap()
{
    QList<QVariant> argumentList;
    m_soundEffectInter->callWithCallback(QStringLiteral("GetSoundEnabledMap"), argumentList, this, SIGNAL(pendingCallWatcherFinished(QMap<QString, bool>)));
}

void SoundDBusProxy::EnableSound(const QString &name, bool enabled)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(name) << QVariant::fromValue(enabled);
    m_soundEffectInter->asyncCallWithArgumentList(QStringLiteral("EnableSound"), argumentList);
}

QString SoundDBusProxy::GetSoundFile(const QString &name)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(name);
    return QDBusPendingReply<QString>(m_soundEffectInter->asyncCallWithArgumentList(QStringLiteral("GetSoundFile"), argumentList));
}

bool SoundDBusProxy::hasBattery()
{
    return qvariant_cast<bool>(m_powerInter->internalPropGet("HasBattery"));
}

void SoundDBusProxy::setSinkDevicePath(const QString &path)
{
    if (m_defaultSink) {
        m_defaultSink->deleteLater();
    }
    m_defaultSink = new DCCDBusInterface(AudioService, path, SinkInterface, QDBusConnection::sessionBus(), this);
    m_defaultSink->setSuffix("Sink");
}

bool SoundDBusProxy::muteSink()
{
    return qvariant_cast<bool>(m_defaultSink->internalPropGet("MuteSink"));
}

void SoundDBusProxy::SetMuteSink(bool in0)
{
    if (m_defaultSink) {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        m_defaultSink->asyncCallWithArgumentList(QStringLiteral("SetMute"), argumentList);
    }
}

double SoundDBusProxy::balanceSink()
{
    return qvariant_cast<double>(m_defaultSink->internalPropGet("BalanceSink"));
}

double SoundDBusProxy::baseVolumeSink()
{
     return qvariant_cast<double>(m_defaultSink->internalPropGet("BaseVolumeSink"));
}

void SoundDBusProxy::SetBalanceSink(double in0, bool in1)
{
    if (m_defaultSink) {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        m_defaultSink->asyncCallWithArgumentList(QStringLiteral("SetBalance"), argumentList);
    }
}

double SoundDBusProxy::volumeSink()
{
    return qvariant_cast<double>(m_defaultSink->internalPropGet("VolumeSink"));
}

void SoundDBusProxy::SetVolumeSink(double in0, bool in1)
{
    if (m_defaultSink) {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        m_defaultSink->asyncCallWithArgumentList(QStringLiteral("SetVolume"), argumentList);
    }
}

AudioPort SoundDBusProxy::activePortSink()
{
    return qvariant_cast<AudioPort>(m_defaultSink->internalPropGet("ActivePortSink"));
}

uint SoundDBusProxy::cardSink()
{
    return qvariant_cast<uint>(m_defaultSink->internalPropGet("CardSink"));
}

void SoundDBusProxy::setSourceDevicePath(const QString &path)
{
    if (m_defaultSource) {
        m_defaultSource->deleteLater();
    }

    m_defaultSource = new DCCDBusInterface(AudioService, path, SourceInterface, QDBusConnection::sessionBus(), this);
    m_defaultSource->setSuffix("Source");
}

void SoundDBusProxy::SetSourceMute(bool in0)
{
    if (m_defaultSource) {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        m_defaultSource->asyncCallWithArgumentList(QStringLiteral("SetMute"), argumentList);
    }
}

double SoundDBusProxy::volumeSource()
{
    return qvariant_cast<double>(m_defaultSource->internalPropGet("VolumeSource"));
}

AudioPort SoundDBusProxy::activePortSource()
{
    return qvariant_cast<AudioPort>(m_defaultSource->internalPropGet("ActivePortSource"));
}

void SoundDBusProxy::SetSourceVolume(double in0, bool in1)
{
    if (m_defaultSource) {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        m_defaultSource->asyncCallWithArgumentList(QStringLiteral("SetVolume"), argumentList);
    }
}

uint SoundDBusProxy::cardSource()
{
    return qvariant_cast<uint>(m_defaultSource->internalPropGet("CardSource"));
}

QDBusObjectPath SoundDBusProxy::GetMeter()
{
    QList<QVariant> argumentList;
    return QDBusPendingReply<QDBusObjectPath>(m_defaultSource->asyncCallWithArgumentList(QStringLiteral("GetMeter"), argumentList));
}

void SoundDBusProxy::setMeterDevicePath(const QString &path)
{
    if (m_sourceMeter) {
        m_sourceMeter->deleteLater();
    }
    m_sourceMeter = new DCCDBusInterface(AudioService, path, MeterInterface, QDBusConnection::sessionBus(), this);
    m_sourceMeter->setSuffix("Meter");
}

double SoundDBusProxy::volumeMeter()
{
    return qvariant_cast<double>(m_sourceMeter->internalPropGet("VolumeMeter"));
}

void SoundDBusProxy::Tick()
{
    if (m_sourceMeter) {
        QList<QVariant> argumentList;
        m_sourceMeter->asyncCallWithArgumentList(QStringLiteral("Tick"), argumentList);
    }
}

QList<QDBusObjectPath> SoundDBusProxy::sinkInputs()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_audioInter->internalPropGet("SinkInputs"));
}

QList<QDBusObjectPath> SoundDBusProxy::sinks()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_audioInter->internalPropGet("Sinks"));
}

QList<QDBusObjectPath> SoundDBusProxy::sources()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_audioInter->internalPropGet("Sources"));
}

bool SoundDBusProxy::muteSource()
{
    return qvariant_cast<bool>(m_defaultSource->internalPropGet("MuteSource"));
}
