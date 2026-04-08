// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "soundInteraction.h"

#include "dccfactory.h"

#include <QDebug>
#include <QtQml/qqml.h>

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

DCC_FACTORY_CLASS(soundInteraction,\
                  qmlRegisterType<SoundWorker>("dcc", 1, 0, "SoundWorker"),\
                  qmlRegisterType<SoundModel>("dcc", 1, 0, "SoundModel"),\
                  qmlRegisterType<SoundDeviceModel>("SoundDeviceModel", 1, 0, "SoundDeviceModel"))

#include "soundInteraction.moc"
