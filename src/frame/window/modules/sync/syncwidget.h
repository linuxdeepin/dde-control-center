/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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

#include "interface/namespace.h"

#include <QWidget>

QT_BEGIN_NAMESPACE
class QStackedLayout;
class QLabel;
QT_END_NAMESPACE

namespace dcc {
namespace cloudsync {
class SyncModel;
enum SyncType : int;
enum SyncState : int;
}
}

namespace DCC_NAMESPACE {
class MainWindow;
namespace sync {
class LoginPage;
class IndexPage;
class LogoutPage;
class SyncWidget : public QWidget
{
    Q_OBJECT
public:
    SyncWidget(QWidget *parent = nullptr);
    void setModel(dcc::cloudsync::SyncModel *model, MainWindow *pMainWindow);

Q_SIGNALS:
    void requestLoginUser() const;
    void requestLogoutUser() const;
    void requestAsyncLogoutUser(const QString &ubid) const;
    void requestSetAutoSync(bool enable) const;
    void requestSetModuleState(std::pair<dcc::cloudsync::SyncType, bool> state);
    void requestUOSID(QString &uosid);
    void requestUUID(QString &uuid);
    void requestLocalBindCheck(const QString &uosid, const QString &uuid);
    void requestHostName(QString &hostName);
    void requestBindAccount(const QString &uuid, const QString &hostName);
    void requestUnBindAccount(const QString &ubid);
    void ubid(const QString &ubid);
    void resetPasswdError(const QString &error);
    void unBindRet(bool ret);
private:
    void onUserInfoChanged(const QVariantMap &userInfo);

private:
    QStackedLayout *m_mainLayout;
    dcc::cloudsync::SyncModel *m_model;
    LoginPage *m_loginPage;
    IndexPage *m_indexPage;
    LogoutPage *m_cnonlyPage;
    MainWindow *m_pMainWindow;
};
} // namespace sync

} // namespace DCC_NAMESPACE
