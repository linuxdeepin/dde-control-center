#include "soundcontrol.h"

#include <QJsonDocument>

namespace Plugin
{
namespace Sound
{

SoundControl::SoundControl(QObject *parent): QObject(parent)
{
    qDebug() << "SoundControl init";
    m_Model.initialized = false;

    m_dbusAudio = new DBusAudio(this);

    m_effects = new DBusSoundEffects("com.deepin.daemon.SoundEffect",
                                     "/com/deepin/daemon/SoundEffect",
                                     QDBusConnection::sessionBus(), this);
    m_meterTimer = new QTimer(this);
}

SoundControl::~SoundControl()
{
    qDebug() << "SoundControl destory";
}

void SoundControl::prepare()
{
    prepareModel(m_Model);
}

void SoundControl::prepareModel(SoundModel &model)
{
    model.activeSinkPort = m_dbusAudio->activeSinkPort();
    model.activeSourcePort = m_dbusAudio->activeSourcePort();
    model.sink.defaultSink = m_dbusAudio->defaultSink();
    model.source.defaultSource = m_dbusAudio->defaultSource();

    updatePorts(model);
    connect(m_dbusAudio, &DBusAudio::CardsChanged, this, &SoundControl::handleDBusCardsChanged);

    updateSinks(model);

    connect(m_dbusAudio, &DBusAudio::SinksChanged, this, &SoundControl::handleDBusSinksChanged);
    connect(m_dbusAudio, &DBusAudio::DefaultSinkChanged, this, &SoundControl::handleDBusDefaultSinkChanged);
    connect(m_dbusAudio, &DBusAudio::ActiveSinkPortChanged,
    [&] {
        if (m_Model.activeSinkPort != m_dbusAudio->activeSinkPort())
        {
            m_Model.activeSinkPort = m_dbusAudio->activeSinkPort();
            if (m_defaultsink) {
                m_Model.sink.volume    = m_defaultsink->volume();
                m_Model.sink.balance   = m_defaultsink->balance();
                m_Model.sink.mute      = m_defaultsink->mute();
            }
            emit outputActivePortChanged(m_Model);
        }
    });

    updateSources(model);
    connect(m_dbusAudio, &DBusAudio::SourcesChanged, this, &SoundControl::handleDBusSourcesChanged);
    connect(m_dbusAudio, &DBusAudio::DefaultSourceChanged, this, &SoundControl::handleDBusDefaultSourceChanged);
    connect(m_dbusAudio, &DBusAudio::ActiveSourcePortChanged,
    [&] {
        if (m_Model.activeSourcePort != m_dbusAudio->activeSourcePort())
        {
            m_Model.activeSourcePort = m_dbusAudio->activeSourcePort();
            if (m_defaultSource) {
                m_Model.source.volume    = m_defaultSource->volume();
                m_Model.source.mute      = m_defaultSource->mute();
            }
            emit inputActivePortChanged(m_Model);
        }
    });

    connect(m_effects, &DBusSoundEffects::EnabledChanged, [this] {
        emit this->effectsEnabledChanged(m_effects->enabled());
    });
    model.effect.enabled = m_effects->enabled();
    model.initialized = true;
}

void SoundControl::updatePorts(SoundModel &model)
{
    model.sinkPorts.clear();
    model.sourcePorts.clear();

    QString cardInfo = m_dbusAudio->cards();
    QJsonParseError err;
    auto cardsJson = QJsonDocument::fromJson(cardInfo.toUtf8(), &err);
    if (QJsonParseError::NoError != err.error) {
        qWarning() << "parse dbus cards info failed" << err.errorString();
        return;
    }

    for (QJsonValue card : cardsJson.array()) {
        uint cardIndex = card.toObject()["Id"].toInt();
        QJsonArray ports = card.toObject()["Ports"].toArray();
        for (QJsonValue port : ports) {
            SoundPort p;
            p.card = cardIndex;
            p.name = port.toObject()["Name"].toString();
            p.description = port.toObject()["Description"].toString();
            p.available = static_cast<PortStatus>(port.toObject()["Available"].toInt());
            p.direction = static_cast<PortDirection>(port.toObject()["Direction"].toInt());
            switch (p.direction) {
            case Output:
                model.sinkPorts.push_back(p);
                break;
            case Input:
                model.sourcePorts.push_back(p);
                break;
            default:
                qWarning() << "unkonw port type"  << p.name << p.direction;
                break;
            }
        }
    }
}

void SoundControl::updateSinks(SoundModel &model)
{
    m_sinks.clear();

    QString defaultSinkName = model.sink.defaultSink;
    DBusAudioSink *sinkFound = nullptr;

    QList<QDBusObjectPath> sinkPaths = m_dbusAudio->sinks();
    QString pathfound = "";
    foreach(QDBusObjectPath path, sinkPaths) {
        auto sink = new DBusAudioSink(path.path(), this);
        if (defaultSinkName == sink->name()) {
            sinkFound = sink;
            pathfound = path.path();
        }
        m_sinks << sink;
    }

    if (pathfound == model.sink.defaultSinkPath) {
        qDebug() << "sink not change";
        return;
    }

    if (pathfound.isEmpty()) {
        qDebug() << "sink not found";
        model.sink.defaultPortActive = false;
        model.sink.defaultSinkPath = "";
        emit outputChanged(model);
        return;
    }

    model.sink.defaultSinkPath = pathfound;
    model.sink.defaultPortActive = true;

    if (m_defaultsink) {
        m_defaultsink->deleteLater();
    }
    m_defaultsink = sinkFound;

    model.sink.volume         = m_defaultsink->volume();
    model.sink.balance        = m_defaultsink->balance();
    model.sink.mute           = m_defaultsink->mute();

    connect(m_defaultsink, &DBusAudioSink::VolumeChanged, this, &SoundControl::handleVolumeChnaged);
    connect(m_defaultsink, &DBusAudioSink::BalanceChanged, this, &SoundControl::handleBalanceChanged);
    connect(m_defaultsink, &DBusAudioSink::MuteChanged, this, &SoundControl::handleMuteChanged);

    emit outputChanged(model);
}

void SoundControl::updateSources(SoundModel &model)
{
    m_sources.clear();

    QString defaultSourceName = model.source.defaultSource;
    DBusAudioSource *activeSource = nullptr;

    QList<QDBusObjectPath> sourcePaths = m_dbusAudio->sources();
    QString pathfound = "";
    foreach(QDBusObjectPath path, sourcePaths) {
        auto source = new DBusAudioSource(path.path(), this);
        if (defaultSourceName == source->name()) {
            activeSource = source;
            pathfound = path.path();
        }
        m_sources << source;
    }

    if (pathfound == model.source.defaultSourcePath) {
        qDebug() << "source not change";
        return;
    }

    if (pathfound.isEmpty()) {
        qDebug() << "source not found";
        model.source.defaultSourceActive = false;
        model.source.defaultSourcePath = "";
        emit inputChanged(model);
        return;
    }

    model.source.defaultSourceActive    = true;
    model.source.defaultSourcePath = pathfound;

    if (m_defaultSource) {
        m_defaultSource->deleteLater();
    }

    m_defaultSource = activeSource;

    model.source.volume             = m_defaultSource->volume();
    model.source.mute               = m_defaultSource->mute();

    connect(m_defaultSource, &DBusAudioSource::VolumeChanged, this, &SoundControl::handleInputVolumeChnaged);
    connect(m_defaultSource, &DBusAudioSource::MuteChanged, this, &SoundControl::handleInputMuteChnaged);

#ifndef DCC_DISABLE_MICROPHONE_FEEDBACK
    // init meter
    QString meterPath = m_defaultSource->GetMeter().value().path();
    QString meterName = meterPath;
    meterName = meterName.replace("/", ".").mid(1);

    if (m_dbusMeter) {
        m_dbusMeter->deleteLater();
    }

    m_dbusMeter = new QDBusInterface("com.deepin.daemon.Audio",
                                     meterPath,
                                     meterName,
                                     QDBusConnection::sessionBus(),
                                     this);
    connect(m_meterTimer, &QTimer::timeout, m_dbusMeter, [&] {
        m_dbusMeter->call("Tick");
        m_meterTimer->start(5000);
    });

    QDBusConnection::sessionBus().connect(m_dbusMeter->service(),
                                          m_dbusMeter->path(),
                                          "org.freedesktop.DBus.Properties",
                                          "PropertiesChanged", "sa{sv}as",
                                          this, SLOT(handleMeterVolumeChanged(QDBusMessage)));
    m_meterTimer->start(5000);
#endif
    emit inputChanged(model);
}

void SoundControl::handleVolumeChnaged()
{
    if (m_defaultsink) {
        emit outputVolumeChanged(m_defaultsink->volume());
    }
}

void SoundControl::handleInputVolumeChnaged()
{
    if (m_defaultSource) {
        emit inputVolumeChanged(m_defaultSource->volume());
    }
}

void SoundControl::handleInputMuteChnaged()
{
    if (m_defaultSource) {
        emit inputMuteChanged(m_defaultSource->mute());
    }
}

void SoundControl::handleBalanceChanged()
{
    if (m_defaultsink) {
        emit outputBalanceChanged(m_defaultsink->balance());
    }
}

void SoundControl::handleMuteChanged()
{
    if (m_defaultsink) {
        emit outputMuteChanged(m_defaultsink->mute());
    }
}

void SoundControl::handleMeterVolumeChanged(const QDBusMessage &msg)
{
    QList<QVariant> arguments = msg.arguments();

    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    if (changedProps.contains("Volume")) {
//        qDebug() << "meter change to " << changedProps.value("Volume").toDouble();
        emit inputMeterVolumeChanged(changedProps.value("Volume").toDouble() * 100);
    }
}

void SoundControl::setEffectsEnabled(bool enable)
{
    m_effects->setEnabled(enable);
}

void SoundControl::handleDBusSourcesChanged()
{
    updateSources(m_Model);
}

void SoundControl::handleDBusDefaultSourceChanged()
{
    m_Model.source.defaultSource = m_dbusAudio->defaultSource();
}

void SoundControl::handleDBusSinksChanged()
{
    updateSinks(m_Model);
}

void SoundControl::handleDBusDefaultSinkChanged()
{
    qDebug() << "handleDBusDefaultSinkChanged";
    m_Model.sink.defaultSink = m_dbusAudio->defaultSink();
}

void SoundControl::reset()
{
    if (m_dbusAudio) {
        m_dbusAudio->Reset();
    }

    // effects dbus service is not always running, so we may not receive changed singal, need to reset check button state
    if (m_effects)
        emit this->effectsEnabledChanged(true);
}

void SoundControl::handleDBusCardsChanged()
{
    updatePorts(m_Model);
    emit cardsChanged(m_Model);
}

void SoundControl::init()
{
    if (!m_Model.initialized) {
        prepareModel(m_Model);
    }

    handleDBusCardsChanged();
    emit this->initialized(m_Model);
}

void SoundControl::setPort(uint card, const QString &name, int direction)
{
    if (direction == Output && name != m_Model.activeSinkPort) {
        m_Model.activeSinkPort = name;
        m_dbusAudio->SetPort(card, name, direction);
    }

    if (direction == Input && name != m_Model.activeSourcePort) {
        m_Model.activeSourcePort = name;
        m_dbusAudio->SetPort(card, name, direction);
    }
}

void SoundControl::setDefaultDeviceByIndex(int index)
{
    DBusAudioSink *defaultSink = m_sinks.value(index);
    if (defaultSink && defaultSink->name() != m_dbusAudio->defaultSink()) {
        m_dbusAudio->SetDefaultSink(defaultSink->name()).waitForFinished();
    }
}

void SoundControl::setDefaultOutputPortByIndex(int index)
{
    if (m_defaultsink) {
        SinkPortStruct port =  m_defaultsink->ports().value(index);
        if (port.name != m_defaultsink->activePort().name) {
            m_defaultsink->SetPort(port.id).waitForFinished();
        }
    }
}

void SoundControl::setMute(bool mute)
{
    if (m_defaultsink && mute != m_defaultsink->mute()) {
        m_defaultsink->SetMute(mute).waitForFinished();
    }
}

void SoundControl::setOutputVolume(double v, bool b)
{
    if (m_defaultsink && qAbs(v * 100 - m_defaultsink->volume() * 100) > 1) {
        m_defaultsink->SetVolume(v, b).waitForFinished();
    }
}

void SoundControl::setBalance(double v, bool b)
{
    if (m_defaultsink && qAbs(v * 100 - m_defaultsink->balance() * 100) > 1) {
        m_defaultsink->SetBalance(v, b).waitForFinished();
    }
}

void SoundControl::setInputVolume(double v, bool b)
{
    if (m_defaultSource && qAbs(v * 100 - m_defaultSource->volume() * 100) > 1) {
        m_defaultSource->SetVolume(v, b).waitForFinished();
    }
}

void SoundControl::setInputMute(bool mute)
{
    if (m_defaultSource && mute != m_defaultSource->mute()) {
        m_defaultSource->SetMute(mute).waitForFinished();
    }
}

void  SoundControl::setDefaultInputPortByIndex(int index)
{
    if (m_defaultSource) {
        SourcePortStruct port =  m_defaultSource->ports().value(index);
        if (port.name != m_defaultSource->activePort().name) {
            m_defaultSource->SetPort(port.id).waitForFinished();
        }
    }
}

void  SoundControl::setDefaultInputDeviceByIndex(int index)
{
    DBusAudioSource *source =  m_sources.value(index);
    if (source && source->name() != m_dbusAudio->defaultSource()) {
        m_dbusAudio->SetDefaultSource(source->name()).waitForFinished();
    }
}

}
}

