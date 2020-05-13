/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#include "appitemmodel.h"

#include <QJsonObject>

using namespace dcc;
using namespace dcc::notification;

AppItemModel::AppItemModel(QObject *parent)
    : QObject(parent)
    , m_softName(QString())
    , m_isAllowNotify(false)
    , m_isNotifySound(false)
    , m_isLockShowNotify(false)
    , m_isOnlyInNotifyCenter(false)
    , m_isShowNotifyPreview(false)
{

}

void AppItemModel::setItem(const QString &name, const QJsonObject &item)
{
    setSoftName(name);
    setAllowNotify(item["AllowNotify"].toBool());
    setLockShowNotify(item["LockShowNotify"].toBool());
    setNotifySound(item["NotificationSound"].toBool());
    setOnlyInNotifyCenter(item["OnlyInNotifyCenter"].toBool());
    setShowNotifyPreview(item["ShowNotifyPreview"].toBool());
}

QJsonObject AppItemModel::convertQJson()
{
    QJsonObject json;
    json.insert("AllowNotify", isAllowNotify());
    json.insert("LockShowNotify", isLockShowNotify());
    json.insert("NotificationSound", isNotifySound());
    json.insert("OnlyInNotifyCenter", isOnlyInNotifyCenter());
    json.insert("ShowNotifyPreview", isShowNotifyPreview());
    return json;
}

void AppItemModel::setSoftName(const QString &name)
{
    if (m_softName != name) {
        m_softName = name;

        Q_EMIT softNameChanged(name);
    }
}

void AppItemModel::setAllowNotify(const bool &state)
{
    if (m_isAllowNotify != state) {
        m_isAllowNotify = state;

        Q_EMIT allowNotifyChanged(state);
    }
}

void AppItemModel::setNotifySound(const bool &state)
{
    if (m_isNotifySound != state) {
        m_isNotifySound = state;

        Q_EMIT notifySoundChanged(state);
    }
}

void AppItemModel::setLockShowNotify(const bool &state)
{
    if (m_isLockShowNotify != state) {
        m_isLockShowNotify = state;

        Q_EMIT lockShowNotifyChanged(state);
    }
}

void AppItemModel::setOnlyInNotifyCenter(const bool &state)
{
    if (m_isOnlyInNotifyCenter != state) {
        m_isOnlyInNotifyCenter = state;

        Q_EMIT onlyInNotifyCenterChanged(state);
    }
}

void AppItemModel::setShowNotifyPreview(const bool &state)
{
    if (m_isShowNotifyPreview != state) {
        m_isShowNotifyPreview = state;

        Q_EMIT showNotifyPreviewChanged(state);
    }
}
