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

#include "soundmodel.h"

#include <QDebug>

static const QMap<DDesktopServices::SystemSoundEffect, QString> SOUND_EFFECT_MAP{
    { DDesktopServices::SystemSoundEffect::SSE_Notifications, "message" },
    { DDesktopServices::SystemSoundEffect::SEE_Screenshot, "camera-shutter" },
    { DDesktopServices::SystemSoundEffect::SSE_EmptyTrash, "trash-empty" },
    { DDesktopServices::SystemSoundEffect::SSE_SendFileComplete, "x-deepin-app-sent-to-desktop" },
    { DDesktopServices::SystemSoundEffect::SSE_BootUp, "desktop-login" },
    { DDesktopServices::SystemSoundEffect::SSE_Shutdown, "system-shutdown" },
    { DDesktopServices::SystemSoundEffect::SSE_Logout, "desktop-logout" },
    { DDesktopServices::SystemSoundEffect::SSE_WakeUp, "suspend-resume" },
    { DDesktopServices::SystemSoundEffect::SSE_VolumeChange, "audio-volume-change" },
    { DDesktopServices::SystemSoundEffect::SSE_LowBattery, "power-unplug-battery-low" },
    { DDesktopServices::SystemSoundEffect::SSE_PlugIn, "power-plug" },
    { DDesktopServices::SystemSoundEffect::SSE_PlugOut, "power-unplug" },
    { DDesktopServices::SystemSoundEffect::SSE_DeviceAdded, "device-added" },
    { DDesktopServices::SystemSoundEffect::SSE_DeviceRemoved, "device-removed" },
    { DDesktopServices::SystemSoundEffect::SSE_Error, "dialog-error" }
};

namespace dcc {
namespace sound {

SoundModel::SoundModel(QObject *parent)
    : QObject(parent)
    , m_speakerOn(true)
    , m_microphoneOn(true)
    , m_speakerVolume(75)
    , m_speakerBalance(0)
    , m_microphoneVolume(75)
#ifndef DCC_DISABLE_FEEDBACK
    , m_microphoneFeedback(50)
#endif
{
    m_soundEffectMap = {
        { "Boot up", DDesktopServices::SSE_BootUp },
        { "Shut down", DDesktopServices::SSE_Shutdown },
        { "Log out", DDesktopServices::SSE_Logout },
        { "Wake up", DDesktopServices::SSE_WakeUp },
        { "Volume +/-", DDesktopServices::SSE_VolumeChange },
        { "Notification", DDesktopServices::SSE_Notifications },
        { "Low battery", DDesktopServices::SSE_LowBattery },
        { "Send icon in Launcher to Desktop", DDesktopServices::SSE_SendFileComplete },
        { "Empty Trash", DDesktopServices::SSE_EmptyTrash },
        { "Plug in", DDesktopServices::SSE_PlugIn },
        { "Plug out", DDesktopServices::SSE_PlugOut },
        { "Removable device connected", DDesktopServices::SSE_DeviceAdded },
        { "Removable device removed", DDesktopServices::SSE_DeviceRemoved },
        { "Error", DDesktopServices::SSE_Error },
    };
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

        Q_EMIT speakerOnChanged(speakerOn);
    }
}

void SoundModel::setMicrophoneOn(bool microphoneOn)
{
    if (microphoneOn != m_microphoneOn) {
        m_microphoneOn = microphoneOn;

        Q_EMIT microphoneOnChanged(microphoneOn);
    }
}

void SoundModel::setSpeakerBalance(double speakerBalance)
{
    if (speakerBalance != m_speakerBalance) {
        m_speakerBalance = speakerBalance;

        Q_EMIT speakerBalanceChanged(speakerBalance);
    }
}

void SoundModel::setMicrophoneVolume(double microphoneVolume)
{
    if (microphoneVolume != m_microphoneVolume) {
        m_microphoneVolume = microphoneVolume;

        Q_EMIT microphoneVolumeChanged(microphoneVolume);
    }
}
#ifndef DCC_DISABLE_FEEDBACK
void SoundModel::setMicrophoneFeedback(double microphoneFeedback)
{
    if (microphoneFeedback != m_microphoneFeedback) {
        m_microphoneFeedback = microphoneFeedback;

        Q_EMIT microphoneFeedbackChanged(microphoneFeedback);
    }
}
#endif

void SoundModel::addPort(Port *port)
{
    if (!containsPort(port)) {
        m_ports.append(port);
        Q_EMIT portAdded(port);
    }
}

void SoundModel::removePort(const QString &portId, const uint &cardId)
{
    Port *port = findPort(portId, cardId);
    if (port) {
        m_ports.removeOne(port);
        port->deleteLater();
        Q_EMIT portRemoved(portId, cardId);
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
        Q_EMIT speakerVolumeChanged(speakerVolume);
    }
}

QDBusObjectPath SoundModel::defaultSource() const
{
    return m_defaultSource;
}

void SoundModel::setDefaultSource(const QDBusObjectPath &defaultSource)
{
    m_defaultSource = defaultSource;

    Q_EMIT defaultSourceChanged(m_defaultSource);
}

QDBusObjectPath SoundModel::defaultSink() const
{
    return m_defaultSink;
}

void SoundModel::setDefaultSink(const QDBusObjectPath &defaultSink)
{
    m_defaultSink = defaultSink;

    Q_EMIT defaultSinkChanged(m_defaultSink);
}

QString SoundModel::audioCards() const
{
    return m_audioCards;
}

void SoundModel::setAudioCards(const QString &audioCards)
{
    m_audioCards = audioCards;

    Q_EMIT audioCardsChanged(m_audioCards);
}

void SoundModel::setEffectData(DDesktopServices::SystemSoundEffect effect, const bool enable)
{
    m_soundEffectData[effect] = enable;

    Q_EMIT soundEffectDataChanged(effect, enable);
}

bool SoundModel::queryEffectData(DDesktopServices::SystemSoundEffect effect)
{
    return m_soundEffectData[effect];
}

void SoundModel::setEnableSoundEffect(bool enableSoundEffect)
{
    if (m_enableSoundEffect == enableSoundEffect) return;

    m_enableSoundEffect = enableSoundEffect;

    Q_EMIT enableSoundEffectChanged(enableSoundEffect);
}

void SoundModel::updateSoundEffectPath(DDesktopServices::SystemSoundEffect effect, const QString &path)
{
    m_soundEffectPaths[effect] = path;
}

const QString SoundModel::soundEffectPathByType(DDesktopServices::SystemSoundEffect effect)
{
    return m_soundEffectPaths[effect];
}

const QString SoundModel::getNameByEffectType(DDesktopServices::SystemSoundEffect effect) const
{
    return SOUND_EFFECT_MAP.value(effect);
}

DDesktopServices::SystemSoundEffect SoundModel::getEffectTypeByGsettingName(const QString &name) {
    return SOUND_EFFECT_MAP.key(name);
}

bool SoundModel::checkSEExist(const QString &name)
{
    return SOUND_EFFECT_MAP.values().contains(name);
}

void SoundModel::setIsLaptop(bool isLaptop) {
    if (isLaptop == m_isLaptop) return;

    m_isLaptop = isLaptop;

    Q_EMIT isLaptopChanged(isLaptop);
}

void Port::setId(const QString &id)
{
    if (id != m_id) {
        m_id = id;
        Q_EMIT idChanged(id);
    }
}

void Port::setName(const QString &name)
{
    if (name != m_name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Port::setCardName(const QString &cardName)
{
    if (cardName != m_cardName) {
        m_cardName = cardName;
        Q_EMIT cardNameChanged(cardName);
    }
}

void Port::setIsActive(bool isActive)
{
    if (isActive != m_isActive) {
        m_isActive = isActive;
        Q_EMIT isActiveChanged(isActive);
    }
}

void Port::setDirection(const Direction &direction)
{
    if (direction != m_direction) {
        m_direction = direction;
        Q_EMIT directionChanged(direction);
    }
}

void Port::setCardId(const uint &cardId)
{
    if (cardId != m_cardId) {
        m_cardId = cardId;
        Q_EMIT cardIdChanged(cardId);
    }
}

} // namespace sound
} // namespace dcc
