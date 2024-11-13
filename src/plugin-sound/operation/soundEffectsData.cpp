// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "soundEffectsData.h"

SoundEffectsData::SoundEffectsData()
    : m_name("")
    , m_dispalyText("")
    , m_aniIconPath("")
{

}

QString SoundEffectsData::dispalyText() const
{
    return m_dispalyText;
}

void SoundEffectsData::setDispalyText(const QString &newDispalyText)
{
    m_dispalyText = newDispalyText;
}

QString SoundEffectsData::name() const
{
    return m_name;
}

void SoundEffectsData::setName(const QString &newName)
{
    m_name = newName;
}

bool SoundEffectsData::checked() const
{
    return m_checked;
}

void SoundEffectsData::setChecked(bool newChecked)
{
    m_checked = newChecked;
}

QString SoundEffectsData::path() const
{
    return m_path;
}

void SoundEffectsData::setPath(const QString &newPath)
{
    m_path = newPath;
}

DDesktopServices::SystemSoundEffect SoundEffectsData::systemSoundEffect() const
{
    return m_systemSoundEffect;
}

void SoundEffectsData::setSystemSoundEffect(DDesktopServices::SystemSoundEffect newSystemSoundEffect)
{
    m_systemSoundEffect = newSystemSoundEffect;
}

QString SoundEffectsData::aniIconPath() const
{
    return m_aniIconPath;
}

void SoundEffectsData::setAniIconPath(const QString &newAniIconPath)
{
    m_aniIconPath = newAniIconPath;
}
