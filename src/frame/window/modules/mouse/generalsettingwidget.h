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
#ifndef GENERALSETTINGWIDGET_H
#define GENERALSETTINGWIDGET_H

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

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
class GeneralSettingWidget : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit GeneralSettingWidget(QWidget *parent = nullptr);
    ~GeneralSettingWidget();

    void setModel(dcc::mouse::MouseModel *const model);
Q_SIGNALS:
    void requestSetLeftHand(const bool state);
    void requestSetDisTyping(const bool state);
    void requestScrollSpeed(const int speed);
    void requestSetDouClick(const int value);
private:
    void onDoubleClickSpeedChanged(int speed);
    void onScrollSpeedChanged(uint speed);
private:
    dcc::mouse::MouseModel *m_mouseModel;
    dcc::widgets::SettingsGroup *m_generalSettingsGrp;

    dcc::widgets::SwitchWidget *m_leftHand;
    dcc::widgets::SwitchWidget *m_disInTyping;
    dcc::widgets::TitledSliderItem *m_doubleSlider;
    dcc::widgets::TitledSliderItem *m_scrollSpeedSlider;
    dcc::mouse::DouTestWidget *m_doubleTest;
    QVBoxLayout *m_contentLayout;
};
}
}
#endif // GENERALSETTINGWIDGET_H
