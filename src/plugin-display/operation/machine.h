//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>
#include "interface/namespace.h"

namespace DCC_NAMESPACE {

class Machine : public QObject
{
    Q_OBJECT
public:
    explicit Machine(QObject *parent = 0);
    ~Machine();

    void setPath(const QString& path);
    inline QString Path() { return m_path; }

    inline const QString Name() { return m_name; }
    inline const QString IP() { return m_IP; }
    inline bool Connected() { return m_connected; }
    inline bool deviceSharing() { return m_deviceSharing; }
    inline bool isHistory() { return m_isHistoryDev; }
    inline QString UUID() { return m_UUID; }
    inline quint16 direction() { return m_direction; }

public Q_SLOTS:
    void setName(const QString& name);
    void setIP(const QString& ip);
    void setConnected(const bool connected);
    void setDeviceSharing(const bool deviceSharing);
    void setDisconnectStatus(bool status);
    void setHistoryStates(bool isHistory);
    void setUUID(const QString& uuid);
    void setDirection(const quint16 dir);

Q_SIGNALS:
    void connectedChanged(bool connected);
    void nameChanged(const QString& name);
    void IPChanged(const QString& Ip);
    void deviceSharingChanged(bool deviceSharing);
    void disconnectStatusChanged(bool status);
    void historyStatusChanged(bool status);
    void directionChanged(const quint16& dir);

private:
    QString m_path;
    QString m_name;
    QString m_IP;
    bool m_connected;
    bool m_deviceSharing;
    // 判断是否是历史设备
    bool m_isHistoryDev;
    QString m_UUID;
    quint16 m_direction;
};

}
#endif // MACHINE_H
