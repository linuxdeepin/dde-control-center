/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: Tianlu Shao <shaotianlu@uniontech.com>
 *             andywang <wangwei_cm@deepin.com>
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
#ifndef MOUSESETTINGWIDGET_H
#define MOUSESETTINGWIDGET_H

#include "interface/namespace.h"
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/operation/mouseworker.h"

#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
class MouseSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit MouseSettingWidget(QWidget *parent = nullptr);
    ~MouseSettingWidget();

    void setModel(MouseModel *const model);

Q_SIGNALS:
    void requestSetMouseMotionAcceleration(const int value);
    void requestSetAccelProfile(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetMouseNaturalScroll(const bool state);

private Q_SLOTS:
    void onMouseMoveSpeedChanged(int speed);

private:
    MouseModel *m_mouseModel;
    SettingsGroup *m_mouseSettingsGrp;
    TitledSliderItem *m_mouseMoveSlider;
    SwitchWidget *m_adaptiveAccelProfile;
    SwitchWidget *m_disTchStn;
    SwitchWidget *m_mouseNaturalScroll;
    QVBoxLayout *m_contentLayout;
};
}
#endif // MOUSESETTINGWIDGET_H
