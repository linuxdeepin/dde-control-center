// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "sounddbusproxy.h"
// #include "widgets/dccdbusinterface.h"

#include "audioport.h"
#include <QDBusArgument>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDBusReply>

const static QString AudioService = QStringLiteral("org.deepin.dde.Audio1");
const static QString AudioPath = QStringLiteral("/org/deepin/dde/Audio1");
const static QString AudioInterface = QStringLiteral("org.deepin.dde.Audio1");

const static QString SoundEffectService = QStringLiteral("org.deepin.dde.SoundEffect1");
const static QString SoundEffectPath = QStringLiteral("/org/deepin/dde/SoundEffect1");
const static QString SoundEffectInterface = QStringLiteral("org.deepin.dde.SoundEffect1");

const static QString PowerService = QStringLiteral("org.deepin.dde.Power1");
const static QString PowerPath = QStringLiteral("/org/deepin/dde/Power1");
const static QString PowerInterface = QStringLiteral("org.deepin.dde.Power1");

const static QString SinkInterface = QStringLiteral("org.deepin.dde.Audio1.Sink");
const static QString SourceInterface = QStringLiteral("org.deepin.dde.Audio1.Source");
const static QString MeterInterface = QStringLiteral("org.deepin.dde.Audio1.Meter");

// using namespace DCC_NAMESPACE;
SoundDBusProxy::SoundDBusProxy(QObject *parent)
    : QObject(parent)
    , m_audioInter(new DDBusInterface(AudioService, AudioPath, AudioInterface, QDBusConnection::sessionBus(), this))
    , m_soundEffectInter(new DDBusInterface(SoundEffectService, SoundEffectPath, SoundEffectInterface, QDBusConnection::sessionBus(), this))
    , m_powerInter(new DDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::systemBus(), this))
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
    return qvariant_cast<QDBusObjectPath>(m_audioInter->property("DefaultSink"));
}

QDBusObjectPath SoundDBusProxy::defaultSource()
{
    return qvariant_cast<QDBusObjectPath>(m_audioInter->property("DefaultSource"));
}

QString SoundDBusProxy::cardsWithoutUnavailable()
{
    return qvariant_cast<QString>(m_audioInter->property("CardsWithoutUnavailable"));
}

QStringList SoundDBusProxy::bluetoothAudioModeOpts()
{
    return qvariant_cast<QStringList>(m_audioInter->property("BluetoothAudioModeOpts"));
}

QString SoundDBusProxy::bluetoothAudioMode()
{
    return qvariant_cast<QString>(m_audioInter->property("BluetoothAudioMode"));
}

double SoundDBusProxy::maxUIVolume()
{
    return qvariant_cast<double>(m_audioInter->property("MaxUIVolume"));
}

bool SoundDBusProxy::increaseVolume()
{
    return qvariant_cast<bool>(m_audioInter->property("IncreaseVolume"));
}

void SoundDBusProxy::setIncreaseVolume(bool value)
{
    m_audioInter->setProperty("IncreaseVolume", QVariant::fromValue(value));
}

bool SoundDBusProxy::reduceNoise()
{
    return qvariant_cast<bool>(m_audioInter->property("ReduceNoise"));
}

void SoundDBusProxy::setReduceNoise(bool value)
{
    m_audioInter->setProperty("ReduceNoise", QVariant::fromValue(value));
}

bool SoundDBusProxy::pausePlayer()
{
    return qvariant_cast<bool>(m_audioInter->property("PausePlayer"));
}

void SoundDBusProxy::setPausePlayer(bool value)
{
    m_audioInter->setProperty("PausePlayer", QVariant::fromValue(value));
}

QString SoundDBusProxy::audioServer()
{
    return qvariant_cast<QString>(m_audioInter->property("CurrentAudioServer"));
}

void SoundDBusProxy::SetAudioServer(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    m_audioInter->asyncCallWithArgumentList(QStringLiteral("SetCurrentAudioServer"), argumentList);
}

bool SoundDBusProxy::audioServerState()
{
    return qvariant_cast<bool>(m_audioInter->property("AudioServerState"));
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
    return qvariant_cast<bool>(m_soundEffectInter->property("Enabled"));
}

void SoundDBusProxy::setEnabled(bool value)
{
    m_soundEffectInter->setProperty("Enabled", QVariant::fromValue(value));
}

void SoundDBusProxy::GetSoundEnabledMap()
{
    QList<QVariant> argumentList;
    m_soundEffectInter->callWithCallback(QStringLiteral("GetSoundEnabledMap"), argumentList, this, SIGNAL(pendingCallWatcherFinished(QMap<QString, bool>)));
}

void SoundDBusProxy::EnableSound(const QString &name, bool enabled, QObject *receiver, const char *member, const char *errorSlot)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(name) << QVariant::fromValue(enabled);
    m_soundEffectInter->callWithCallback(QStringLiteral("EnableSound"), argumentList, receiver, member, errorSlot);
}

QString SoundDBusProxy::GetSoundFile(const QString &name)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(name);
    return QDBusPendingReply<QString>(m_soundEffectInter->asyncCallWithArgumentList(QStringLiteral("GetSoundFile"), argumentList));
}

bool SoundDBusProxy::hasBattery()
{
    return qvariant_cast<bool>(m_powerInter->property("HasBattery"));
}

void SoundDBusProxy::setSinkDevicePath(const QString &path)
{
    if (m_defaultSink) {
        m_defaultSink->deleteLater();
    }
    m_defaultSink = new DDBusInterface(AudioService, path, SinkInterface, QDBusConnection::sessionBus(), this);
    m_defaultSink->setSuffix("Sink");
}

bool SoundDBusProxy::muteSink()
{
    return qvariant_cast<bool>(m_defaultSink->property("MuteSink"));
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
    return qvariant_cast<double>(m_defaultSink->property("BalanceSink"));
}

double SoundDBusProxy::baseVolumeSink()
{
     return qvariant_cast<double>(m_defaultSink->property("BaseVolumeSink"));
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
    return qvariant_cast<double>(m_defaultSink->property("VolumeSink"));
}

void SoundDBusProxy::SetVolumeSink(double in0, bool in1)
{
    if (m_defaultSink) {
        QList<QVariant> argumentList;
        // Round in0 to 2 decimal places to avoid floating point precision issues
        double roundedIn0 = qRound(in0 * 100.0) / 100.0;
        argumentList << QVariant::fromValue(roundedIn0) << QVariant::fromValue(in1);
        m_defaultSink->asyncCallWithArgumentList(QStringLiteral("SetVolume"), argumentList);
    }
}

AudioPort SoundDBusProxy::activePortSink()
{
    return qvariant_cast<AudioPort>(m_defaultSink->property("ActivePortSink"));
}

uint SoundDBusProxy::cardSink()
{
    return qvariant_cast<uint>(m_defaultSink->property("CardSink"));
}

void SoundDBusProxy::setSourceDevicePath(const QString &path)
{
    if (m_defaultSource) {
        m_defaultSource->deleteLater();
    }

    m_defaultSource = new DDBusInterface(AudioService, path, SourceInterface, QDBusConnection::sessionBus(), this);
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
    return qvariant_cast<double>(m_defaultSource->property("VolumeSource"));
}

AudioPort SoundDBusProxy::activePortSource()
{
    return qvariant_cast<AudioPort>(m_defaultSource->property("ActivePortSource"));
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
    return qvariant_cast<uint>(m_defaultSource->property("CardSource"));
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
    m_sourceMeter = new DDBusInterface(AudioService, path, MeterInterface, QDBusConnection::sessionBus(), this);
    m_sourceMeter->setSuffix("Meter");
}

double SoundDBusProxy::volumeMeter()
{
    return qvariant_cast<double>(m_sourceMeter->property("VolumeMeter"));
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
    return qvariant_cast<QList<QDBusObjectPath>>(m_audioInter->property("SinkInputs"));
}

QList<QDBusObjectPath> SoundDBusProxy::sinks()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_audioInter->property("Sinks"));
}

QList<QDBusObjectPath> SoundDBusProxy::sources()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_audioInter->property("Sources"));
}

bool SoundDBusProxy::muteSource()
{
    return qvariant_cast<bool>(m_defaultSource->property("MuteSource"));
}

bool SoundDBusProxy::audioMono()
{
    return qvariant_cast<bool>(m_audioInter->property("Mono"));
}

void SoundDBusProxy::setAudioMono(bool audioMono)
{
    double oldBalance = balanceSink();
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(audioMono);
    QDBusPendingCall call = m_audioInter->asyncCallWithArgumentList(QStringLiteral("SetMono"), argumentList);

    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, call, watcher, oldBalance] {
        if (call.isError()) {
            qWarning() << " set Audio Mono error: " << call.error().message();
        }
        Q_EMIT AudioMonoChanged(this->audioMono());
        watcher->deleteLater();
        SetBalanceSink(oldBalance,true);
    });
}
