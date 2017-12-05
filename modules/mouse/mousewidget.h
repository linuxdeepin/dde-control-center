/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef MOUSEWIDGET_H
#define MOUSEWIDGET_H

#include "modulewidget.h"

#include <QPushButton>
#include <QVBoxLayout>

namespace dcc
{
namespace widgets{
class SettingsGroup;
}
namespace mouse
{
class BaseSettings;
class MouseSettings;
class PalmDetectSetting;
class ThinkpadSettings;
class MouseModel;
class MouseModelMouseSettings;
class MouseWidget : public ModuleWidget
{
    Q_OBJECT

public:
    explicit MouseWidget();
    void setModel(MouseModel *const model);

signals:
    void setDefault();
    void requestSetLeftHand(const bool state);
    void requestSetMouseNaturalScroll(const bool state);
    void requestSetTouchNaturalScroll(const bool state);
    void requestSetDisTyping(const bool state);
    void requestSetDisTouchPad(const bool state);
    void requestSetTapClick(const bool state);
    void requestSetDouClick(const int &value);
    void requestSetMouseMotionAcceleration(const int &value);
    void requestSetTouchpadMotionAcceleration(const int &value);
    void requestSetTrackPointMotionAcceleration(const int &value);
    void requestDetectState(bool enable);
    void requestContact(int value);
    void requestPressure(int value);

private slots:
    void onTouchpadVisibleChanged(const bool visible);
    void onTouchpadHideChanged(const bool visiable);

private:
    QPushButton *m_nextPage;
    BaseSettings *m_baseSettings;
    MouseSettings *m_mouseSettings;
    MouseSettings *m_touchSettings;
    PalmDetectSetting *m_palmDetectSetting;
    ThinkpadSettings *m_ThinkapdSettings;
    MouseModelMouseSettings *m_touchpadModel;
    MouseModel *m_mouseModel;

};
}
}

#endif // MOUSEWIDGET_H
