// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDWORKER_H
#define SOUNDWORKER_H

#include "audioport.h"
#include "qdbusconnectioninterface.h"
#include "sounddbusproxy.h"
#include "soundmodel.h"

#include <DDesktopServices>

#include <QHash>
#include <QMediaDevices>
#include <QObject>
#include <QPointer>
#include <QSoundEffect>
#include <QTimer>
#include <QtQml/qqml.h>

class SoundWorker : public QObject
{
    Q_OBJECT

    QML_NAMED_ELEMENT(SoundWorker)
    QML_SINGLETON
public:
    explicit SoundWorker(SoundModel *model, QObject *parent = 0);
    ~SoundWorker() override;

    void activate();
    void deactivate();

    Q_INVOKABLE void setSinkVolume(double volume);
    Q_INVOKABLE void setReduceNoise(bool value);
    Q_INVOKABLE void setPausePlayer(bool value);
    Q_INVOKABLE void setIncreaseVolume(bool value);
    Q_INVOKABLE void setSinkBalance(double balance);
    Q_INVOKABLE void setActivePort(int index, int portType);
    Q_INVOKABLE void setSoundEffectEnable(int index, bool enable);
    Q_INVOKABLE void setSourceVolume(double volume);
    Q_INVOKABLE void enableAllSoundEffect(bool enable);
    Q_INVOKABLE void setPortEnableIndex(int index, bool checked, int portType);
    Q_INVOKABLE void playSoundEffect(int index);
    Q_INVOKABLE void stopSoundEffectPlayback();
    Q_INVOKABLE void setAudioServerIndex(int index);
    Q_INVOKABLE void setAudioMono(bool enable);
    Q_INVOKABLE void setMicrophoneFeedbackEnabled(QObject *consumer, bool enabled);

public Q_SLOTS:
    void switchSpeaker(bool on);
    void switchMicrophone(bool on);

    void setPortEnabled(unsigned int cardid,QString portName,bool enable);
    void setSourceMute(bool flag = true);
    void setSinkMute(bool flag = true);

    void setPort(Port *port);
    void setEffectEnable(DDesktopServices::SystemSoundEffect effect, bool enable);
    void setBluetoothMode(const QString &mode);
    void refreshSoundEffect();

    void setAudioServer(const QString &value);

private Q_SLOTS:
    void defaultSinkChanged(const QDBusObjectPath &path);
    void defaultSourceChanged(const QDBusObjectPath &path);
    void cardsChanged(const QString &cards);

    void activeSinkPortChanged(const AudioPort &activeSinkPort);
    void activeSourcePortChanged(const AudioPort &activeSourcePort);

    void onSinkCardChanged(const uint &cardId);
    void onSourceCardChanged(const uint &cardId);

    void getSoundEnabledMapFinished(QMap<QString, bool> map);
    void getSoundPathFinished(QDBusPendingCallWatcher *watcher);

    void onAniTimerTimeOut();
    void onSoundPlayingChanged();
    void updatePlayAniIconPath();
    void changeOutputDeviceComboxStatus();
    void changeInputDeviceComboxStatus();

private:
    void initConnect();
    void updatePortActivity();
    void initAudioServerData();
#ifndef DCC_DISABLE_FEEDBACK
    void ensureMicrophoneMeter();
    void stopMicrophoneMeter();
    void removeFeedbackConsumer(QObject *consumer);
#endif

private:
    QPointer<SoundModel> m_model;
    QString m_activeSinkPort;
    QString m_activeSourcePort;
    uint m_activeOutputCard;
    uint m_activeInputCard;

    SoundDBusProxy *m_soundDBusInter;

#ifndef DCC_DISABLE_FEEDBACK
    enum class MeterState { Idle, Pending, Active };

    QHash<QObject *, QMetaObject::Connection> m_feedbackConsumers;
    MeterState m_meterState = MeterState::Idle;
    QString m_meterSourcePath;
    quint64 m_meterGeneration = 0;
    QPointer<QDBusPendingCallWatcher> m_meterWatcher;
    QTimer *m_pingTimer;
#endif
    QDBusConnectionInterface *m_inter;

    QSoundEffect* m_sound;
    int m_waitSoundPortReceipt;
    QTimer* m_waitInputReceiptTimer;
    QTimer* m_waitOutputReceiptTimer;
    QMediaDevices* m_mediaDevices;

    QTimer* m_playAnimationTime;
    int m_upateSoundEffectsIndex;
    QString m_playAniIconPath;
};

#endif // SOUNDWORKER_H
