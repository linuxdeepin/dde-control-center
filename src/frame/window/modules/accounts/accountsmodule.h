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

#include "window/interface/moduleinterface.h"

#include <com_deepin_daemon_accounts.h>

namespace dcc {
namespace accounts {
class User;
class AccountsWorker;
class UserModel;
class FingerWorker;
class FingerModel;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsWidget;
class AccountsModule : public QObject, public ModuleInterface
{
    Q_OBJECT

public:
    explicit AccountsModule(FrameProxyInterface *frame, QObject *parent = 0);

    void initialize();
    void reset();
    const QString name() const;
    void showPage(const QString &pageName);
    void contentPopped(QWidget *const w);
    void active() override;
    void load(QString path) override;

public Q_SLOTS:
    void onShowAccountsDetailWidget(dcc::accounts::User *account);
    void onShowCreateAccountPage();
    void onShowPasswordPage(dcc::accounts::User *account);
    void onShowAddThumb(const QString &name, const QString &thumb);

private:
    ~AccountsModule();

private:
    dcc::accounts::UserModel      *m_userList;
    dcc::accounts::AccountsWorker *m_accountsWorker;
    dcc::accounts::FingerWorker *m_fingerWorker;
    dcc::accounts::FingerModel *m_fingerModel;
    AccountsWidget *m_accountsWidget = nullptr;
};

}   // namespace accounts
}   // namespace dccV20
