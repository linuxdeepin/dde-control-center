//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MOUSEDBUSPROXY_H
#define MOUSEDBUSPROXY_H

#include "mouseworker.h"

#include <QObject>
#include <QDBusMessage>

class QDBusInterface;

namespace DCC_NAMESPACE {
class MouseDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MouseDBusProxy(MouseWorker *worker, QObject *parent = nullptr);
    void active();
    void deactive();
    void init();

public Q_SLOTS:
    void onDefaultReset();
    void setLeftHandState(const bool state);
    void setDouClick(const int &value);

    // mouse settings
    void setMouseNaturalScrollState(const bool state);
    void setDisableTouchPadWhenMouseExist(const bool state);
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
    void setTouchpadEnabled(bool state);

    // redpoint settings
    void setTrackPointMotionAcceleration(const double &value);

    // device properties
    void setScrollSpeed(uint speed);

    void onMousePathPropertiesChanged(QDBusMessage msg);
    void onTouchpadPathPropertiesChanged(QDBusMessage msg);
    void onTrackpointPathPropertiesChanged(QDBusMessage msg);
    void onInputDevicesPathPropertiesChanged(QDBusMessage msg);

private:
    MouseWorker  *m_worker;
    QDBusInterface *m_dbusMouseProperties;
    QDBusInterface *m_dbusTouchPadProperties;
    QDBusInterface *m_dbusTrackPointProperties;
    QDBusInterface *m_dbusDevicesProperties;

    QDBusInterface *m_dbusMouse;
    QDBusInterface *m_dbusTouchPad;
    QDBusInterface *m_dbusTrackPoint;
    QDBusInterface *m_dbusDevices;

};
}



#endif // MOUSEWORKER_H
