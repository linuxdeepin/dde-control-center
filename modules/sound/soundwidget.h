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
#include "widgets/titledsettingsitem.h"

using namespace dcc;

class SoundWidget : public ModuleWidget
{
    Q_OBJECT
public:
    SoundWidget();
    ~SoundWidget();

private:
    SettingsGroup *m_speakerGroup;
    SwitchWidget *m_speakerSwitch;
    TitledSettingsItem *m_outputVolumeSliderItem;
    TitledSettingsItem *m_outputBalanceSliderItem;

    SettingsGroup *m_microphoneGroup;
    SwitchWidget *m_microphoneSwitch;
    TitledSettingsItem *m_inputVolumeSliderItem;
    TitledSettingsItem *m_inputFeedbackSliderItem;

    SettingsGroup *m_soundEffectGroup;
    SwitchWidget *m_soundEffectSwitch;
};

#endif // SOUNDWIDGET_H
