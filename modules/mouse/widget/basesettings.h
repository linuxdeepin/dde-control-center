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

#ifndef BASESETTINGS_H
#define BASESETTINGS_H

#include "translucentframe.h"
#include <QVBoxLayout>

namespace dcc
{
namespace widgets
{
class SettingsGroup;
class SwitchWidget;
class TitledSliderItem;
class TranslucentFrame;
}
namespace mouse
{
class MouseModelBaseSettings;
class BaseSettings : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit BaseSettings(QWidget *parent = 0);
    void setIsTypingVisible(const bool visible);

signals:
    void Reset();
    void requestSetLeftHand(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetSliderValue(const int &value);

public slots:
    void setModel(MouseModelBaseSettings *const baseSettings);
    void setSliderValue(const int &value);

private:
    QVBoxLayout   *m_mainLayout;
    widgets::SettingsGroup *m_mainGroup;
    widgets::SwitchWidget  *m_leftHand;
    widgets::SwitchWidget  *m_isTyping;
    widgets::TitledSliderItem *douSlider;
    MouseModelBaseSettings *m_baseSettings;
};
}
}


#endif // BASESETTINGS_H
