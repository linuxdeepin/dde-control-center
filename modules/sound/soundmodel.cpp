/*
 * Copyright (C) 2016 ~ 2017 Deepin Technology Co., Ltd.
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

#include "soundmodel.h"

#include <QDebug>

namespace dcc {
namespace sound {

SoundModel::SoundModel(QObject *parent)
    : QObject(parent)
    , m_speakerOn(true)
    , m_microphoneOn(true)
    , m_soundEffectOn(true)
    , m_speakerVolume(75)
    , m_speakerBalance(0)
    , m_microphoneVolume(75)
#ifndef DCC_DISABLE_FEEDBACK
    , m_microphoneFeedback(50)
#endif
{

}

SoundModel::~SoundModel()
{
    for (Port *port : m_ports) {
        if (port)
            port->deleteLater();
    }
}

void SoundModel::setSpeakerOn(bool speakerOn)
{
    if (speakerOn != m_speakerOn) {
        m_speakerOn = speakerOn;

        emit speakerOnChanged(speakerOn);
    }
}

void SoundModel::setMicrophoneOn(bool microphoneOn)
{
    if (microphoneOn != m_microphoneOn) {
        m_microphoneOn = microphoneOn;

        emit microphoneOnChanged(microphoneOn);
    }
}

void SoundModel::setSoundEffectOn(bool soundEffectOn)
{
    if (soundEffectOn != m_soundEffectOn) {
        m_soundEffectOn = soundEffectOn;

        emit soundEffectOnChanged(soundEffectOn);
    }
}

void SoundModel::setSpeakerBalance(double speakerBalance)
{
    if (speakerBalance != m_speakerBalance) {
        m_speakerBalance = speakerBalance;

        emit speakerBalanceChanged(speakerBalance);
    }
}

void SoundModel::setMicrophoneVolume(double microphoneVolume)
{
    if (microphoneVolume != m_microphoneVolume) {
        m_microphoneVolume = microphoneVolume;

        emit microphoneVolumeChanged(microphoneVolume);
    }
}
#ifndef DCC_DISABLE_FEEDBACK
void SoundModel::setMicrophoneFeedback(double microphoneFeedback)
{
    if (microphoneFeedback != m_microphoneFeedback) {
        m_microphoneFeedback = microphoneFeedback;

        emit microphoneFeedbackChanged(microphoneFeedback);
    }
}
#endif

void SoundModel::addPort(Port *port)
{
    qDebug() << "add port: " << port->id();
    if (!containsPort(port)) {
        m_ports.append(port);
        emit portAdded(port);
    }
}

void SoundModel::removePort(const QString &portId, const uint &cardId)
{
    qDebug() << "remove port: " << portId;
    Port *port = findPort(portId, cardId);
    if (port) {
        m_ports.removeOne(port);
        port->deleteLater();
        emit portRemoved(portId, cardId);
    }
}

bool SoundModel::containsPort(const Port *port)
{
    return findPort(port->id(), port->cardId()) != nullptr;
}

Port *SoundModel::findPort(const QString &portId, const uint &cardId) const
{
    for (Port *port : m_ports) {
        if (port->id() == portId && port->cardId() == cardId) {
            return port;
        }
    }

    return nullptr;
}

QList<Port *> SoundModel::ports() const
{
    return m_ports;
}

void SoundModel::setSpeakerVolume(double speakerVolume)
{
    if (m_speakerVolume != speakerVolume)  {
        m_speakerVolume = speakerVolume;
        emit speakerVolumeChanged(speakerVolume);
    }
}

void Port::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        emit idChanged(id);
    }
}

void Port::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        emit nameChanged(name);
    }
}

void Port::setCardName(const QString &cardName)
{
    if (cardName != m_cardName) {
        m_cardName = cardName;
        emit cardNameChanged(cardName);
    }
}

void Port::setIsActive(bool isActive)
{
    if (isActive != m_isActive) {
        m_isActive = isActive;
        emit isActiveChanged(isActive);
    }
}

void Port::setDirection(const Direction &direction)
{
    if (direction != m_direction) {
        m_direction = direction;
        emit directionChanged(direction);
    }
}

void Port::setCardId(const uint &cardId)
{
    if (cardId != m_cardId) {
        m_cardId = cardId;
        emit cardIdChanged(cardId);
    }
}

} // namespace sound
} // namespace dcc
