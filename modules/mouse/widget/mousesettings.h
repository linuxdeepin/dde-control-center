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

#ifndef MOUSESETTINGS_H
#define MOUSESETTINGS_H

#include "translucentframe.h"
#include <QWidget>
#include <QVBoxLayout>

class QSlider;
namespace dcc
{

namespace widgets
{
class TitledSliderItem;
class SwitchWidget;
class SettingsGroup;
class SettingsHead;
class TranslucentFrame;
}
namespace mouse
{
class MouseModelMouseSettings;
class MouseSettings : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit MouseSettings(const QString &title, QWidget *parent = 0);
    void setModel(MouseModelMouseSettings *const baseSettings);
    void setSwitchVisible(const bool visible);

signals:
    void setDefault();
    void requestSetSwitch(const bool state);
    void requestSetSliderValue(const int &value);
    void requestSetNaturalScroll(const bool state);

public slots:
    void setSwitchTitle(const QString &title);
    void setSliderValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    widgets::SettingsHead   *m_title;
    widgets::SettingsGroup *m_mainGroup;
    MouseModelMouseSettings *m_baseSettings;
    widgets::TitledSliderItem *m_speedSlider;
    widgets::SwitchWidget *m_switchWidget;
    widgets::SwitchWidget *m_naturalWidget;
};
}
}


#endif // MOUSESETTINGS_H
