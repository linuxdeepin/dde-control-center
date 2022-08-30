// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
