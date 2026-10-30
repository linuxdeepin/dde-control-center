// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "sounddbusproxy.h"
// #include "widgets/dccdbusinterface.h"

#include "audioport.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusPendingReply>
#include <QDBusReply>
#include <QDBusVariant>

#include <cmath>

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
const static QString PropertiesInterface = QStringLiteral("org.freedesktop.DBus.Properties");

class SoundMeterSubscription : public QObject
{
    Q_OBJECT

public:
    explicit SoundMeterSubscription(const QString &path, QObject *parent)
        : QObject(parent)
        , m_path(path)
    {
    }

    ~SoundMeterSubscription() override { stop(); }

    bool start()
    {
        m_stopped = !QDBusConnection::sessionBus().connect(
                AudioService,
                m_path,
                PropertiesInterface,
                QStringLiteral("PropertiesChanged"),
                { MeterInterface },
                QStringLiteral("sa{sv}as"),
                this,
                SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
        if (m_stopped)
            return false;

        QDBusMessage message = QDBusMessage::createMethodCall(AudioService,
                                                              m_path,
                                                              PropertiesInterface,
                                                              QStringLiteral("Get"));
        message << MeterInterface << QStringLiteral("Volume");
        // An unavailable initial value leaves the zero cache until a live update arrives.
        QDBusConnection::sessionBus().callWithCallback(message,
                                                       this,
                                                       SLOT(onInitialVolume(QDBusVariant)),
                                                       nullptr);
        return true;
    }

    void stop()
    {
        if (m_stopped)
            return;

        m_stopped = true;
        QDBusConnection::sessionBus().disconnect(
                AudioService,
                m_path,
                PropertiesInterface,
                QStringLiteral("PropertiesChanged"),
                { MeterInterface },
                QStringLiteral("sa{sv}as"),
                this,
                SLOT(onPropertiesChanged(QString, QVariantMap, QStringList)));
    }

    double volume() const { return m_volume; }

    void tick()
    {
        if (m_stopped)
            return;

        const QDBusMessage message = QDBusMessage::createMethodCall(AudioService,
                                                                    m_path,
                                                                    MeterInterface,
                                                                    QStringLiteral("Tick"));
        QDBusConnection::sessionBus().asyncCall(message);
    }

Q_SIGNALS:
    void volumeChanged(double value);

private Q_SLOTS:

    void onPropertiesChanged(const QString &interfaceName,
                             const QVariantMap &changedProperties,
                             const QStringList &invalidatedProperties)
    {
        Q_UNUSED(invalidatedProperties)
        if (m_stopped || interfaceName != MeterInterface)
            return;

        const auto volume = changedProperties.constFind(QStringLiteral("Volume"));
        if (volume != changedProperties.cend())
            updateVolume(volume.value(), true);
    }

    void onInitialVolume(const QDBusVariant &value)
    {
        if (m_stopped || m_receivedLiveVolume)
            return;

        updateVolume(value.variant(), false);
    }

private:
    void updateVolume(const QVariant &value, bool live)
    {
        bool valid = false;
        const double volume = value.toDouble(&valid);
        if (!valid || !std::isfinite(volume))
            return;

        if (live)
            m_receivedLiveVolume = true;
        if (m_volume == volume)
            return;

        m_volume = volume;
        Q_EMIT volumeChanged(volume);
    }

    const QString m_path;
    double m_volume = 0.0;
    bool m_stopped = true;
    bool m_receivedLiveVolume = false;
};

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
    if (m_defaultSink) {
        return qvariant_cast<bool>(m_defaultSink->property("MuteSink"));
    }
    return false;
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
    if (m_defaultSink) {
        return qvariant_cast<double>(m_defaultSink->property("BalanceSink"));
    }
    return 0.0;
}

double SoundDBusProxy::baseVolumeSink()
{
    if (m_defaultSink) {
        return qvariant_cast<double>(m_defaultSink->property("BaseVolumeSink"));
    }
    return 0.0;
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
    if (m_defaultSink) {
        return qvariant_cast<double>(m_defaultSink->property("VolumeSink"));
    }
    return 0.0;
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
    if (m_defaultSink) {
        return qvariant_cast<AudioPort>(m_defaultSink->property("ActivePortSink"));
    }
    return AudioPort();
}

uint SoundDBusProxy::cardSink()
{
    if (m_defaultSink) {
        return qvariant_cast<uint>(m_defaultSink->property("CardSink"));
    }
    return 0;
}

void SoundDBusProxy::setSourceDevicePath(const QString &path)
{
    if (m_defaultSource) {
        m_defaultSource->deleteLater();
        m_defaultSource = nullptr;
    }

    if (path.isEmpty() || path == QStringLiteral("/"))
        return;

    m_defaultSource = new DDBusInterface(AudioService,
                                         path,
                                         SourceInterface,
                                         QDBusConnection::sessionBus(),
                                         this);
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
    if (m_defaultSource) {
        return qvariant_cast<double>(m_defaultSource->property("VolumeSource"));
    }
    return 0.0;
}

AudioPort SoundDBusProxy::activePortSource()
{
    if (m_defaultSource) {
        return qvariant_cast<AudioPort>(m_defaultSource->property("ActivePortSource"));
    }
    return AudioPort();
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
    if (m_defaultSource) {
        return qvariant_cast<uint>(m_defaultSource->property("CardSource"));
    }
    return 0;
}

QDBusPendingCall SoundDBusProxy::GetMeter()
{
    if (!m_defaultSource || m_defaultSource->path().isEmpty()
        || m_defaultSource->path() == QStringLiteral("/")) {
        return QDBusPendingCall::fromError(
                QDBusError(QDBusError::UnknownObject, QStringLiteral("No valid audio source")));
    }

    return m_defaultSource->asyncCallWithArgumentList(QStringLiteral("GetMeter"), {});
}

bool SoundDBusProxy::setMeterDevicePath(const QString &path)
{
    clearMeterDevice();
    if (path.isEmpty() || path == QStringLiteral("/"))
        return false;

    auto *subscription = new SoundMeterSubscription(path, this);
    m_sourceMeter = subscription;
    connect(subscription,
            &SoundMeterSubscription::volumeChanged,
            this,
            [this, subscription](double value) {
                if (m_sourceMeter == subscription)
                    Q_EMIT VolumeMeterChanged(value);
            });
    if (!subscription->start()) {
        clearMeterDevice();
        return false;
    }
    return true;
}

void SoundDBusProxy::clearMeterDevice()
{
    auto *subscription = m_sourceMeter;
    m_sourceMeter = nullptr;
    if (!subscription)
        return;

    disconnect(subscription, nullptr, this, nullptr);
    subscription->stop();
    subscription->deleteLater();
}

double SoundDBusProxy::volumeMeter()
{
    return m_sourceMeter ? m_sourceMeter->volume() : 0.0;
}

void SoundDBusProxy::Tick()
{
    if (m_sourceMeter)
        m_sourceMeter->tick();
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
    if (m_defaultSource) {
        return qvariant_cast<bool>(m_defaultSource->property("MuteSource"));
    }
    return false;
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
        SetBalanceSink(oldBalance,false);
    });
}

#include "sounddbusproxy.moc"
