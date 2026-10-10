// SPDX-FileCopyrightText: 2019 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDCONTROLLER_H
#define SOUNDCONTROLLER_H

#include <QObject>

#include <DSingleton>
#include <DConfig>

#include "audio1interface.h"
#include "audio1sinkinterface.h"

DCORE_USE_NAMESPACE

using DBusAudio = org::deepin::dde::Audio1;
using DBusSink = org::deepin::dde::audio1::Sink;

class SoundController final : public QObject, public Dtk::Core::DSingleton<SoundController>
{
    friend class Dtk::Core::DSingleton<SoundController>;

    Q_OBJECT
public:
    DBusAudio *audioInter() const { return m_audioInter; }
    DBusSink *defaultSinkInter() const { return m_defaultSinkInter; }

    bool mute();
    double volume();
    void SetVolume(double in0, bool in1);
    void SetMuteQueued(bool in0);
    void SetMute(bool in0);
    bool existActiveOutputDevice() const;

Q_SIGNALS:
    void defaultSinkChanged(DBusSink *);
    void VolumeChanged(double  value) const;
    void MuteChanged(bool  value) const;

private:
    SoundController(QObject *parent = nullptr);
    ~SoundController();

    void onDefaultSinkChanged(const QDBusObjectPath &path);

private:
    DBusAudio *m_audioInter;
    QPointer<DBusSink> m_defaultSinkInter;
    QSharedPointer<DConfig> m_dconfig;
};

#endif
