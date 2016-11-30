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

private slots:
    void defaultSinkChanged(const QString &id);
    void defaultSourceChanged(const QString &id);
    void sinksChanged(const QList<QDBusObjectPath> & value);
    void sourcesChanged(const QList<QDBusObjectPath> & value);

private:
    SoundModel *m_model;
    Audio *m_audioInter;
    SoundEffect *m_soundEffectInter;
    Sink *m_defaultSink;
    Source *m_defaultSource;
    Meter *m_sourceMeter;
    QList<Sink*> m_sinks;
    QList<Source*> m_sources;

    QTimer *m_pingTimer;
};

}
}

#endif // SOUNDWORKER_H
