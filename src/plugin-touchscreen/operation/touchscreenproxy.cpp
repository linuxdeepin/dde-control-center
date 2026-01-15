//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "touchscreenproxy.h"

#include <QDBusConnection>
#include <QDBusInterface>

DCORE_USE_NAMESPACE

// using namespace DCC_NAMESPACE;
namespace DCC_NAMESPACE {
TouchScreenProxy::TouchScreenProxy(QObject *parent)
    : QObject(parent)
    , m_displayInter(new DDBusInterface("org.deepin.dde.Display1", "/org/deepin/dde/Display1", "org.deepin.dde.Display1", QDBusConnection::sessionBus(), this))
{
    registerTouchscreenInfoList_V2MetaType();
}

TouchscreenInfoList_V2 TouchScreenProxy::touchscreensV2()
{
    return qvariant_cast<TouchscreenInfoList_V2>(m_displayInter->property("TouchscreensV2"));
}

TouchscreenMap TouchScreenProxy::touchMap()
{
    return qvariant_cast<TouchscreenMap>(m_displayInter->property("TouchMap"));
}

QList<QDBusObjectPath> TouchScreenProxy::monitors()
{
    return qvariant_cast<QList<QDBusObjectPath>>(m_displayInter->property("Monitors"));
}

QDBusPendingReply<> TouchScreenProxy::AssociateTouchByUUID(const QString &in0, const QString &in1)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0) << QVariant::fromValue(in1);
    return m_displayInter->asyncCallWithArgumentList(QStringLiteral("AssociateTouchByUUID"), argumentList);
}
}
