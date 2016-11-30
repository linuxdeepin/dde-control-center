/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "soundmodel.h"

namespace dcc {
namespace sound {

SoundModel::SoundModel(QObject *parent)
    : QObject(parent),
      m_speakerOn(true),
      m_microphoneOn(true),
      m_soundEffectOn(true),
      m_speakerBalance(0)
{

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

void SoundModel::setMicrophoneFeedback(double microphoneFeedback)
{
    if (microphoneFeedback != m_microphoneFeedback) {
        m_microphoneFeedback = microphoneFeedback;

        emit microphoneFeedbackChanged(microphoneFeedback);
    }
}

} // namespace sound
} // namespace dcc
