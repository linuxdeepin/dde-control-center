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
#pragma once

#include <QObject>
#include <QMap>

QT_BEGIN_NAMESPACE
class QJsonArray;
QT_END_NAMESPACE

namespace dcc {
namespace notification {

class SysItemModel;
class AppItemModel;

class NotificationModel : public QObject
{
    Q_OBJECT
public:
    explicit NotificationModel(QObject *parent = nullptr);
    void setAllSetting(const QJsonObject &obj);
    void setAllAppSetting(const QJsonObject &obj);
    void setAppSetting(const QJsonObject &object);
    void setSysSetting(const QJsonObject &object);

    inline int getAppSize()const {return m_appItemModels.size();}

    inline SysItemModel *getSystemModel()const {return m_sysItemModel;}
    inline AppItemModel *getAppModel(const int &index) {return m_appItemModels[index];}

public Q_SLOTS:
    void appAdded(const QString &appNamme);
    void appRemoved(const QString &appName);

Q_SIGNALS:
    void appListChanged();

private:
    SysItemModel *m_sysItemModel;
    QList<AppItemModel *> m_appItemModels;
};

}   //namespace notification
}   //namespace dcc
