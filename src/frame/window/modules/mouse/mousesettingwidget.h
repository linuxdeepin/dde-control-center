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
#ifndef MOUSESETTINGWIDGET_H
#define MOUSESETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

namespace dcc {
namespace mouse {
class MouseModel;
class MouseWorker;
}

namespace widgets {
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}
}

namespace DCC_NAMESPACE {
namespace mouse {
class MouseSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit MouseSettingWidget(QWidget *parent = nullptr);
    void setModel(dcc::mouse::MouseModel *const model);

Q_SIGNALS:
    void requestSetMouseMotionAcceleration(const int value);
    void requestSetAccelProfile(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetMouseNaturalScroll(const bool state);

private Q_SLOTS:
    void onMouseMoveSpeedChanged(int speed);

private:
    bool m_isNotWayland{false};
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::widgets::SettingsGroup *m_mouseSettingsGrp;
    dcc::widgets::TitledSliderItem *m_mouseMoveSlider;
    //dcc::widgets::SwitchWidget *m_adaptiveAccelProfile;
    dcc::widgets::SwitchWidget *m_disTchStn;
    dcc::widgets::SwitchWidget *m_mouseNaturalScroll;
    QVBoxLayout *m_contentLayout;
};
}
}
#endif // MOUSESETTINGWIDGET_H
