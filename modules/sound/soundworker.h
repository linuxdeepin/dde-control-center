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

class QGSettings;

namespace dcc {
namespace sound {

class SoundWorker : public QObject, public ModuleWorker
{
    Q_OBJECT
public:
    explicit SoundWorker(SoundModel *model, QObject * parent = 0);

    void activate() Q_DECL_OVERRIDE;
    void deactivate() Q_DECL_OVERRIDE;

public Q_SLOTS:
    void switchSpeaker(bool on);
    void switchMicrophone(bool on);

    void setSinkBalance(double balance);
    void setSourceVolume(double volume);
    void setSinkVolume(double volume);

    void setPort(const Port *port);
    void querySoundEffectData(const QString &name);
    void setEffectEnable(const QString &name, bool enable);
    void playSoundEffect(const QString &name);

private Q_SLOTS:
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
    QPointer<Sink> m_defaultSink;
    QPointer<Source> m_defaultSource;
    QPointer<Meter> m_sourceMeter;
    QList<Sink*> m_sinks;
    QList<Source*> m_sources;
    QGSettings *m_effectGsettings;

    QTimer *m_pingTimer;
    QTimer *m_activeTimer;
};

}
}

#endif // SOUNDWORKER_H
