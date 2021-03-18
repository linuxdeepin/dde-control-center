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
#include "notificationmodel.h"
#include "model/sysitemmodel.h"
#include "model/appitemmodel.h"

using namespace dcc;
using namespace dcc::notification;

#define SYSTEMNOTIFY_NAME "SystemNotify"

NotificationModel::NotificationModel(QObject *parent)
    : QObject(parent)
    , m_sysItemModel(new SysItemModel(this))
{

}

void NotificationModel::setSysSetting(SysItemModel *item)
{
    m_sysItemModel = item;
}

void NotificationModel::clearModel()
{
    m_sysItemModel->deleteLater();
    m_sysItemModel = nullptr;
    qDeleteAll(m_appItemModels);
    m_appItemModels.clear();
}

void NotificationModel::appAdded(AppItemModel *item)
{
    m_appItemModels.append(item);
    Q_EMIT appListChanged();
}

void NotificationModel::appRemoved(const QString &appName)
{
    for (int i = 0; i < m_appItemModels.size(); i++) {
        if (m_appItemModels[i]->getActName() == appName) {
            m_appItemModels[i]->deleteLater();
            m_appItemModels[i] = nullptr;
            m_appItemModels.removeAt(i);
            break;
        }
    }

    Q_EMIT appListChanged();
}


