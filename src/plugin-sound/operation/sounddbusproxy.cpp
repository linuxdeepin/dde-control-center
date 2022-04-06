#include "sounddbusproxy.h"

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


SoundDBusProxy::SoundDBusProxy(QObject *parent)
    : QObject(parent)
    , m_audioInter(new QDBusInterface(AudioService, AudioPath, AudioInterface, QDBusConnection::sessionBus(), this))
    , m_soundEffectInter(new QDBusInterface(SoundEffectService, SoundEffectPath, SoundEffectInterface, QDBusConnection::sessionBus(), this))
    , m_powerInter(new QDBusInterface(PowerService, PowerPath, PowerInterface, QDBusConnection::systemBus(), this))
    , m_defaultSink(nullptr)
    , m_defaultSource(nullptr)
    , m_sourceMeter(nullptr)
{
    qRegisterMetaType<AudioPort>("AudioPort");
    qDBusRegisterMetaType<AudioPort>();

    qRegisterMetaType<SoundEffectQuestions>("SoundEffectQuestions");
    qDBusRegisterMetaType<SoundEffectQuestions>();

    QDBusConnection::sessionBus().connect(AudioService, AudioPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::sessionBus().connect(SoundEffectService, SoundEffectPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
    QDBusConnection::systemBus().connect(PowerService, PowerPath, PropertiesInterface, PropertiesChanged, this, SLOT(onPropertiesChanged(QDBusMessage)));
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
    return qvariant_cast<bool>(m_powerInter->property("HasBattery"));
}

void SoundDBusProxy::setSinkDevicePath(const QString &path)
{
    if (m_defaultSink) {
        m_defaultSink->deleteLater();
        QDBusConnection::sessionBus().disconnect(AudioService, path, PropertiesInterface, PropertiesChanged, this, SLOT(onSinkPropertiesChanged(QDBusMessage)));
    }
    m_defaultSink = new QDBusInterface(AudioService, path, SinkInterface, QDBusConnection::sessionBus(), this);
    QDBusConnection::sessionBus().connect(AudioService, path, PropertiesInterface, PropertiesChanged, this, SLOT(onSinkPropertiesChanged(QDBusMessage)));
}

bool SoundDBusProxy::muteSink()
{
    return qvariant_cast<bool>(m_defaultSink->property("Mute"));
}

void SoundDBusProxy::SetSinkMute(bool in0)
{
    if (m_defaultSink) {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0);
        m_defaultSink->asyncCallWithArgumentList(QStringLiteral("SetMute"), argumentList);
    }
}

double SoundDBusProxy::balanceSink()
{
    return qvariant_cast<double>(m_defaultSink->property("Balance"));
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
    return qvariant_cast<double>(m_defaultSink->property("Volume"));
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
    QDBusInterface defaultSourceInterface(m_defaultSink->service(),m_defaultSink->path(),PropertiesInterface,m_defaultSink->connection());
    QDBusMessage p = defaultSourceInterface.call(QStringLiteral("Get"),m_defaultSink->interface(),QStringLiteral("ActivePort"));
    QVariant v  = p.arguments().first();
    const QDBusArgument arg = v.value<QDBusVariant>().variant().value<QDBusArgument>();
    AudioPort audio;
    arg>>audio;
    return audio;
}

uint SoundDBusProxy::cardSink()
{
    return qvariant_cast<uint>(m_defaultSink->property("Card"));
}

void SoundDBusProxy::setSourceDevicePath(const QString &path)
{
    if (m_defaultSource) {
        m_defaultSource->deleteLater();
        QDBusConnection::sessionBus().disconnect(AudioService, path, PropertiesInterface, PropertiesChanged, this, SLOT(onSourcePropertiesChanged(QDBusMessage)));
    }

    m_defaultSource = new QDBusInterface(AudioService, path, SourceInterface, QDBusConnection::sessionBus(), this);
    QDBusConnection::sessionBus().connect(AudioService, path, PropertiesInterface, PropertiesChanged, this, SLOT(onSourcePropertiesChanged(QDBusMessage)));
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
    return qvariant_cast<double>(m_defaultSource->property("Volume"));
}

void SoundDBusProxy::SetSourceVolume(double in0, bool in1)
{
    if (m_defaultSource) {
        QList<QVariant> argumentList;
        argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
        m_defaultSource->asyncCallWithArgumentList(QStringLiteral("SetVolume"), argumentList);
    }
}

AudioPort SoundDBusProxy::activeSourcePort()
{
    QDBusInterface defaultSourceInterface(m_defaultSource->service(),m_defaultSource->path(),PropertiesInterface,m_defaultSource->connection());
    QDBusMessage p = defaultSourceInterface.call(QStringLiteral("Get"),m_defaultSource->interface(),QStringLiteral("ActivePort"));
    QVariant v  = p.arguments().first();
    const QDBusArgument arg = v.value<QDBusVariant>().variant().value<QDBusArgument>();
    AudioPort audio;
    arg>>audio;
    return audio;
}

uint SoundDBusProxy::cardSource()
{
    return qvariant_cast<uint>(m_defaultSource->property("Card"));
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
        QDBusConnection::sessionBus().disconnect(AudioService, path, PropertiesInterface, PropertiesChanged, this, SLOT(onMeterPropertiesChanged(QDBusMessage)));
    }
    m_sourceMeter = new QDBusInterface(AudioService, path, SourceInterface, QDBusConnection::sessionBus(), this);
    QDBusConnection::sessionBus().connect(AudioService, path, PropertiesInterface, PropertiesChanged, this, SLOT(onMeterPropertiesChanged(QDBusMessage)));
}

double SoundDBusProxy::volumeSourceMeter()
{
    return qvariant_cast<double>(m_sourceMeter->property("Volume"));
}

void SoundDBusProxy::Tick()
{
    if (m_sourceMeter) {
        QList<QVariant> argumentList;
        m_sourceMeter->asyncCallWithArgumentList(QStringLiteral("Tick"), argumentList);
    }
}

bool SoundDBusProxy::muteSource()
{
    return qvariant_cast<bool>(m_defaultSource->property("Mute"));
}

void SoundDBusProxy::onPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "Changed", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
        qInfo()<<it.key()<<it.value()<<message.interface()<<message.path();
    }
}

void SoundDBusProxy::onSinkPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "ChangedSink", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}

void SoundDBusProxy::onSourcePropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "ChangedSource", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}

void SoundDBusProxy::onMeterPropertiesChanged(const QDBusMessage &message)
{
    QVariantMap changedProps = qdbus_cast<QVariantMap>(message.arguments().at(1).value<QDBusArgument>());
    for (QVariantMap::const_iterator it = changedProps.begin(); it != changedProps.end(); ++it) {
        QMetaObject::invokeMethod(this, it.key().toLatin1() + "ChangedMeter", Qt::DirectConnection, QGenericArgument(it.value().typeName(), it.value().data()));
    }
}
