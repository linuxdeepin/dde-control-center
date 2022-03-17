/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: andywang <wangwei_cm@deepin.com>
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
#ifndef TOUCHPADSETTINGWIDGET_H
#define TOUCHPADSETTINGWIDGET_H

#include "interface/namespace.h"
#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class MouseModel;
class MouseWorker;
class DouTestWidget;
class PalmDetectSetting;

class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}

namespace DCC_NAMESPACE {
class TouchPadSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TouchPadSettingWidget(QWidget *parent = nullptr);
    void setModel(MouseModel *const model);

Q_SIGNALS:
    void requestSetTouchpadMotionAcceleration(const int value);
    void requestSetTapClick(const bool state);
    void requestSetTouchNaturalScroll(const bool state);
    void requestDetectState(bool enable);
    void requestContact(int value);
    void requestPressure(int value);

private Q_SLOTS:
    void onTouchMoveSpeedChanged(int speed);

private:
    MouseModel *m_mouseModel;
    PalmDetectSetting *m_palmDetectSetting;
    TitledSliderItem *m_touchMoveSlider;
    SwitchWidget *m_touchClickStn;
    SwitchWidget *m_touchNaturalScroll;
    QVBoxLayout *m_contentLayout;
};
}
#endif // TOUCHPADSETTINGWIDGET_H
