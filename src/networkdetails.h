// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

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
