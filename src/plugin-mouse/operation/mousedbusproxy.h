//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MOUSEDBUSPROXY_H
#define MOUSEDBUSPROXY_H

#include "mouseworker.h"

#include <QDBusMessage>
#include <QDBusPendingCallWatcher>
#include <QObject>

class QDBusInterface;

namespace DCC_NAMESPACE {
class MouseDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MouseDBusProxy(MouseWorker *worker, QObject *parent = nullptr);
    void deactive();
    void init();

    void parseGesturesData(const QDBusArgument &argument);

public Q_SLOTS:
    void active();
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

    // appearance
    void setCursorSize(const int cursorSize);
    void listCursor();

    // redpoint settings
    void setTrackPointMotionAcceleration(const double &value);

    // device properties
    void setScrollSpeed(uint speed);
    bool getLidIsPresent();

    void setGesture(const QString& name, const QString& direction, int fingers, const QString& action);

    void onMousePathPropertiesChanged(QDBusMessage msg);
    void onTouchpadPathPropertiesChanged(QDBusMessage msg);
    void onTrackpointPathPropertiesChanged(QDBusMessage msg);
    void onInputDevicesPathPropertiesChanged(QDBusMessage msg);
    void onGesturePropertiesChanged(QDBusMessage msg);
    void onAppearancePropertiesChanged(QDBusMessage msg);

    void onGetGestureAvaiableActionsFinished(QDBusPendingCallWatcher *w);

private:
    MouseWorker  *m_worker;
    QDBusInterface *m_dbusMouseProperties;
    QDBusInterface *m_dbusTouchPadProperties;
    QDBusInterface *m_dbusTrackPointProperties;
    QDBusInterface *m_dbusDevicesProperties;
    QDBusInterface *m_dbusGestureProperties;

    QDBusInterface *m_dbusMouse;
    QDBusInterface *m_dbusTouchPad;
    QDBusInterface *m_dbusTrackPoint;
    QDBusInterface *m_dbusDevices;
    QDBusInterface *m_dbusGesture;
    QDBusInterface *m_appearance;
};
}

#endif // MOUSEWORKER_H
