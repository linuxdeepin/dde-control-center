// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "soundInteraction.h"

#include "dccfactory.h"

#include <QDebug>

using namespace  dccV25;

soundInteraction::soundInteraction(QObject *parent)
    : QObject(parent)
    , m_soundModel(new SoundModel(this))
    , m_soundWork(new SoundWorker(m_soundModel,this))
{
    m_soundWork->activate();
}

soundInteraction::~soundInteraction()
{

}

SoundModel *soundInteraction::model() const
{
    return m_soundModel;
}

void soundInteraction::setSoundModel(SoundModel *newSoundModel)
{
    m_soundModel = newSoundModel;
}

SoundWorker *soundInteraction::worker() const
{
    qWarning() <<__FUNCTION__<<__LINE__<<m_soundWork;
    return m_soundWork;
}

void soundInteraction::setSoundWork(SoundWorker *newSoundWork)
{
    m_soundWork = newSoundWork;
}

void soundInteraction::setSinkVolume(double value)
{
    m_soundWork->setSinkVolume(value);
}

DCC_FACTORY_CLASS(soundInteraction)

#include "soundInteraction.moc"
