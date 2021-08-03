/*
 * Copyright (C) 2019 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     dongrui <dongrui@uniontech.com>
 *
 * Maintainer: dongrui <dongrui@uniontech.com>
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

#ifndef MOUSEDBUSPROXY_H
#define MOUSEDBUSPROXY_H

#include "mouseworker.h"

#include <com_deepin_daemon_inputdevice_mouse.h>
#include <com_deepin_daemon_inputdevice_touchpad.h>
#include <com_deepin_daemon_inputdevice_trackpoint.h>
#include <com_deepin_daemon_inputdevices.h>

#include <QObject>

using Mouse = com::deepin::daemon::inputdevice::Mouse;
using TouchPad = com::deepin::daemon::inputdevice::TouchPad;
using TrackPoint = com::deepin::daemon::inputdevice::TrackPoint;
using InputDevices = com::deepin::daemon::InputDevices;

namespace dcc {
namespace mouse {
class MouseDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MouseDBusProxy(MouseWorker *worker, QObject *parent = 0);
    void active();
    void deactive();
    void init();

public Q_SLOTS:
    void onDefaultReset();
    void setLeftHandState(const bool state);
    void setDouClick(const int &value);

    // mouse settings
    void setMouseNaturalScrollState(const bool state);
    void setDisTouchPad(const bool state);
    void setAccelProfile(const bool state);
    void setMouseMotionAcceleration(const double &value);

    // touchpad settings
    void setTouchNaturalScrollState(const bool state);
    void setDisTyping(const bool state);
    void setTouchpadMotionAcceleration(const double &value);
    void setTapClick(const bool state);
    void setPalmDetect(bool palmDetect);
    void setPalmMinWidth(int palmMinWidth);
    void setPalmMinz(int palmMinz);

    // redpoint settings
    void setTrackPointMotionAcceleration(const double &value);

    // device properties
    void setScrollSpeed(uint speed);

private:
    Mouse        *m_dbusMouse;
    TouchPad     *m_dbusTouchPad;
    TrackPoint   *m_dbusTrackPoint;
    InputDevices *m_dbusDevices;
    MouseWorker  *m_worker;
};
}
}



#endif // MOUSEWORKER_H
