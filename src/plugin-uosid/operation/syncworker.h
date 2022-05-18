/*
 * Copyright (C) 2016 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

 * Maintainer: duanhongyu <duanhongyu@uniontech.com>
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

#ifndef SYNCWORKER_H
#define SYNCWORKER_H

#include "syncdbusproxy.h"
#include "syncmodel.h"

#include <QObject>

struct BindCheckResult {
    QString ubid = "";
    QString error = "";
    bool ret = false;
};
class IntString;
class SyncModel;
class SyncDBusProxy;
class SyncWorker : public QObject
{
    Q_OBJECT
public:
    explicit SyncWorker(SyncModel * model, QObject *parent = nullptr);

    void activate();
    void refreshSyncState();

public Q_SLOTS:
    void setSync(std::pair<SyncType, bool> state);
    void setSyncState(const QString &syncType, bool state);
    void loginUser();
    void logoutUser();
    void asyncLogoutUser(const QString &ubid);
    void setAutoSync(bool autoSync);
    void licenseStateChangeSlot();

    void getUOSID();
    void getUUID();
    void getHostName();

    void asyncLocalBindCheck(const QString &uuid);
    void asyncBindAccount(const QString &uuid, const QString &hostName);
    void asyncUnbindAccount(const QString &ubid);

    void onSetFullname(const QString &fullname);
    void asyncSetFullname(const QString &fullname);
    /**
     * @brief onPullMessage  插件打开 若是登录状态 用户停留5秒以上 进行同步操作
     */
    void onPullMessage();
    void futurePullMessage();

private:
    void getUserDeepinidInfo();

private:
    void onSyncModuleStateChanged(const QString& module, bool enable);
    void onStateChanged(const IntString& state);
    void onLastSyncTimeChanged(qlonglong lastSyncTime);
    void getLicenseState();
    BindCheckResult checkLocalBind(const QString &uuid);
    BindCheckResult logout(const QString &uuid);
    BindCheckResult bindAccount(const QString &uuid, const QString &hostName);
    BindCheckResult unBindAccount(const QString &ubid);

private:
    SyncModel *m_model;
    SyncDBusProxy *m_syncInter;
};

#endif // SYNCWORKER_H
