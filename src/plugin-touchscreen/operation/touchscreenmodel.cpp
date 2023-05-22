//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "touchscreenmodel.h"
#include "touchscreenmodel_p.h"
#include "monitordbusproxy.h"

#include <ddbussender.h>

#include <QDateTime>
#include <QDebug>

TouchScreenModel::TouchScreenModel(QObject *parent)
    : QObject(parent)
    , DCC_INIT_PRIVATE(TouchScreenModel)
{

}

TouchScreenModel::~TouchScreenModel()
{

}

const TouchscreenInfoList_V2 &TouchScreenModel::touchScreenList() const
{
    Q_D(const TouchScreenModel);
    return d->m_touchScreenList;
}

const QStringList &TouchScreenModel::monitors()
{
    Q_D(const TouchScreenModel);
    return d->m_monitors;
}

const TouchscreenMap &TouchScreenModel::touchMap() const
{
    Q_D(const TouchScreenModel);
    return d->m_touchMap;
}

void TouchScreenModel::assoiateTouch(const QString &monitor, const QString &touchscreenUUID)
{
    Q_D(TouchScreenModel);
    d->assoiateTouch(monitor, touchscreenUUID);
}

void TouchScreenModel::assoiateTouchNotify()
{
    Q_D(TouchScreenModel);
    d->assoiateTouchNotify();
}

void TouchScreenModelPrivate::monitorsChanged(const QList<QDBusObjectPath> &monitors)
{
    if (monitors.empty())
        return;

    Q_Q(TouchScreenModel);
    m_monitors.clear();
    for (const QDBusObjectPath &path : monitors) {
        MonitorDBusProxy *inter = new MonitorDBusProxy(path.path());
        m_monitors << inter->name();
    }
    Q_EMIT q->monitorsChanged(m_monitors);
}

void TouchScreenModelPrivate::assoiateTouch(const QString &monitor, const QString &touchscreenUUID)
{
    m_displayProxy->AssociateTouchByUUID(monitor, touchscreenUUID);
}

void TouchScreenModelPrivate::assoiateTouchNotify()
{
    DDBusSender()
        .service("org.freedesktop.Notifications")
        .path("/org/freedesktop/Notifications")
        .interface("org.freedesktop.Notifications")
        .method("Notify")
        .arg(QString("dde-control-center"))
        .arg(static_cast<uint>(QDateTime::currentMSecsSinceEpoch()))
        .arg(QString("preferences-system"))
        .arg(QObject::tr("Touch Screen Settings"))
        .arg(QObject::tr("The settings of touch screen changed"))
        .arg(QStringList())
        .arg(QVariantMap())
        .arg(3000)
        .call();
}
