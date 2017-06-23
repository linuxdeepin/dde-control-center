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

#include <QGSettings>

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
#ifndef DCC_DISABLE_FEEDBACK
protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
#endif

signals:
    void requestSwitchSpeaker(bool on) const;
    void requestSiwtchMicrophone(bool on) const;
    void requestSwitchSoundEffect(bool on) const;
    void requestSetSpeakerBalance(double balance) const;
    void requestSetMicrophoneVolume(double volume) const;
    void requestSetSpeakerVolume(double volume);
    void requestAdvancedPage() const;

private slots:
    void onGSettingsChanged(const QString &name);

private:
    SettingsGroup *m_speakerGroup;
    SwitchWidget *m_speakerSwitch;
    TitledSliderItem *m_outputVolumeSliderItem;
    DCCSlider *m_outputVolumeSlider;
    TitledSliderItem *m_outputBalanceSliderItem;
    DCCSlider *m_outputBalanceSlider;

    SettingsGroup *m_microphoneGroup;
    SwitchWidget *m_microphoneSwitch;
    TitledSliderItem *m_inputVolumeSliderItem;
    DCCSlider *m_inputVolumeSlider;

#ifndef DCC_DISABLE_FEEDBACK
    TitledSliderItem *m_inputFeedbackSliderItem;
    DCCSlider *m_inputFeedbackSlider;
#endif

    SettingsGroup *m_advancedSettingsGroup;
    NextPageWidget *m_advancedSettingsItem;

    SettingsGroup *m_soundEffectGroup;
    SwitchWidget *m_soundEffectSwitch;

    QGSettings *m_gsettings;
};

}
}
#endif // SOUNDWIDGET_H
