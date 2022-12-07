// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDWORKER_H
#define SOUNDWORKER_H

#include <QObject>

#include <com_deepin_daemon_audio.h>
#include <com_deepin_daemon_audio_sink.h>
#include <com_deepin_daemon_audio_source.h>
#include <com_deepin_daemon_audio_meter.h>
#include <com_deepin_daemon_soundeffect.h>
#include <com_deepin_system_systempower.h>

#include "modules/moduleworker.h"
#include "soundmodel.h"

#include <DDesktopServices>

DWIDGET_USE_NAMESPACE

using com::deepin::daemon::Audio;
using com::deepin::daemon::audio::Sink;
using com::deepin::daemon::audio::Source;
using com::deepin::daemon::audio::Meter;
using com::deepin::daemon::SoundEffect;
using SystemPowerInter = com::deepin::system::Power;
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

    void refreshSoundEffect();

public Q_SLOTS:
    void switchSpeaker(bool on);
    void switchMicrophone(bool on);

    void setPortEnabled(unsigned int cardid,QString portName,bool enable);
    void setSinkBalance(double balance);
    void setSourceVolume(double volume);
    void setSinkVolume(double volume);
    void setSourceMute(bool flag = true);
    void setSinkMute(bool flag = true);
    void setIncreaseVolume(bool value);
    void setReduceNoise(bool value);

    void setPort(const Port *port);
    void setEffectEnable(DDesktopServices::SystemSoundEffect effect, bool enable);
    void enableAllSoundEffect(bool enable);
    void setBluetoothMode(const QString &mode);

private Q_SLOTS:
    void defaultSinkChanged(const QDBusObjectPath &path);
    void defaultSourceChanged(const QDBusObjectPath &path);
    void cardsChanged(const QString &cards);
    void sourcesChanged(const QList<QDBusObjectPath> &sources);

    void activeSinkPortChanged(const AudioPort &activeSinkPort);
    void activeSourcePortChanged(const AudioPort &activeSourcePort);

    void onSinkCardChanged(const uint &cardId);
    void onSourceCardChanged(const uint &cardId);
    void onGsettingsChanged(const QString &key);
    void getSoundEnabledMapFinished(QDBusPendingCallWatcher *watcher);
    void getSoundPathFinished(QDBusPendingCallWatcher *watcher);

private:
    void initConnect();
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
    QPointer<Meter> m_defaultSourceMeter;
    QList<Sink*> m_sinks;
    QList<Source*> m_sources;
    SystemPowerInter *m_powerInter;
    QGSettings *m_dccSettings;
    QMap<QDBusObjectPath, QPointer<Meter> > m_sourceMeterMap;

    QTimer *m_pingTimer;
    QDBusConnectionInterface *m_inter;
    int m_waitSoundPortReceipt;
};

}
}

// should include 2.0.9.17
#if DTK_VERSION < DTK_VERSION_CHECK(2, 0, 9, 17)
    Q_DECLARE_METATYPE(DDesktopServices::SystemSoundEffect)
#endif

#endif // SOUNDWORKER_H
