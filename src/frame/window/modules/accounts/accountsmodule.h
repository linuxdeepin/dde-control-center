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
    explicit AccountsModule(FrameProxyInterface *frame, QObject *parent = nullptr);
    void initFingerData();

    void initialize() override;
    void reset() override;
    const QString name() const override;
    const QString displayName() const override;
    void showPage(const QString &pageName) override;
    void contentPopped(QWidget *const w) override;
    void active() override;
    int load(const QString &path) override;
    QStringList availPage() const override;

public Q_SLOTS:
    void onShowAccountsDetailWidget(dcc::accounts::User *account);
    void onShowCreateAccountPage();
    void onShowPasswordPage(dcc::accounts::User *account);
    void onShowAddThumb(const QString &name, const QString &thumb);
    void onHandleVaildChanged(const bool isVaild);
    void onSetMainWindowEnabled(const bool isEnabled);

    void dealDevicesStatus(const bool &status);

private:
    ~AccountsModule();

private:
    dcc::accounts::UserModel *m_userModel{nullptr};
    dcc::accounts::AccountsWorker *m_accountsWorker{nullptr};
    dcc::accounts::FingerWorker *m_fingerWorker{nullptr};
    dcc::accounts::FingerModel *m_fingerModel{nullptr};
    AccountsWidget *m_accountsWidget = nullptr;
    MainWindow *m_pMainWindow = nullptr;

    QDBusInterface *m_inter;
    QString m_curName;
    QString m_selectName;
};

}   // namespace accounts
}   // namespace dccV20
