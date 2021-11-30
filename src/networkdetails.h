/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     donghualin <donghualin@uniontech.com>
 *
 * Maintainer: donghualin <donghualin@uniontech.com>
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

#ifndef NETWORKDETAILS_H
#define NETWORKDETAILS_H

#include <QObject>

class QJsonObject;

namespace dde {

namespace network {

class NetworkDeviceBase;

class NetworkDetails : QObject
{
    Q_OBJECT

    friend class NetworkController;
    friend class NetworkInterProcesser;

private:
    enum InfoType {
        Ip,
        Gateway
    };

public:
    // 连接的网络名称
    inline QString name() { return m_name; }
    inline QList<QPair<QString, QString>> items() { return m_items; }

private:
    NetworkDetails(QObject *parent = Q_NULLPTR);
    ~NetworkDetails();

    void updateData(const QJsonObject &info);
    void appendInfo(const QString &title, const QString &value);
    QString ipv6Infomation(QJsonObject connectinfo, InfoType type);
    QString devicePath();

    QString prefixToNetMask(int prefixLength);

private:
    QString m_name;
    QString m_devicePath;
    QList<QPair<QString, QString>> m_items;
};

}

}

#endif // UNETWORKDETAILS_H
