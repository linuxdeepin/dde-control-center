/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "soundworker.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QGSettings>

namespace dcc {
namespace sound {

#define GSETTINGS_WAIT_SOUND_RECEIPT "wait-sound-receipt"

SoundWorker::SoundWorker(SoundModel *model, QObject *parent)
    : QObject(parent)
    , m_model(model)
    , m_activeOutputCard(UINT_MAX)
    , m_activeInputCard(UINT_MAX)
    , m_audioInter(new Audio("com.deepin.daemon.Audio", "/com/deepin/daemon/Audio", QDBusConnection::sessionBus(), this))
    , m_soundEffectInter(new SoundEffect("com.deepin.daemon.SoundEffect", "/com/deepin/daemon/SoundEffect", QDBusConnection::sessionBus(), this))
    , m_defaultSink(nullptr)
    , m_defaultSource(nullptr)
    , m_sourceMeter(nullptr)
    , m_powerInter(new SystemPowerInter("com.deepin.system.Power", "/com/deepin/system/Power", QDBusConnection::systemBus(), this))
    , m_dccSettings(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
    , m_pingTimer(new QTimer(this))
    , m_inter(QDBusConnection::sessionBus().interface())
{
    m_audioInter->setSync(false);
    m_powerInter->setSync(false);

    m_pingTimer->setInterval(5000);
    m_pingTimer->setSingleShot(false);

    m_waitSoundPortReceipt = m_dccSettings->get(GSETTINGS_WAIT_SOUND_RECEIPT).toInt();

    if (m_inter->isServiceRegistered(m_audioInter->service()).value()) {
        initConnect();
    } else {
        connect(m_inter, &QDBusConnectionInterface::serviceOwnerChanged, this, [=] (const QString &name, const QString &oldOwner, const QString &newOwner) {
            Q_UNUSED(oldOwner)
            if (name == m_audioInter->service() && !newOwner.isEmpty()) {
                qDebug() << objectName() << m_audioInter->service() << "daemon started, init connection and disconnect";
                initConnect();
                disconnect(m_inter);
            }
        });
    }
}

void SoundWorker::initConnect()
{
    connect(m_model, &SoundModel::defaultSinkChanged, this, &SoundWorker::defaultSinkChanged);
    connect(m_model, &SoundModel::defaultSourceChanged, this, &SoundWorker::defaultSourceChanged);
    connect(m_model, &SoundModel::audioCardsChanged, this, &SoundWorker::cardsChanged);

    connect(m_audioInter, &Audio::DefaultSinkChanged, m_model, &SoundModel::setDefaultSink);
    connect(m_audioInter, &Audio::DefaultSourceChanged, m_model, &SoundModel::setDefaultSource);
    connect(m_audioInter, &Audio::MaxUIVolumeChanged, m_model, &SoundModel::setMaxUIVolume);
    connect(m_audioInter, &Audio::IncreaseVolumeChanged, m_model, &SoundModel::setIncreaseVolume);
    connect(m_audioInter, &Audio::CardsWithoutUnavailableChanged, m_model, &SoundModel::setAudioCards);
    connect(m_audioInter, &Audio::ReduceNoiseChanged, m_model, &SoundModel::setReduceNoise);
    connect(m_audioInter, &Audio::BluetoothAudioModeOptsChanged, m_model, &SoundModel::setBluetoothAudioModeOpts);
    connect(m_audioInter, &Audio::BluetoothAudioModeChanged, m_model, &SoundModel::setCurrentBluetoothAudioMode);
    connect(m_soundEffectInter, &SoundEffect::EnabledChanged, m_model, &SoundModel::setEnableSoundEffect);

    connect(m_pingTimer, &QTimer::timeout, [this] { if (m_sourceMeter) m_sourceMeter->Tick(); });
    connect(m_powerInter, &SystemPowerInter::HasBatteryChanged, m_model, &SoundModel::setIsLaptop);
    connect(m_dccSettings, &QGSettings::changed, this, &SoundWorker::onGsettingsChanged);

    m_model->setDefaultSink(m_audioInter->defaultSink());
    m_model->setDefaultSource(m_audioInter->defaultSource());
    m_model->setAudioCards(m_audioInter->cardsWithoutUnavailable());
    m_model->setIsLaptop(m_powerInter->hasBattery());
    m_model->setMaxUIVolume(m_audioInter->maxUIVolume());
    m_model->setIncreaseVolume(m_audioInter->increaseVolume());
    m_model->setReduceNoise(m_audioInter->reduceNoise());
    m_model->setBluetoothAudioModeOpts(m_audioInter->bluetoothAudioModeOpts());
    m_model->setCurrentBluetoothAudioMode(m_audioInter->bluetoothAudioMode());
    m_model->setWaitSoundReceiptTime(m_waitSoundPortReceipt);
}

void SoundWorker::activate()
{
    m_pingTimer->start();

    m_audioInter->blockSignals(false);
    if (m_defaultSink) m_defaultSink->blockSignals(false);
    if (m_defaultSource) m_defaultSource->blockSignals(false);
    if (m_sourceMeter) m_sourceMeter->blockSignals(false);

    defaultSinkChanged(m_model->defaultSink());
    defaultSourceChanged(m_model->defaultSource());
    cardsChanged(m_model->audioCards());
}

void SoundWorker::deactivate()
{
    m_pingTimer->stop();

    m_audioInter->blockSignals(true);
    if (m_defaultSink) m_defaultSink->blockSignals(true);
    if (m_defaultSource) m_defaultSource->blockSignals(true);
    if (m_sourceMeter) m_sourceMeter->blockSignals(true);
}

void SoundWorker::refreshSoundEffect()
{
    m_model->setEnableSoundEffect(m_soundEffectInter->enabled());

    QDBusPendingCall call = m_soundEffectInter->GetSoundEnabledMap();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, &SoundWorker::getSoundEnabledMapFinished);
}

void SoundWorker::switchSpeaker(bool on)
{
    if (m_defaultSink) {
        m_defaultSink->SetMute(!on);
    }
}

void SoundWorker::switchMicrophone(bool on)
{
    if (m_defaultSource) {
        m_defaultSource->SetMute(!on);
    }
}

void SoundWorker::setPortEnabled(unsigned int cardid, QString portName, bool enable)
{
    if (m_audioInter)
        m_audioInter->SetPortEnabled(cardid, portName, enable);
}

void SoundWorker::setSinkBalance(double balance)
{
    if (m_defaultSink) {
        m_defaultSink->SetBalance(balance, true);
        qDebug() << "set balance to " << balance;
    }
}

void SoundWorker::setSourceVolume(double volume)
{
    if (m_defaultSource) {
        m_defaultSource->SetVolume(volume, true);
        qDebug() << "set source volume to " << volume;
    }
}

void SoundWorker::setSinkVolume(double volume)
{
    if (m_defaultSink) {
        m_defaultSink->SetVolume(volume, true);
        qDebug() << "set sink volume to " << volume;
    }
}

//切换输入静音状态，flag为false时直接取消静音
void SoundWorker::setSinkMute(bool flag)
{
    if (!m_defaultSink)
        return;
    if (flag) {
        m_defaultSink->SetMute(!m_defaultSink->mute());
    } else if (m_defaultSink->mute()) {
        m_defaultSink->SetMute(false);
    }
}

//通知后端切换静音状态,flag为false时直接取消静音
void SoundWorker::setSourceMute(bool flag)
{
    if (!m_defaultSource)
        return;
    if (flag) {
        m_defaultSource->SetMute(!m_defaultSource->mute());
    } else if (m_defaultSource->mute()) {
        m_defaultSource->SetMute(false);
    }
}

void SoundWorker::setIncreaseVolume(bool value)
{
    m_audioInter->setIncreaseVolume(value);
}

void SoundWorker::setReduceNoise(bool value)
{
    m_audioInter->setReduceNoise(value);
}

void SoundWorker::setPort(const Port *port)
{
    auto rep = m_audioInter->SetPort(port->cardId(), port->id(), int(port->direction()));
    qDebug() << "cardID:" << port->cardId()  << "portName:" << port->name() << "  " << port->id() << "  " << port->direction();
    qDebug() << rep.error() << "isError:" << rep.isError();
    m_model->setPort(port);
}

void SoundWorker::setEffectEnable(DDesktopServices::SystemSoundEffect effect, bool enable)
{
    QDBusPendingCall async = m_soundEffectInter->EnableSound(m_model->getNameByEffectType(effect), enable);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);
    watcher->waitForFinished();
}

void SoundWorker::enableAllSoundEffect(bool enable)
{
    m_soundEffectInter->setEnabled(enable);
}

void SoundWorker::setBluetoothMode(const QString &mode)
{
    m_audioInter->SetBluetoothAudioMode(mode).waitForFinished();
}

void SoundWorker::defaultSinkChanged(const QDBusObjectPath &path)
{
    qDebug() << "sink default path:" << path.path();
    if (path.path().isEmpty() || path.path() == "/" )
        return; //路径为空

    if (m_defaultSink)
        m_defaultSink->deleteLater();
    m_defaultSink = new Sink("com.deepin.daemon.Audio", path.path(), QDBusConnection::sessionBus(), this);

    connect(m_defaultSink, &Sink::MuteChanged, [this](bool mute) { m_model->setSpeakerOn(mute);});
    connect(m_defaultSink, &Sink::BalanceChanged, m_model, &SoundModel::setSpeakerBalance);
    connect(m_defaultSink, &Sink::VolumeChanged, m_model, &SoundModel::setSpeakerVolume);
    connect(m_defaultSink, &Sink::ActivePortChanged, this, &SoundWorker::activeSinkPortChanged);
    connect(m_defaultSink, &Sink::CardChanged, this, &SoundWorker::onSinkCardChanged);

    m_model->setSpeakerOn(m_defaultSink->mute());
    m_model->setSpeakerBalance(m_defaultSink->balance());
    m_model->setSpeakerVolume(m_defaultSink->volume());
    m_model->setSpeakerName(m_defaultSink->name());

    activeSinkPortChanged(m_defaultSink->activePort());
    onSinkCardChanged(m_defaultSink->card());
}

void SoundWorker::defaultSourceChanged(const QDBusObjectPath &path)
{
    qDebug() << "source default path:" << path.path();
    if (path.path().isEmpty() || path.path() == "/" ) return; //路径为空

    if (m_defaultSource) m_defaultSource->deleteLater();
    m_defaultSource = new Source("com.deepin.daemon.Audio", path.path(), QDBusConnection::sessionBus(), this);

    connect(m_defaultSource, &Source::MuteChanged, [this](bool mute) { m_model->setMicrophoneOn(mute); });
    connect(m_defaultSource, &Source::VolumeChanged, m_model, &SoundModel::setMicrophoneVolume);
    connect(m_defaultSource, &Source::ActivePortChanged, this, &SoundWorker::activeSourcePortChanged);
    connect(m_defaultSource, &Source::CardChanged, this, &SoundWorker::onSourceCardChanged);

    m_model->setMicrophoneOn(m_defaultSource->mute());
    m_model->setMicrophoneVolume(m_defaultSource->volume());
    activeSourcePortChanged(m_defaultSource->activePort());
    onSourceCardChanged(m_defaultSource->card());
    m_model->setMicrophoneName(m_defaultSource->name());

#ifndef DCC_DISABLE_FEEDBACK
    QDBusPendingCall call = m_defaultSource->GetMeter();
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, [this, call] {
        if (!call.isError()) {
            QDBusReply<QDBusObjectPath> reply = call.reply();
            QDBusObjectPath path = reply.value();

            if (m_sourceMeter) {
                m_sourceMeter->deleteLater();
            }

            m_sourceMeter = new Meter("com.deepin.daemon.Audio", path.path(), QDBusConnection::sessionBus(), this);
            m_sourceMeter->setSync(false);
            connect(m_sourceMeter, &Meter::VolumeChanged, m_model, &SoundModel::setMicrophoneFeedback);
            m_model->setMicrophoneFeedback(m_sourceMeter->volume());
        } else {
            qDebug() << "get meter failed " << call.error().message();
        }
    });
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
    qDebug() << "active sink port changed to: " << activeSinkPort.name;
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
    qDebug() << "active source port changed to: " << activeSourcePort.name;
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

void SoundWorker::onGsettingsChanged(const QString &key)
{
    const QVariant &value = m_dccSettings->get(key);

    if (key == GSETTINGS_WAIT_SOUND_RECEIPT) {
        m_model->setWaitSoundReceiptTime(value.toInt());
    }
}

void SoundWorker::getSoundEnabledMapFinished(QDBusPendingCallWatcher *watcher)
{
    m_model->setEnableSoundEffect(m_soundEffectInter->enabled());

    if (!watcher->isError()) {
        QDBusReply<QMap<QString, bool>> value = watcher->reply();
        auto map = value.value();
        for (auto it = map.constBegin(); it != map.constEnd(); ++it) {
            if (!m_model->checkSEExist(it.key())) continue;

            DDesktopServices::SystemSoundEffect type = m_model->getEffectTypeByGsettingName(it.key());
            m_model->setEffectData(type, it.value());

            QDBusPendingCall call = m_soundEffectInter->GetSoundFile(it.key());
            QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(call, this);
            watcher->setProperty("Type", type);
            connect(watcher, &QDBusPendingCallWatcher::finished, this, &SoundWorker::getSoundPathFinished);
        }
    } else {
        qDebug() << "get sound enabled map error." << watcher->error();
    }

    watcher->deleteLater();
}

void SoundWorker::getSoundPathFinished(QDBusPendingCallWatcher *watcher)
{
    if (!watcher->isError()) {
        QDBusReply<QString> reply = watcher->reply();
        m_model->updateSoundEffectPath(
                    watcher->property("Type").value<DDesktopServices::SystemSoundEffect>(),
                    reply.value());
    } else {
        qDebug() << "get sound path error." << watcher->error();
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

}
}
