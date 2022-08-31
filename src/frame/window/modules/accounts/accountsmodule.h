// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/moduleinterface.h"
#include "../../mainwindow.h"

#include <com_deepin_daemon_accounts.h>

namespace dcc {
namespace accounts {
class User;
class AccountsWorker;
class UserModel;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsWidget;
class AccountsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

    enum PassErrorCode {
        ErrCodeEmpty = 1,
        ErrCodeInvalidChar,
        ErrCodeFirstCharInvalid,
        ErrCodeExist,
        ErrCodeNameExist,
        ErrCodeSystemUsed,
        ErrCodeLen
    };

public:
    explicit AccountsModule(FrameProxyInterface *frame, QObject *parent = nullptr);

    virtual void preInitialize(bool sync = false, FrameProxyInterface::PushType = FrameProxyInterface::PushType::Normal) override;
    void initialize() override;
    void reset() override;
    const QString name() const override;
    const QString displayName() const override;
    void showPage(const QString &pageName) override;
    void contentPopped(QWidget *const w) override;
    void active() override;
    int load(const QString &path) override;
    QStringList availPage() const override;
    void addChildPageTrans() const override;

Q_SIGNALS:
    void requestLoadUserList();
    void editingFinished(QDBusPendingReply<bool, QString, int> reply, const QString userFullName);

public Q_SLOTS:
    void onShowAccountsDetailWidget(dcc::accounts::User *account);
    void onShowCreateAccountPage();
    void onShowPasswordPage(dcc::accounts::User *account);
    void onShowSecurityQuestionsPage(dcc::accounts::User *account);
    void onSetMainWindowEnabled(const bool isEnabled);
    void onShowUserGroupsPage(dcc::accounts::User *user);
private:
    ~AccountsModule();
    void initSearchData() override;

private:
    dcc::accounts::UserModel *m_userModel{nullptr};
    dcc::accounts::AccountsWorker *m_accountsWorker{nullptr};
    AccountsWidget *m_accountsWidget = nullptr;
    MainWindow *m_pMainWindow = nullptr;
    bool m_isCreatePage;
};

}   // namespace accounts
}   // namespace dccV20
