/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundworker.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>

namespace dcc {
namespace sound {

SoundWorker::SoundWorker(SoundModel *model, QObject * parent) :
    QObject(parent),
    m_model(model),
    m_activeOutputCard(UINT_MAX),
    m_activeInputCard(UINT_MAX),
    m_audioInter(new Audio("com.deepin.daemon.Audio", "/com/deepin/daemon/Audio", QDBusConnection::sessionBus(), this)),
    m_soundEffectInter(new SoundEffect("com.deepin.daemon.SoundEffect", "/com/deepin/daemon/SoundEffect", QDBusConnection::sessionBus(), this)),
    m_defaultSink(nullptr),
    m_defaultSource(nullptr),
    m_sourceMeter(nullptr),
    m_pingTimer(new QTimer(this))
{
    m_audioInter->setSync(false);
    //    m_soundEffectInter->setSync(false);

    m_pingTimer->setInterval(500);
    m_pingTimer->setSingleShot(false);

    connect(m_audioInter, &Audio::DefaultSinkChanged, this, &SoundWorker::defaultSinkChanged);
    connect(m_audioInter, &Audio::DefaultSourceChanged, this, &SoundWorker::defaultSourceChanged);
//    connect(m_audioInter, &Audio::SinksChanged, this, &SoundWorker::sinksChanged);
//    connect(m_audioInter, &Audio::SourcesChanged, this, &SoundWorker::sourcesChanged);
    connect(m_audioInter, &Audio::CardsChanged, this, &SoundWorker::cardsChanged);

    connect(m_pingTimer, &QTimer::timeout, [this] { if (m_sourceMeter) m_sourceMeter->Tick(); });
}

void SoundWorker::activate()
{
    m_pingTimer->start();

    m_audioInter->blockSignals(false);
    m_soundEffectInter->blockSignals(false);
    if (m_defaultSink) m_defaultSink->blockSignals(false);
    if (m_defaultSource) m_defaultSource->blockSignals(false);
    if (m_sourceMeter) m_sourceMeter->blockSignals(false);

//    sinksChanged(m_audioInter->sinks());
//    sourcesChanged(m_audioInter->sources());
    defaultSinkChanged(m_audioInter->defaultSink());
    defaultSourceChanged(m_audioInter->defaultSource());
    cardsChanged(m_audioInter->cards());

    m_model->setSoundEffectOn(m_soundEffectInter->enabled());
    connect(m_soundEffectInter, &SoundEffect::EnabledChanged, m_model, &SoundModel::setSoundEffectOn);
}

void SoundWorker::deactivate()
{
    m_pingTimer->stop();

    m_audioInter->blockSignals(true);
    m_soundEffectInter->blockSignals(true);
    if (m_defaultSink) m_defaultSink->blockSignals(true);
    if (m_defaultSource) m_defaultSource->blockSignals(true);
    if (m_sourceMeter) m_sourceMeter->blockSignals(true);
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

void SoundWorker::switchSoundEffect(bool on)
{
    m_soundEffectInter->setEnabled(on);
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

void SoundWorker::setPort(const Port *port)
{
    m_audioInter->SetPort(port->cardId(), port->id(), int(port->direction()));
}

void SoundWorker::defaultSinkChanged(const QDBusObjectPath &path)
{
    if (path.path().isEmpty()) return;

    if (m_defaultSink) m_defaultSink->deleteLater();
    m_defaultSink = new Sink("com.deepin.daemon.Audio", path.path(), QDBusConnection::sessionBus(), this);

    connect(m_defaultSink, &Sink::MuteChanged, [this] (bool mute) { m_model->setSpeakerOn(!mute); });
    connect(m_defaultSink, &Sink::BalanceChanged, m_model, &SoundModel::setSpeakerBalance);
    connect(m_defaultSink, &Sink::VolumeChanged, m_model, &SoundModel::setSpeakerVolume);
    connect(m_defaultSink, &Sink::ActivePortChanged, this, &SoundWorker::activeSinkPortChanged);
    connect(m_defaultSink, &Sink::CardChanged, this, &SoundWorker::onSinkCardChanged);

    m_model->setSpeakerOn(!m_defaultSink->mute());
    m_model->setSpeakerBalance(m_defaultSink->balance());
    m_model->setSpeakerVolume(m_defaultSink->volume());
    activeSinkPortChanged(m_defaultSink->activePort());
    onSinkCardChanged(m_defaultSink->card());
}

void SoundWorker::defaultSourceChanged(const QDBusObjectPath &path)
{
    if (path.path().isEmpty()) return;

    if (m_defaultSource) m_defaultSource->deleteLater();
    m_defaultSource = new Source("com.deepin.daemon.Audio", path.path(), QDBusConnection::sessionBus(), this);

    connect(m_defaultSource, &Source::MuteChanged, [this] (bool mute) { m_model->setMicrophoneOn(!mute); });
    connect(m_defaultSource, &Source::VolumeChanged, m_model, &SoundModel::setMicrophoneVolume);
    connect(m_defaultSource, &Source::ActivePortChanged, this, &SoundWorker::activeSourcePortChanged);
    connect(m_defaultSource, &Source::CardChanged, this, &SoundWorker::onSourceCardChanged);

    m_model->setMicrophoneOn(!m_defaultSource->mute());
    m_model->setMicrophoneVolume(m_defaultSource->volume());
    activeSourcePortChanged(m_defaultSource->activePort());
    onSourceCardChanged(m_defaultSource->card());

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
            qWarning() << "get meter failed " << call.error().message();
        }
    });
#endif
}

//void SoundWorker::sinksChanged(const QList<QDBusObjectPath> &value)
//{
//    // TODO: reuse all the sink objects.
//    for (QDBusObjectPath path : value) {
//        Sink *sink = new Sink("com.deepin.daemon.Audio", path.path(), QDBusConnection::sessionBus(), this);
//        if (sink->isValid()) {
//            m_sinks << sink;
//        }
//    }
//}

//void SoundWorker::sourcesChanged(const QList<QDBusObjectPath> &value)
//{
//    // TODO: reuse all the source objects.
//    for (QDBusObjectPath path : value) {
//        Source *source = new Source("com.deepin.daemon.Audio", path.path(), QDBusConnection::sessionBus(), this);
//        if (source->isValid()) {
//            m_sources << source;
//        }
//    }
//}

void SoundWorker::cardsChanged(const QString &cards)
{
    QMap<uint, QStringList> tmpCardIds;

    QJsonDocument doc = QJsonDocument::fromJson(cards.toUtf8());
    QJsonArray jCards = doc.array();
    for (QJsonValue cV : jCards) {
        QJsonObject jCard = cV.toObject();
        const uint cardId = jCard["Id"].toInt();
        const QString cardName = jCard["Name"].toString();
        QJsonArray jPorts = jCard["Ports"].toArray();

        QStringList tmpPorts;

        for (QJsonValue pV : jPorts) {
            QJsonObject jPort = pV.toObject();
            // const double portAvai = jPort["Available"].toDouble();
            // if (portAvai == 2 || portAvai == 0 ||) { // 0 Unknow 1 Not availabel 2 Available
            const QString portId = jPort["Name"].toString();
            const QString portName = jPort["Description"].toString();

            Port *port = m_model->findPort(portId, cardId);
            const bool include = port != nullptr;
            if (!include) { port = new Port(m_model); }

            port->setId(portId);
            port->setName(portName);
            port->setDirection(Port::Direction(jPort["Direction"].toDouble()));
            port->setCardId(cardId);
            port->setCardName(cardName);
            port->setIsActive(portId == m_activeSinkPort || portId == m_activeSourcePort);

            if (!include) { m_model->addPort(port); }

            tmpPorts << portId;
            // }
        }
        tmpCardIds.insert(cardId, tmpPorts);
    }

    for (Port *port : m_model->ports()) {
        //if the card is not in the list
        if (!tmpCardIds.contains(port->cardId())) {
            m_model->removePort(port->id(), port->cardId());
        }
    }
}

void SoundWorker::activeSinkPortChanged(const AudioPort &activeSinkPort)
{
    qDebug() << "active sink port changed to: " << activeSinkPort.name;
    m_activeSinkPort = activeSinkPort.name;

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

void SoundWorker::updatePortActivity()
{
    for (Port *port : m_model->ports()) {
        const bool isActiveOuputPort = port->id() == m_activeSinkPort && port->cardId() == m_activeOutputCard;
        const bool isActiveInputPort = port->id() == m_activeSourcePort && port->cardId() == m_activeInputCard;

        port->setIsActive(isActiveInputPort || isActiveOuputPort);
    }
}

}
}
