// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "soundmodel.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>

SoundModel::SoundModel()
    : m_volume(0)
    , m_mute(false)
    , m_maxVolumeUI(100)
{

}

SoundModel::~SoundModel()
{

}

void SoundModel::setVolume(double volume)
{
    int tmp = qRound(volume * 100.0f);
    if (m_volume != tmp) {
        m_volume = tmp;
        Q_EMIT volumeChanged(m_volume);
    }
}

void SoundModel::setMute(bool mute)
{
    if (m_mute != mute) {
        m_mute = mute;
        Q_EMIT muteStateChanged(m_mute);
    }
}

void SoundModel::setCardsInfo(const QString &info)
{
    if (m_cardsInfo != info) {
        m_cardsInfo = info;
        analysisCardsInfo();
        Q_EMIT cardsInfoChanged();
    }
}

void SoundModel::setActivePort(uint cardId, const QString &portName)
{
    const QString& activePortKey = SoundCardPort::compositeKey(cardId, portName);
    SoundCardPort* activePort = nullptr;
    for (auto port : m_ports.values()) {
        if (port->uniqueKey() == activePortKey) {
            port->setActive(true);
            activePort = port;
        } else {
            port->setActive(false);
        }
    }
    Q_EMIT activePortChanged(activePort);
}

SoundCardPort* SoundModel::activePort() const
{
    for (const auto port : m_ports.values()) {
        if (port->isActive())
            return port;
    }
    return nullptr;
}

void SoundModel::setMaxVolumeUI(double volume)
{
    int tmp = qRound(volume * 100.0f);
    if (tmp != m_maxVolumeUI ) {
        m_maxVolumeUI = tmp;
        Q_EMIT maxVolumeUIChanged(m_maxVolumeUI);
    }
}

void SoundModel::analysisCardsInfo()
{
    QStringList tmpPorts;
    const auto &cardsStr = m_cardsInfo;
    QJsonDocument doc = QJsonDocument::fromJson(cardsStr.toUtf8());
    QJsonArray cardArray = doc.array();
    for (QJsonValue cV : cardArray) {
        QJsonObject jCard = cV.toObject();
        const uint cardId = static_cast<uint>(jCard["Id"].toDouble());
        const QString cardName = jCard["Name"].toString();
        QJsonArray jPorts = jCard["Ports"].toArray();
        for (QJsonValue pV : jPorts) {
            QJsonObject jPort = pV.toObject();
            const double portAvailable = jPort["Available"].toDouble();
            const auto direction =SoundCardPort::Direction(jPort["Direction"].toDouble());
            if ((portAvailable == 2 || portAvailable == 0) && direction == SoundCardPort::Out) { // 0 Unknow 1 Not available 2 Available
                const QString &portName = jPort["Name"].toString();
                const QString &key = SoundCardPort::compositeKey(cardId, portName);

                SoundCardPort* port = m_ports.value(key, new SoundCardPort(this));
                port->setPortName(portName);
                port->setDescription(jPort["Description"].toString());
                port->setDirection(direction);
                port->setCardId(cardId);
                port->setCardName(cardName);
                const bool oldEnabled = port->isEnabled();
                port->setEnabled(jPort["Enabled"].toBool());
                port->setBluetooth(jPort["Bluetooth"].toBool());
                port->setPortType(jPort["PortType"].toInt());
                if (!m_ports.contains(key)) {
                    m_ports.insert(key, port);
                    if (port->isEnabled()) {
                        Q_EMIT enabledPortAdded(port);
                    }
                } else if (!oldEnabled && port->isEnabled()){
                    Q_EMIT enabledPortAdded(port);
                }

                tmpPorts << key;
            }
        }
    }

    for (SoundCardPort* port : m_ports.values()) {
        // 判断端口是否在最新的设备列表中
        const auto &key = port->uniqueKey();
        if (!tmpPorts.contains(key)) {
            Q_EMIT portRemoved(key);
            m_ports.remove(key);
            port->deleteLater();
        }
    }
}

bool SoundModel::existActiveOutputDevice() const
{
    for (const auto port : m_ports.values()) {
        if (port->direction() == SoundCardPort::Out && port->isEnabled()) {
            return true;
        }
    }

    return false;
}
