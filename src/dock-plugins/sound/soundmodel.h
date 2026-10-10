// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDMODEL_H
#define SOUNDMODEL_H

#include "soundcardport.h"

#include <QPointer>
#include <QObject>
#include <QMap>

#include <DSingleton>

class SoundModel : public QObject, public Dtk::Core::DSingleton<SoundModel>
{
    friend class Dtk::Core::DSingleton<SoundModel>;

    Q_OBJECT
public:
    void setVolume(double volume);
    int volume() const { return m_volume; }

    void setMute(bool mute);
    bool isMute() const { return m_mute; }

    void setCardsInfo(const QString &info);

    SoundCardPort* port(const QString &key) const { return m_ports.value(key, nullptr); }
    QList<SoundCardPort*> ports() const { return m_ports.values(); }

    void setActivePort(uint cardId, const QString &portName);
    SoundCardPort* activePort() const;
    void setMaxVolumeUI(double volume);
    int maxVolumeUI() const { return m_maxVolumeUI; }

    bool existActiveOutputDevice() const;

Q_SIGNALS:
    void volumeChanged(int volume);
    void muteStateChanged(bool isMute);
    void cardsInfoChanged();
    void enabledPortAdded(SoundCardPort*);
    void portRemoved(const QString &key);
    void maxVolumeUIChanged(int volume);
    void activePortChanged(SoundCardPort*);

private:
    SoundModel();
    ~SoundModel();

    void analysisCardsInfo();

private:
    int m_volume;
    bool m_mute;
    int m_maxVolumeUI; // UI 显示的最大音量，在开启增强的情况下是可能会超过 100
    QString m_cardsInfo;
    QMap<QString, SoundCardPort*> m_ports;
};

#endif
