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

#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>

using namespace dcc;
using namespace dcc::notification;

#define SYSTEMNOTIFY_NAME "SystemNotify"

NotificationModel::NotificationModel(QObject *parent)
    : QObject(parent)
    , m_sysItemModel(new SysItemModel(this))
{

}

void NotificationModel::setAllSetting(const QJsonObject &obj)
{
    QJsonObject object = obj[SYSTEMNOTIFY_NAME].toObject();

    setSysSetting(object);
    setAllAppSetting(obj);

    Q_EMIT appListChanged();
}

void NotificationModel::setAllAppSetting(const QJsonObject &obj)
{
    qDeleteAll(m_appItemModels);
    m_appItemModels.clear();
    auto keys = obj.keys();
    // 所有的app设置中需要去掉系统设置选项
    for (int i = 0; i < keys.size(); i++) {
        if (keys[i] == SYSTEMNOTIFY_NAME) {
            continue;
        }
        AppItemModel *sitem = new AppItemModel(this);
        sitem->setItem(keys[i], obj.value(keys[i]).toObject());
        m_appItemModels.append(sitem);
    }
}

void NotificationModel::setAppSetting(const QJsonObject &object)
{
    AppItemModel *softModel = new AppItemModel;
    auto keys = object.keys();
    softModel->setItem(keys[0], object.value(keys[0]).toObject());

    for (int i = 0; i != m_appItemModels.size(); i++) {
        if (m_appItemModels[i]->getAppName() == softModel->getAppName()) {
            m_appItemModels[i] = softModel;
        } else {
            softModel->deleteLater();
        }
    }
}

void NotificationModel::setSysSetting(const QJsonObject &object)
{
    if (!object.isEmpty())
        m_sysItemModel->setItem(object);
}

void NotificationModel::appAdded(const QString &appNamme)
{
    AppItemModel *softModel = new AppItemModel;
    softModel->setSoftName(appNamme);
    m_appItemModels.append(softModel);

    Q_EMIT appListChanged();
}

void NotificationModel::appRemoved(const QString &appName)
{
    for (int i = 0; i < m_appItemModels.size(); i++) {
        if (m_appItemModels[i]->getAppName() == appName) {
            m_appItemModels.removeAt(i);
            break;
        }
    }

    Q_EMIT appListChanged();
}

void NotificationModel::setTheme(const QString &theme)
{
    if (m_theme == theme)
        return;
    m_theme = theme;
    Q_EMIT themeChanged(m_theme);
}



