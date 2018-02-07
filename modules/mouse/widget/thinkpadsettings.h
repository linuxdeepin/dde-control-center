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

#ifndef THINKPADSETTINGS_H
#define THINKPADSETTINGS_H


#include "translucentframe.h"
#include <QObject>
#include <QVBoxLayout>

class QSlider;
namespace dcc
{

namespace widgets
{
class TitledSliderItem;
class SettingsGroup;
class SettingsHead;
}
namespace mouse
{
class MouseModelThinkpadSettings;
class ThinkpadSettings : public widgets::TranslucentFrame
{
    Q_OBJECT
public:
    explicit ThinkpadSettings(QWidget *parent = 0);
    void setModel(MouseModelThinkpadSettings *const baseSettings);

signals:
    void requestSetSliderValue(const int &value);

public slots:
    void setSliderValue(const int &value);

private:
    QVBoxLayout *m_mainLayout;
    widgets::SettingsGroup *m_mainGroup;
    MouseModelThinkpadSettings *m_baseSettings;
    widgets::TitledSliderItem *m_speedSlider;
    widgets::SettingsHead *m_title;
};
}
}


#endif // THINKPADSETTINGS_H
