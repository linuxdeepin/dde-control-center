/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#ifndef MOUSEWIDGET_H
#define MOUSEWIDGET_H

#include "modules/modulewidget.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc
{
namespace widgets{
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
namespace mouse
{
class MouseModel;
class DouTestWidget;
class PalmDetectSetting;
class MouseWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit MouseWidget();
    void setModel(MouseModel *const model);

Q_SIGNALS:
    void setDefault();
    void requestSetLeftHand(const bool state);
    void requestSetMouseNaturalScroll(const bool state);
    void requestSetTouchNaturalScroll(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetTapClick(const bool state);
    void requestSetDouClick(const int &value);
    void requestSetMouseMotionAcceleration(const int &value);
    void requestSetAccelProfile(const bool state);
    void requestSetTouchpadMotionAcceleration(const int &value);
    void requestSetTrackPointMotionAcceleration(const int &value);
    void requestDetectState(bool enable);
    void requestContact(int value);
    void requestPressure(int value);
    void requestScrollSpeed(int speed);

private Q_SLOTS:
    void onTouchpadHideChanged(const bool visible);
    void onMouseMoveSpeedChanged(int speed);
    void onTouchMoveSpeedChanged(int speed);
    void onRedPointMoveSpeedChanged(int speed);
    void onDoubleClickSpeedChanged(int speed);
    void onScrollSpeedChanged(int speed);

private:
    MouseModel *m_mouseModel;
    widgets::SettingsGroup *m_baseSettingsGrp;
    widgets::SettingsGroup *m_mouseSettingsGrp;
    widgets::SettingsGroup *m_touchSettingsGrp;
    widgets::SettingsGroup *m_thinkapdSettingsGrp;

    widgets::SwitchWidget *m_leftHand;
    widgets::SwitchWidget *m_adaptiveAccelProfile;
    widgets::SwitchWidget *m_disInTyping;
    widgets::TitledSliderItem *m_doubleSlider;
    DouTestWidget *m_doubleTest;

    widgets::TitledSliderItem *m_mouseMoveSlider;
    widgets::TitledSliderItem *m_scrollSpeedSlider;
    widgets::SwitchWidget *m_disTchStn;
    widgets::SwitchWidget *m_mouseNaturalScroll;

    widgets::TitledSliderItem *m_touchMoveSlider;
    widgets::SwitchWidget *m_touchClickStn;
    widgets::SwitchWidget *m_touchNaturalScroll;

    PalmDetectSetting *m_palmDetectSetting;

    widgets::TitledSliderItem *m_trackMoveSlider;
};
}
}

#endif // MOUSEWIDGET_H
