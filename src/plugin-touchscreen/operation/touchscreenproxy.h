//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef TOUCHSCREENPROXY_H
#define TOUCHSCREENPROXY_H

#include "types/touchscreeninfolist_v2.h"
#include "types/touchscreenmap.h"

#include <DDBusInterface>
#include <QDBusReply>
#include <QObject>

namespace DCC_NAMESPACE {

class QDBusMessage;

class TouchScreenProxy : public QObject
{
    Q_OBJECT
public:
    explicit TouchScreenProxy(QObject *parent = nullptr);

    Q_PROPERTY(TouchscreenInfoList_V2 TouchscreensV2 READ touchscreensV2 NOTIFY TouchscreensV2Changed)
    TouchscreenInfoList_V2 touchscreensV2();

    Q_PROPERTY(TouchscreenMap TouchMap READ touchMap NOTIFY TouchMapChanged)
    TouchscreenMap touchMap();

    Q_PROPERTY(QList<QDBusObjectPath> Monitors READ monitors NOTIFY MonitorsChanged)
    QList<QDBusObjectPath> monitors();

Q_SIGNALS:
    void TouchscreensV2Changed(TouchscreenInfoList_V2 value);
    void MonitorsChanged(const QList<QDBusObjectPath> & value);
    void TouchMapChanged(TouchscreenMap value);

public Q_SLOTS:
    QDBusPendingReply<> AssociateTouchByUUID(const QString &in0, const QString &in1);

private:
    Dtk::Core::DDBusInterface *m_displayInter;
    TouchscreenInfoList_V2 m_TouchscreensList;
    QList<QDBusObjectPath> m_Monitors;
    TouchscreenMap m_TouchMap;
};
}
#endif // TOUCHSCREENPROXY_H
