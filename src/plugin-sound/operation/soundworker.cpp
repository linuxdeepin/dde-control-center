//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "soundworker.h"

#include <QJsonDocument>
#include <QTimer>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QLoggingCategory>

Q_LOGGING_CATEGORY(DdcSoundWorker, "dcc-sound-worker")

#define GSETTINGS_WAIT_SOUND_RECEIPT "wait-sound-receipt"

SoundWorker::SoundWorker(SoundModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_activeOutputCard(UINT_MAX)
    , m_activeInputCard(UINT_MAX)
    , m_soundDBusInter(new SoundDBusProxy(this))
    , m_pingTimer(new QTimer(this))
    , m_inter(QDBusConnection::sessionBus().interface())
{
    m_pingTimer->setInterval(5000);
    m_pingTimer->setSingleShot(false);
    m_waitSoundPortReceipt = 1000;
    initConnect();
}

void SoundWorker::initConnect()
{
    connect(m_model, &SoundModel::defaultSinkChanged, this, &SoundWorker::defaultSinkChanged);
    connect(m_model, &SoundModel::defaultSourceChanged, this, &SoundWorker::defaultSourceChanged);
    connect(m_model, &SoundModel::audioCardsChanged, this, &SoundWorker::cardsChanged);

    connect(m_soundDBusInter, &SoundDBusProxy::DefaultSinkChanged, m_model, &SoundModel::setDefaultSink);
    connect(m_soundDBusInter, &SoundDBusProxy::DefaultSourceChanged, m_model, &SoundModel::setDefaultSource);
    connect(m_soundDBusInter, &SoundDBusProxy::MaxUIVolumeChanged, m_model, &SoundModel::setMaxUIVolume);
    connect(m_soundDBusInter, &SoundDBusProxy::IncreaseVolumeChanged, m_model, &SoundModel::setIncreaseVolume);
    connect(m_soundDBusInter, &SoundDBusProxy::CardsWithoutUnavailableChanged, m_model, &SoundModel::setAudioCards);
    connect(m_soundDBusInter, &SoundDBusProxy::ReduceNoiseChanged, m_model, &SoundModel::setReduceNoise);
    connect(m_soundDBusInter, &SoundDBusProxy::PausePlayerChanged, m_model, &SoundModel::setPausePlayer);
    connect(m_soundDBusInter, &SoundDBusProxy::BluetoothAudioModeOptsChanged, m_model, &SoundModel::setBluetoothAudioModeOpts);
    connect(m_soundDBusInter, &SoundDBusProxy::BluetoothAudioModeChanged, m_model, &SoundModel::setCurrentBluetoothAudioMode);

    connect(m_soundDBusInter, &SoundDBusProxy::EnabledChanged, m_model, &SoundModel::setEnableSoundEffect);
    connect(m_soundDBusInter, &SoundDBusProxy::pendingCallWatcherFinished, this, &SoundWorker::getSoundEnabledMapFinished);

    connect(m_pingTimer, &QTimer::timeout, [this] { if (m_soundDBusInter) m_soundDBusInter->Tick(); });
    connect(m_soundDBusInter, &SoundDBusProxy::HasBatteryChanged, m_model, &SoundModel::setIsLaptop);

    connect(m_soundDBusInter, &SoundDBusProxy::CurrentAudioServerChanged, m_model, &SoundModel::setAudioServer);
    connect(m_soundDBusInter, &SoundDBusProxy::AudioServerStateChanged, m_model, &SoundModel::setAudioServerChangedState);
}

void SoundWorker::activate()
{
    m_model->setDefaultSink(m_soundDBusInter->defaultSink());
    m_model->setDefaultSource(m_soundDBusInter->defaultSource());
    m_model->setAudioCards(m_soundDBusInter->cardsWithoutUnavailable());
    m_model->setIsLaptop(m_soundDBusInter->hasBattery());
    m_model->setMaxUIVolume(m_soundDBusInter->maxUIVolume());
    m_model->setIncreaseVolume(m_soundDBusInter->increaseVolume());
    m_model->setReduceNoise(m_soundDBusInter->reduceNoise());
    m_model->setPausePlayer(m_soundDBusInter->pausePlayer());
    m_model->setBluetoothAudioModeOpts(m_soundDBusInter->bluetoothAudioModeOpts());
    m_model->setCurrentBluetoothAudioMode(m_soundDBusInter->bluetoothAudioMode());
    m_model->setEnableSoundEffect(m_soundDBusInter->enabled());
    m_model->setWaitSoundReceiptTime(m_waitSoundPortReceipt);
    m_model->setAudioServer(m_soundDBusInter->audioServer());
    m_model->setAudioServerChangedState(m_soundDBusInter->audioServerState());

    m_pingTimer->start();
    m_soundDBusInter->blockSignals(false);

    defaultSinkChanged(m_model->defaultSink());
    defaultSourceChanged(m_model->defaultSource());
    cardsChanged(m_model->audioCards());
}

void SoundWorker::deactivate()
{
    m_pingTimer->stop();

    m_soundDBusInter->blockSignals(true);
}

void SoundWorker::refreshSoundEffect()
{
    m_model->setEnableSoundEffect(m_soundDBusInter->enabled());
    m_soundDBusInter->GetSoundEnabledMap();
}

void SoundWorker::setAudioServer(const QString &value)
{
    m_soundDBusInter->SetAudioServer(value);
    m_model->setAudioServer(value);
}

void SoundWorker::switchSpeaker(bool on)
{
    m_soundDBusInter->SetMuteSink(!on);
}

void SoundWorker::switchMicrophone(bool on)
{
    m_soundDBusInter->SetSourceMute(!on);
}

void SoundWorker::setPortEnabled(unsigned int cardid, QString portName, bool enable)
{
    if (m_soundDBusInter)
        m_soundDBusInter->SetPortEnabled(cardid, portName, enable);
}

void SoundWorker::setSinkBalance(double balance)
{
    m_soundDBusInter->SetBalanceSink(balance, true);
    qCDebug(DdcSoundWorker) << "set balance to " << balance;

}

void SoundWorker::setSourceVolume(double volume)
{
    m_soundDBusInter->SetSourceVolume(volume, true);
    qCDebug(DdcSoundWorker) << "set source volume to " << volume;
}

void SoundWorker::setSinkVolume(double volume)
{
    m_soundDBusInter->SetVolumeSink(volume, true);
    qCDebug(DdcSoundWorker) << "set sink volume to " << volume;
}

//切换输入静音状态，flag为false时直接取消静音
void SoundWorker::setSinkMute(bool flag)
{
    if (flag) {
        m_soundDBusInter->SetMuteSink(!m_soundDBusInter->muteSink());
    } else if (m_soundDBusInter->muteSink()) {
        m_soundDBusInter->SetMuteSink(false);
    }
}

//通知后端切换静音状态,flag为false时直接取消静音
void SoundWorker::setSourceMute(bool flag)
{
    if (flag) {
        m_soundDBusInter->SetSourceMute(!m_soundDBusInter->muteSource());
    } else if (m_soundDBusInter->muteSource()) {
        m_soundDBusInter->SetSourceMute(false);
    }
}

void SoundWorker::setIncreaseVolume(bool value)
{
    m_soundDBusInter->setIncreaseVolume(value);
}

void SoundWorker::setReduceNoise(bool value)
{
    m_soundDBusInter->setReduceNoise(value);
}

void SoundWorker::setPausePlayer(bool value)
{
    m_soundDBusInter->setPausePlayer(value);
}

void SoundWorker::setPort(const Port *port)
{
    m_soundDBusInter->SetPort(port->cardId(), port->id(), int(port->direction()));
    qCDebug(DdcSoundWorker) << "cardID:" << port->cardId()  << "portName:" << port->name() << "  " << port->id() << "  " << port->direction();
    m_model->setPort(port);
}

void SoundWorker::setEffectEnable(DDesktopServices::SystemSoundEffect effect, bool enable)
{
    m_soundDBusInter->EnableSound(m_model->getNameByEffectType(effect), enable, this , SLOT(refreshSoundEffect()), SLOT(refreshSoundEffect()));
}

void SoundWorker::enableAllSoundEffect(bool enable)
{
    m_soundDBusInter->setEnabled(enable);
}

void SoundWorker::setBluetoothMode(const QString &mode)
{
    m_soundDBusInter->SetBluetoothAudioMode(mode);
}

void SoundWorker::defaultSinkChanged(const QDBusObjectPath &path)
{
    qCDebug(DdcSoundWorker) << "sink default path:" << path.path();
    if (path.path().isEmpty() || path.path() == "/" )
        return; //路径为空


    m_soundDBusInter->setSinkDevicePath(path.path());
    connect(m_soundDBusInter, &SoundDBusProxy::MuteSinkChanged, [this](bool mute) { m_model->setSpeakerOn(mute);});
    connect(m_soundDBusInter, &SoundDBusProxy::BalanceSinkChanged, m_model, &SoundModel::setSpeakerBalance);
    connect(m_soundDBusInter, &SoundDBusProxy::VolumeSinkChanged, m_model, &SoundModel::setSpeakerVolume);
    connect(m_soundDBusInter, &SoundDBusProxy::ActivePortSinkChanged, this, &SoundWorker::activeSinkPortChanged);
    connect(m_soundDBusInter, &SoundDBusProxy::CardSinkChanged, this, &SoundWorker::onSinkCardChanged);

    m_model->setSpeakerOn(m_soundDBusInter->muteSink());
    m_model->setSpeakerBalance(m_soundDBusInter->balanceSink());
    m_model->setSpeakerVolume(m_soundDBusInter->volumeSink());
    activeSinkPortChanged(m_soundDBusInter->activePortSink());
    onSinkCardChanged(m_soundDBusInter->cardSink());
}

void SoundWorker::defaultSourceChanged(const QDBusObjectPath &path)
{
    qDebug() << "source default path:" << path.path();
    if (path.path().isEmpty() || path.path() == "/" ) return; //路径为空

    m_soundDBusInter->setSourceDevicePath(path.path());

    connect(m_soundDBusInter, &SoundDBusProxy::MuteSourceChanged, [this](bool mute) { m_model->setMicrophoneOn(mute); });
    connect(m_soundDBusInter, &SoundDBusProxy::VolumeSourceChanged, m_model, &SoundModel::setMicrophoneVolume);
    connect(m_soundDBusInter, &SoundDBusProxy::ActivePortSourceChanged, this, &SoundWorker::activeSourcePortChanged);
    connect(m_soundDBusInter, &SoundDBusProxy::CardSourceChanged, this, &SoundWorker::onSourceCardChanged);

    m_model->setMicrophoneOn(m_soundDBusInter->muteSource());
    m_model->setMicrophoneVolume(m_soundDBusInter->volumeSource());
    activeSourcePortChanged(m_soundDBusInter->activePortSource());
    onSourceCardChanged(m_soundDBusInter->cardSource());

#ifndef DCC_DISABLE_FEEDBACK
    QDBusObjectPath meter = m_soundDBusInter->GetMeter();
    if (meter.path().isEmpty())
        return;
    m_soundDBusInter->setMeterDevicePath(meter.path());
    connect(m_soundDBusInter, &SoundDBusProxy::VolumeMeterChanged, m_model, &SoundModel::setMicrophoneFeedback);
    m_model->setMicrophoneFeedback(m_soundDBusInter->volumeMeter());

#endif
}

void SoundWorker::cardsChanged(const QString &cards)
{
    QMap<uint, QStringList> tmpCardIds;
    QJsonDocument doc = QJsonDocument::fromJson(cards.toUtf8());
    QJsonArray jCards = doc.array();
    for (QJsonValue cV : jCards) {
        QJsonObject jCard = cV.toObject();
        const uint cardId = static_cast<uint>(jCard["Id"].toInt());
        const QString cardName = jCard["Name"].toString();
        QJsonArray jPorts = jCard["Ports"].toArray();

        QStringList tmpPorts;

        for (QJsonValue pV : jPorts) {
            QJsonObject jPort = pV.toObject();
            const double portAvai = jPort["Available"].toDouble();
            if (portAvai == 2.0 || portAvai == 0.0) {  // 0 Unknown 1 Not available 2 Available
                const QString portId = jPort["Name"].toString();
                const QString portName = jPort["Description"].toString();
                const bool isEnabled = jPort["Enabled"].toBool();
                const bool isBluetooth = jPort["Bluetooth"].toBool();

                Port *port = m_model->findPort(portId, cardId);
                const bool include = port != nullptr;
                if (!include) { port = new Port(m_model); }

                port->setId(portId);
                port->setName(portName);
                port->setDirection(Port::Direction(jPort["Direction"].toDouble()));
                port->setCardId(cardId);
                port->setCardName(cardName);
                port->setEnabled(isEnabled);
                port->setIsBluetoothPort(isBluetooth);

                const bool isActiveOuputPort = (portId == m_activeSinkPort) && (cardId == m_activeOutputCard);
                const bool isActiveInputPort = (portId == m_activeSourcePort) && (cardId == m_activeInputCard);

                port->setIsActive(isActiveInputPort || isActiveOuputPort);

                if (!include) { m_model->addPort(port); }

                tmpPorts << portId;
            }
        }
        if (!jPorts.isEmpty())
            tmpCardIds.insert(cardId, tmpPorts);
    }

    for (Port *port : m_model->ports()) {
        //if the card is not in the list
        if (!tmpCardIds.contains(port->cardId())) {
            m_model->removePort(port->id(), port->cardId());
        } else if (!tmpCardIds[port->cardId()].contains(port->id())) {
            m_model->removePort(port->id(), port->cardId());
        }
    }
}

void SoundWorker::activeSinkPortChanged(const AudioPort &activeSinkPort)
{
    qCDebug(DdcSoundWorker) << "active sink port changed to: " << activeSinkPort.name;
    m_activeSinkPort = activeSinkPort.name;

    for (auto port : m_model->ports()) {
        if (m_activeSinkPort == port->id()) {
            m_model->setPort(port);
        }
    }

    updatePortActivity();
}

void SoundWorker::activeSourcePortChanged(const AudioPort &activeSourcePort)
{
    qCDebug(DdcSoundWorker) << "active source port changed to: " << activeSourcePort.name;
    m_activeSourcePort = activeSourcePort.name;

    updatePortActivity();
}

void SoundWorker::onSinkCardChanged(const uint &cardId)
{
    m_activeOutputCard = cardId;

    updatePortActivity();
}

void SoundWorker::onSourceCardChanged(const uint &cardId)
{
    m_activeInputCard = cardId;

    updatePortActivity();
}

void SoundWorker::getSoundEnabledMapFinished(QMap<QString, bool> map)
{
    for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
        if (!m_model->checkSEExist(it.key())) continue;

        DDesktopServices::SystemSoundEffect type = m_model->getEffectTypeByGsettingName(it.key());
        m_model->setEffectData(type, it.value());

        QString path = m_soundDBusInter->GetSoundFile(it.key());
        m_model->updateSoundEffectPath(type, path);
    }
}


void SoundWorker::getSoundPathFinished(QDBusPendingCallWatcher *watcher)
{
    if (!watcher->isError()) {
        QDBusReply<QString> reply = watcher->reply();
        m_model->updateSoundEffectPath(
                    watcher->property("Type").value<DDesktopServices::SystemSoundEffect>(),
                    reply.value());
    } else {
        qCDebug(DdcSoundWorker) << "get sound path error." << watcher->error();
    }

    watcher->deleteLater();
}

void SoundWorker::updatePortActivity()
{
    for (Port *port : m_model->ports()) {
        const bool isActiveOuputPort = (port->id() == m_activeSinkPort) && (port->cardId() == m_activeOutputCard);
        const bool isActiveInputPort = (port->id() == m_activeSourcePort) && (port->cardId() == m_activeInputCard);
        port->setIsActive(isActiveInputPort || isActiveOuputPort);
    }
}
