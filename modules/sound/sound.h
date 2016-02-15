/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUND_H
#define SOUND_H

#include <QObject>
#include <QtPlugin>

#include <dswitchlineexpand.h>
#include <dslider.h>
#include <dbuttonlist.h>
#include <dbaseexpand.h>
#include <dscrollarea.h>
#include <darrowlineexpand.h>


#include "interfaces.h"
#include "soundeffectswitchwidget.h"
#include "dbus/dbusaudio.h"
#include "dbus/dbusaudiosink.h"
#include "dbus/dbusaudiosource.h"
#include "dbus/dbussoundeffects.h"

DWIDGET_USE_NAMESPACE

class QFrame;
class Sound: public QObject, ModuleInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.deepin.ControlCenter.ModuleInterface" FILE "sound.json")
    Q_INTERFACES(ModuleInterface)

public:
    Sound();
    ~Sound() Q_DECL_OVERRIDE;
    QFrame *getContent() Q_DECL_OVERRIDE;

private:
    QFrame *m_frame;

    DSwitchLineExpand *m_speakerExpand;
    DSwitchLineExpand *m_microphoneExpand;
    DSlider *m_outputVolumeSlider;
    DSlider *m_leftRightBalanceSlider;
    DSlider *m_inputVolumeSlider;
    DSlider *m_inputFeedbackSlider;
    DBaseExpand *m_outputPortsExpand;
    DButtonList *m_outputPortsList;
    DBaseExpand *m_inputPortsExpand;
    DButtonList *m_inputPortsList;
    DBaseExpand *m_outputDevicesExpand;
    DButtonList *m_outputDevicesList;
    DBaseExpand *m_inputDevicesExpand;
    DButtonList *m_inputDevicesList;
    QFrame *m_mainWidget;
    DScrollArea *m_scrollArea;
    DArrowLineExpand *m_soundEffectsExpand;
    SoundEffectSwitchWidget *m_loginSoundSwitch;
    SoundEffectSwitchWidget *m_logoutSoundSwitch;
    SoundEffectSwitchWidget *m_shutdownSoundSwitch;
    SoundEffectSwitchWidget *m_restoreSoundSwitch;
    SoundEffectSwitchWidget *m_nitifySoundSwitch;
    SoundEffectSwitchWidget *m_unableOperateSoundSwitch;
    SoundEffectSwitchWidget *m_emptyTrashSoundSwitch;
    SoundEffectSwitchWidget *m_volumeSoundSwitch;
    SoundEffectSwitchWidget *m_batterySoundSwitch;
    SoundEffectSwitchWidget *m_powerInSoundSwitch;
    SoundEffectSwitchWidget *m_powerOutSoundSwitch;
    SoundEffectSwitchWidget *m_deviceInSoundSwitch;
    SoundEffectSwitchWidget *m_deviceOutSoundSwitch;
    SoundEffectSwitchWidget *m_sendAppToDesktopSoundSwitch;
    SoundEffectSwitchWidget *m_screenshotSoundSwitch;
    DBusSoundEffects *m_soundEffectsInter;


    DBusAudio *m_dbusAudio;
    DBusAudioSink *m_sink = nullptr;
    DBusAudioSource *m_source = nullptr;
    QList<DBusAudioSink *> m_sinks;
    QList<DBusAudioSource *> m_sources;
    QDBusInterface *m_dbusMeter = nullptr;
    QTimer m_meterTimer;
    QTimer m_delaySetOutputVolumeTimer;
    QTimer m_delaySetBalanceTimer;
    QTime m_timeDeltaRecorder;

    void initBackend();
    void initUI();

    com::deepin::daemon::Audio::Sink *getDefaultSink();
    com::deepin::daemon::Audio::Source *getDefaultSource();

    void updateSinks();
    void updateSources();
    void updateSpeakerUI();
    void updateMicrophoneUI();
    void updateOutputPorts();
    void updateInputPorts();
    void updateOutputDevices();
    void updateInputDevices();
    Q_SLOT void meterVolumeChanged(const QDBusMessage &msg);
};

#endif
