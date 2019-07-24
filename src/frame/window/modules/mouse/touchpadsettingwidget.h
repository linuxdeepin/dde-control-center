/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <wangwei_cm@deepin.com>
 *
 * Maintainer: zccrs <zhangjide@deepin.com>
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

#include "window/namespace.h"
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

namespace dcc {
namespace mouse {
class MouseModel;
class MouseWorker;
class DouTestWidget;
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
    explicit TouchPadSettingWidget(QWidget *parent = 0);
    void setModel(dcc::mouse::MouseModel *const model);
Q_SIGNALS:
    void requestSetTouchpadMotionAcceleration(const int &value);
    void requestSetTapClick(const bool state);
    void requestSetTouchNaturalScroll(const bool state);
private:
    void onTouchMoveSpeedChanged(int speed);
private:
    dcc::mouse::MouseModel *m_mouseModel;

    dcc::widgets::SettingsGroup *m_touchpadSettingsGrp;
    dcc::widgets::TitledSliderItem *m_touchMoveSlider;
    dcc::widgets::SwitchWidget *m_touchClickStn;
    dcc::widgets::SwitchWidget *m_touchNaturalScroll;

    QVBoxLayout *m_contentLayout;
};
}
}
#endif // TOUCHPADSETTINGWIDGET_H
