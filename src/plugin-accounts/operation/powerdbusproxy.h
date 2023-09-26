//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <DDBusInterface>
using Dtk::Core::DDBusInterface;
class PowerDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit PowerDBusProxy(QObject *parent = nullptr);

    // Power
    Q_PROPERTY(bool ScreenBlackLock READ screenBlackLock WRITE setScreenBlackLock NOTIFY ScreenBlackLockChanged)
    bool screenBlackLock();
    void setScreenBlackLock(bool value);
    Q_PROPERTY(bool SleepLock READ sleepLock WRITE setSleepLock NOTIFY SleepLockChanged)
    bool sleepLock();
    void setSleepLock(bool value);
    bool login1ManagerCanSuspend();

signals:
    // Power
    void ScreenBlackLockChanged(bool value) const;
    void SleepLockChanged(bool value) const;

private:
    DDBusInterface *m_powerInter;
    DDBusInterface *m_login1ManagerInter;
};
