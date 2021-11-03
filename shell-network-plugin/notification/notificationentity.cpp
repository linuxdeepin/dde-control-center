/*
 * Copyright (C) 2014 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     kirigaya <kirigaya@mkacg.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "notificationentity.h"

#include <QDateTime>

NotificationEntity::NotificationEntity(const QString &appName, const QString &id,
                                       const QString &appIcon, const QString &summary,
                                       const QString &body, const QStringList &actions,
                                       const QVariantMap hints, const QString &ctime,
                                       const QString &replacesId, const QString &timeout,
                                       QObject *parent) :
    QObject(parent),
    m_appName(appName),
    m_id(id.toUInt()),
    m_appIcon(appIcon),
    m_summary(summary),
    m_body(body),
    m_actions(actions),
    m_hints(hints),
    m_ctime(ctime),
    m_replacesId(replacesId),
    m_timeout(timeout),
    m_isTitle(false),
    m_hideCount(0),
    m_showPreview(true),
    m_index(0),
    m_showInNotifyCenter(true)
{

}

NotificationEntity::NotificationEntity(const NotificationEntity &notify) :
    NotificationEntity(notify.appName(), QString::number(notify.id()), notify.appIcon(), notify.summary(),
                       notify.body(), notify.actions(), notify.hints(), notify.ctime(),
                       notify.replacesId(), notify.timeout())
{

}

QString NotificationEntity::appName() const
{
    return m_appName;
}

void NotificationEntity::setAppName(const QString &appName)
{
    m_appName = appName;
}

uint NotificationEntity::id() const
{
    return m_id;
}

// id is guarrented to be uint by ourselves.
void NotificationEntity::setId(const QString &id)
{
    m_id = id.toUInt();
}

QString NotificationEntity::appIcon() const
{
    return m_appIcon;
}

void NotificationEntity::setAppIcon(const QString &appIcon)
{
    m_appIcon = appIcon;
}

QString NotificationEntity::summary() const
{
    return m_summary;
}

void NotificationEntity::setSummary(const QString &summary)
{
    m_summary = summary;
}

QString NotificationEntity::body() const
{
    return m_body;
}

void NotificationEntity::setBody(const QString &body)
{
    m_body = body;
}

QStringList NotificationEntity::actions() const
{
    return m_actions;
}

void NotificationEntity::setActions(const QStringList &actions)
{
    m_actions = actions;
}

QVariantMap NotificationEntity::hints() const
{
    return m_hints;
}

void NotificationEntity::setHints(const QVariantMap &hints)
{
    m_hints = hints;
}

QString NotificationEntity::ctime() const
{
    return m_ctime;
}

void NotificationEntity::setTime(const QString &time)
{
    m_ctime = time;
}

QString NotificationEntity::replacesId() const
{
    return m_replacesId;
}

void NotificationEntity::setReplacesId(const QString &replacesId)
{
    m_replacesId = replacesId;
}

QString NotificationEntity::timeout() const
{
    return m_timeout;
}

void NotificationEntity::setTimeout(const QString &timeout)
{
    m_timeout = timeout;
}

void NotificationEntity::setIsTitle(bool is)
{
    m_isTitle = is;
}

void NotificationEntity::setHideCount(int count)
{
    m_hideCount = count;
}

void NotificationEntity::setShowPreview(bool show)
{
    m_showPreview = show;
}

void NotificationEntity::setCurrentIndex(int idx)
{
    m_index = idx;
}

void NotificationEntity::setShowInNotifyCenter(bool isShow)
{
    m_showInNotifyCenter = isShow;
}
