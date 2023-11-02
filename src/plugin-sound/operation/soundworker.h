//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SOUNDWORKER_H
#define SOUNDWORKER_H

#include <QObject>

#include "sounddbusproxy.h"
#include "soundmodel.h"
#include "audioport.h"

#include <DDesktopServices>
#include "qdbusconnectioninterface.h"

DWIDGET_USE_NAMESPACE

class SoundWorker : public QObject
{
    Q_OBJECT
public:
    explicit SoundWorker(SoundModel *model, QObject * parent = 0);

    void activate();
    void deactivate();


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
    void setPausePlayer(bool value);

    void setPort(const Port *port);
    void setEffectEnable(DDesktopServices::SystemSoundEffect effect, bool enable);
    void enableAllSoundEffect(bool enable);
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

private:
    void initConnect();
    void updatePortActivity();

private:
    SoundModel *m_model;
    QString m_activeSinkPort;
    QString m_activeSourcePort;
    uint m_activeOutputCard;
    uint m_activeInputCard;

    SoundDBusProxy *m_soundDBusInter;

    QTimer *m_pingTimer;
    QDBusConnectionInterface *m_inter;
    int m_waitSoundPortReceipt;
};

#endif // SOUNDWORKER_H
