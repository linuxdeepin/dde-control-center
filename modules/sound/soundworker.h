/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDWORKER_H
#define SOUNDWORKER_H

#include <QObject>

#include <com_deepin_daemon_audio.h>
#include <com_deepin_daemon_audio_sink.h>
#include <com_deepin_daemon_audio_source.h>
#include <com_deepin_daemon_audio_meter.h>
#include <com_deepin_daemon_soundeffect.h>

#include "moduleworker.h"
#include "soundmodel.h"

using com::deepin::daemon::Audio;
using com::deepin::daemon::audio::Sink;
using com::deepin::daemon::audio::Source;
using com::deepin::daemon::audio::Meter;
using com::deepin::daemon::SoundEffect;

namespace dcc {
namespace sound {

class SoundWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    explicit SoundWorker(SoundModel *model, QObject * parent = 0);

    void activate() Q_DECL_OVERRIDE;
    void deactivate() Q_DECL_OVERRIDE;

public slots:
    void switchSpeaker(bool on);
    void switchMicrophone(bool on);
    void switchSoundEffect(bool on);

    void setSinkBalance(double balance);
    void setSourceVolume(double volume);
    void setSinkVolume(double volume);

    void setPort(const Port *port);

private slots:
    void defaultSinkChanged(const QDBusObjectPath &path);
    void defaultSourceChanged(const QDBusObjectPath &path);
//    void sinksChanged(const QList<QDBusObjectPath> & value);
//    void sourcesChanged(const QList<QDBusObjectPath> & value);
    void cardsChanged(const QString &cards);

    void activeSinkPortChanged(const AudioPort &activeSinkPort);
    void activeSourcePortChanged(const AudioPort &activeSourcePort);

    void onSinkCardChanged(const uint &cardId);
    void onSourceCardChanged(const uint &cardId);

private:
    void updatePortActivity();

private:
    SoundModel *m_model;
    QString m_activeSinkPort;
    QString m_activeSourcePort;
    uint m_activeOutputCard;
    uint m_activeInputCard;

    Audio *m_audioInter;
    SoundEffect *m_soundEffectInter;
    QPointer<Sink> m_defaultSink;
    QPointer<Source> m_defaultSource;
    QPointer<Meter> m_sourceMeter;
    QList<Sink*> m_sinks;
    QList<Source*> m_sources;

    QTimer *m_pingTimer;
};

}
}

#endif // SOUNDWORKER_H
