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
#include "src/plugin-mouse/operation/mousemodel.h"
#include "src/plugin-mouse/operation/mouseworker.h"

#include <QWidget>

class QVBoxLayout;

namespace DCC_NAMESPACE {
class DouTestWidget;

class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
}

namespace DCC_NAMESPACE {
class GeneralSettingWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GeneralSettingWidget(QWidget *parent = nullptr);
    ~GeneralSettingWidget();

    void setModel(MouseModel *const model);
Q_SIGNALS:
    void requestSetLeftHand(const bool state);
    void requestSetDisTyping(const bool state);
    void requestScrollSpeed(const int speed);
    void requestSetDouClick(const int value);
private:
    void onDoubleClickSpeedChanged(int speed);
    void onScrollSpeedChanged(uint speed);
private:
    MouseModel *m_mouseModel;
    SettingsGroup *m_generalSettingsGrp;

    SwitchWidget *m_leftHand;
    SwitchWidget *m_disInTyping;
    TitledSliderItem *m_doubleSlider;
    TitledSliderItem *m_scrollSpeedSlider;
    DouTestWidget *m_doubleTest;
    QVBoxLayout *m_contentLayout;
};
}
#endif // GENERALSETTINGWIDGET_H
