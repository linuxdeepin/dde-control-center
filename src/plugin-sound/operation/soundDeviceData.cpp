// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "soundDeviceData.h"

SoundDeviceData::SoundDeviceData()
    : m_ischecked(false)
    , m_name("")
{

}

bool SoundDeviceData::ischecked() const
{
    return m_ischecked;
}

void SoundDeviceData::setIschecked(bool newIschecked)
{
    m_ischecked = newIschecked;
}

QString SoundDeviceData::name() const
{
    return m_name;
}

void SoundDeviceData::setName(const QString &newName)
{
    m_name = newName;
}

QString SoundDeviceData::getPortId() const
{
    return portId;
}

void SoundDeviceData::setPortId(const QString &newPortId)
{
    portId = newPortId;
}

uint SoundDeviceData::getCardId() const
{
    return cardId;
}

void SoundDeviceData::setCardId(uint newCardId)
{
    cardId = newCardId;
}
