/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef DCC_SOUND_SOUNDMODEL_H
#define DCC_SOUND_SOUNDMODEL_H

#include <QObject>

namespace dcc {
namespace sound {

class SoundModel : public QObject
{
    Q_OBJECT
public:
    explicit SoundModel(QObject *parent = 0);

    inline bool speakerOn() const { return m_speakerOn; }
    void setSpeakerOn(bool speakerOn);

    inline bool microphoneOn() const { return m_microphoneOn; }
    void setMicrophoneOn(bool microphoneOn);

    inline bool soundEffectOn() const { return m_soundEffectOn; }
    void setSoundEffectOn(bool soundEffectOn);

    inline double speakerBalance() const { return m_speakerBalance; }
    void setSpeakerBalance(double speakerBalance);

    inline double microphoneVolume() const { return m_microphoneVolume; }
    void setMicrophoneVolume(double microphoneVolume);

    inline double microphoneFeedback() const { return m_microphoneFeedback; }
    void setMicrophoneFeedback(double microphoneFeedback);

signals:
    void speakerOnChanged(bool speakerOn) const;
    void microphoneOnChanged(bool microphoneOn) const;
    void soundEffectOnChanged(bool soundEffectOn) const;
    void speakerBalanceChanged(double speakerBalance) const;
    void microphoneVolumeChanged(double microphoneVolume) const;
    void microphoneFeedbackChanged(double microphoneFeedback) const;

private:
    bool m_speakerOn;
    bool m_microphoneOn;
    bool m_soundEffectOn;
    double m_speakerBalance;
    double m_microphoneVolume;
    double m_microphoneFeedback;
};

} // namespace sound
} // namespace dcc

#endif // DCC_SOUND_SOUNDMODEL_H
