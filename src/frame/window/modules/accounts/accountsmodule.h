/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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
