/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef SOUNDWIDGET_H
#define SOUNDWIDGET_H

#include "modules/modulewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "soundmodel.h"
#include "widgets/nextpagewidget.h"

#include "widgets/titledslideritem.h"

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

Q_SIGNALS:
    void requestSwitchSpeaker(bool on) const;
    void requestSiwtchMicrophone(bool on) const;
    void requestSwitchSoundEffect(bool on) const;
    void requestSetSpeakerBalance(double balance) const;
    void requestSetMicrophoneVolume(double volume) const;
    void requestSetSpeakerVolume(double volume);
    void requestAdvancedPage() const;
    void requestEffectPage() const;

private Q_SLOTS:
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
    NextPageWidget *m_soundEffectBtn;

    QGSettings *m_gsettings;
};

}
}
#endif // SOUNDWIDGET_H
