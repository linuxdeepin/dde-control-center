#include "soundcontrol.h"

namespace Plugin
{
namespace Sound
{

SoundControl::SoundControl(QObject *parent): QObject(parent)
{
    qDebug() << "SoundControl init";
    m_prepareModel.initialized = false;

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
    prepareModel(m_prepareModel);
}

void SoundControl::prepareModel(SoundModel &model)
{
    updateSinks(model);

    connect(m_dbusAudio, &DBusAudio::SinksChanged, this, &SoundControl::handleDBusSinksChanged);
    connect(m_dbusAudio, &DBusAudio::DefaultSinkChanged, this, &SoundControl::handleDBusDefaultSinkChanged);

    updateSources(model);

    connect(m_dbusAudio, &DBusAudio::SourcesChanged, this, &SoundControl::handleDBusSourcesChanged);
    connect(m_dbusAudio, &DBusAudio::DefaultSourceChanged, this, &SoundControl::handleDBusDefaultSourceChanged);

    connect(m_effects, &DBusSoundEffects::EnabledChanged, [this] {
        emit this->effectsEnabledChanged(m_effects->enabled());
    });
    model.effect.enabled = m_effects->enabled();

    model.initialized = true;
}

void SoundControl::updateSinks(SoundModel &model)
{
    qDebug() << "updateSinks";
    m_sinks.clear();

    QString defaultSinkName = m_dbusAudio->defaultSink();
    DBusAudioSink *activeSink = nullptr;

    QList<QDBusObjectPath> sinkPaths = m_dbusAudio->sinks();
    foreach(QDBusObjectPath path, sinkPaths) {
        auto sink = new DBusAudioSink(path.path(), this);
        if (defaultSinkName == sink->name()) {
            activeSink = sink;
        }
        m_sinks << sink;
    }

    model.sink.defaultPortActive = false;

    if (m_sinks.isEmpty()) {
        qWarning() << "sink is empty";
        return;
    }

    // check default port is vaild
    if (m_defaultsink) {
        m_defaultsink->deleteLater();
    }
    m_defaultsink = activeSink;

    if (activeSink) {
        model.sink.volume         = m_defaultsink->volume();
        model.sink.balance        = m_defaultsink->balance();
        model.sink.mute           = m_defaultsink->mute();
        model.sink.defaultPortActive = true;

        connect(m_defaultsink, &DBusAudioSink::ActivePortChanged, this, &SoundControl::handleActivePortChanged);
        connect(m_defaultsink, &DBusAudioSink::VolumeChanged, this, &SoundControl::handleVolumeChnaged);
        connect(m_defaultsink, &DBusAudioSink::BalanceChanged, this, &SoundControl::handleBalanceChanged);
        connect(m_defaultsink, &DBusAudioSink::MuteChanged, this, &SoundControl::handleMuteChanged);
    } else {
        activeSink = m_sinks.first();
    }

    model.sink.activePort =  activeSink->activePort().name;
    model.sink.portList.clear();
    for (auto &port : activeSink->ports()) {
        model.sink.portList << port.name;
    }

    model.sink.defaultDevices = activeSink->description();
    model.sink.devicesList.clear();
    for (auto sink : m_sinks) {
        model.sink.devicesList << sink->description();
    }
}

void SoundControl::updateSources(SoundModel &model)
{
    qDebug() << "updateSources";
    m_sources.clear();

    QString defaultSourceName = m_dbusAudio->defaultSource();
    DBusAudioSource *activeSource = nullptr;

    QList<QDBusObjectPath> sourcePaths = m_dbusAudio->sources();
    foreach(QDBusObjectPath path, sourcePaths) {
        auto source = new DBusAudioSource(path.path(), this);
        if (defaultSourceName == source->name()) {
            activeSource = source;
        }
        m_sources << source;
    }

    model.source.defaultSourceActive          = false;
    if (m_sources.isEmpty()) {
        qWarning() << "source is empty";
        return;
    }

    if (m_defaultSource) {
        m_defaultSource->deleteLater();
    }

    m_defaultSource = activeSource;

    if (!activeSource) {
        activeSource = m_sources.first();
    }

    model.source.volume             = activeSource->volume();
    model.source.mute               = activeSource->mute();
    model.source.activeSourcePort   = activeSource->activePort();
    model.source.sourcePortList     = activeSource->ports();

    model.source.activePort =  activeSource->activePort().name;
    model.source.portList.clear();
    for (auto &port : activeSource->ports()) {
        model.source.portList << port.name;
    }

    model.source.defaultDevices     = activeSource->description();
    model.source.devicesList.clear();
    for (auto source : m_sources) {
        model.source.devicesList << source->description();
    }

    if (!m_defaultSource) {
        return;
    }

    connect(m_defaultSource, &DBusAudioSource::VolumeChanged, this, &SoundControl::handleInputVolumeChnaged);
    connect(m_defaultSource, &DBusAudioSource::MuteChanged, this, &SoundControl::handleInputMuteChnaged);
    connect(m_defaultSource, &DBusAudioSource::ActivePortChanged, this, &SoundControl::handleInputActivePortChanged);

    model.source.defaultSourceActive    = true;

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
}

void SoundControl::handleActivePortChanged()
{
    SoundModel model;
    updateSinks(model);
    emit activePortChanged(model);
}

void SoundControl::handleInputActivePortChanged()
{
    SoundModel model;
    updateSources(model);
    emit inputActivePortChanged(model);
}

void SoundControl::handleVolumeChnaged()
{
    if (m_defaultsink) {
        emit volumeChanged(m_defaultsink->volume());
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
        emit balanceChanged(m_defaultsink->balance());
    }
}

void SoundControl::handleMuteChanged()
{
    if (m_defaultsink) {
        emit muteChanged(m_defaultsink->mute());
    }
}

void SoundControl::handleMeterVolumeChanged(const QDBusMessage &msg)
{
    QList<QVariant> arguments = msg.arguments();

    QVariantMap changedProps = qdbus_cast<QVariantMap>(arguments.at(1).value<QDBusArgument>());
    if (changedProps.contains("Volume")) {
//        qDebug() << "meter change to " << changedProps.value("Volume").toDouble();
        emit meterVolumeChanged(changedProps.value("Volume").toDouble() * 100);
    }
}

void SoundControl::setEffectsEnabled(bool enable)
{
    m_effects->setEnabled(enable);
}

void SoundControl::handleDBusSourcesChanged()
{
    SoundModel  model;
    updateSources(model);
    emit this->sourcesChanged(model);
}

void SoundControl::handleDBusDefaultSourceChanged()
{
    SoundModel  model;
    updateSources(model);
    emit this->defaultSourceChanged(model);
}

void SoundControl::handleDBusSinksChanged()
{
    SoundModel  model;
    updateSinks(model);
    emit this->sinksChanged(model);
}

void SoundControl::handleDBusDefaultSinkChanged()
{
    SoundModel  model;
    updateSinks(model);
    emit this->defaultSinkChanged(model);
}

void SoundControl::init()
{
    if (!m_prepareModel.initialized) {
        prepareModel(m_prepareModel);
    }
    emit this->initialized(m_prepareModel);
}

void SoundControl::setDefaultDeviceByIndex(int index)
{
    DBusAudioSink *defaultSink = m_sinks.value(index);
    if (defaultSink && defaultSink->name() != m_dbusAudio->defaultSink()) {
        m_dbusAudio->SetDefaultSink(defaultSink->name()).waitForFinished();
    }
}

void SoundControl::setDefaultPortByIndex(int index)
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

