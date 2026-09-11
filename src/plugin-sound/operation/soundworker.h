// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDWORKER_H
#define SOUNDWORKER_H

#include <QObject>

#include "sounddbusproxy.h"
#include "soundmodel.h"
#include "audioport.h"

#include <DDesktopServices>
#include "qdbusconnectioninterface.h"
#include <QtQml/qqml.h>
#include <QTimer>
#include <QSoundEffect>
#include <QMediaDevices>

class SoundWorker : public QObject
{
    Q_OBJECT

    QML_NAMED_ELEMENT(SoundWorker)
    QML_SINGLETON
public:
    explicit SoundWorker(SoundModel *model, QObject * parent = 0);

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

private:
    SoundModel *m_model;
    QString m_activeSinkPort;
    QString m_activeSourcePort;
    uint m_activeOutputCard;
    uint m_activeInputCard;

    SoundDBusProxy *m_soundDBusInter;

    QTimer *m_pingTimer;
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
