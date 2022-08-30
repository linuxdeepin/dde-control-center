// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
