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
#include "widgets/contentwidget.h"

namespace dcc {
namespace mouse {
class MouseModel;
class MouseWorker;
class DouTestWidget;
class PalmDetectSetting;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class TouchPadSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit TouchPadSettingWidget(QWidget *parent = nullptr);
    void setModel(dcc::mouse::MouseModel *const model);

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
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::mouse::PalmDetectSetting *m_palmDetectSetting;
    dcc::widgets::TitledSliderItem *m_touchMoveSlider;
    dcc::widgets::SwitchWidget *m_touchClickStn;
    dcc::widgets::SwitchWidget *m_touchNaturalScroll;
    QVBoxLayout *m_contentLayout;
    bool m_isNotWayland{false};
};
}
}
#endif // TOUCHPADSETTINGWIDGET_H
