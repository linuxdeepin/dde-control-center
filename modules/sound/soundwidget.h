/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef SOUNDWIDGET_H
#define SOUNDWIDGET_H

#include "modulewidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "soundmodel.h"
#include "nextpagewidget.h"

#include "titledslideritem.h"

using namespace dcc;
using namespace dcc::widgets;

namespace dcc {
namespace sound {

class SoundWidget : public ModuleWidget
{
    Q_OBJECT
public:
    explicit SoundWidget(SoundModel *model);
    ~SoundWidget();

    void setModel(SoundModel *model);

signals:
    void requestSwitchSpeaker(bool on) const;
    void requestSiwtchMicrophone(bool on) const;
    void requestSwitchSoundEffect(bool on) const;
    void requestSetSpeakerBalance(double balance) const;
    void requestSetMicrophoneVolume(double balance) const;

private:
    SettingsGroup *m_speakerGroup;
    SwitchWidget *m_speakerSwitch;
    TitledSliderItem *m_outputBalanceSliderItem;
    DCCSlider *m_outputBalanceSlider;

    SettingsGroup *m_microphoneGroup;
    SwitchWidget *m_microphoneSwitch;
    TitledSliderItem *m_inputVolumeSliderItem;
    DCCSlider *m_inputVolumeSlider;
    TitledSliderItem *m_inputFeedbackSliderItem;
    DCCSlider *m_inputFeedbackSlider;

    SettingsGroup *m_advancedSettingsGroup;
    NextPageWidget *m_advancedSettingsItem;

    SettingsGroup *m_soundEffectGroup;
    SwitchWidget *m_soundEffectSwitch;
};

}
}
#endif // SOUNDWIDGET_H
