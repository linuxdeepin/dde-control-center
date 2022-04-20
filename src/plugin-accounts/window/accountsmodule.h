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

#include "interface/namespace.h"
#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

class QVBoxLayout;
class QStackedWidget;

namespace DCC_NAMESPACE {
class User;
class AccountsWorker;
class UserModel;
class AccountsDetailWidget;
class AccountsListWidget;
class ModifyPasswdPage;

class AccountsPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID PluginInterface_iid FILE "AccountsPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
};


class AccountsModule : public ModuleObject
{
    Q_OBJECT
public:
    explicit AccountsModule(QObject *parent = nullptr);
    ~AccountsModule();
    AccountsWorker *work() { return m_worker; }
    UserModel *model() { return m_model; }

protected:
    virtual void active() override;

private:
    UserModel  *m_model;
    AccountsWorker *m_worker;
};
class AccountsPageModule : public ModuleObject
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
    explicit AccountsPageModule(UserModel *model, AccountsWorker *worker, QObject *parent = nullptr)
        : ModuleObject(parent), m_model(model), m_worker(worker)  {}

    virtual QWidget *page() override;

protected:
    bool eventFilter(QObject *o, QEvent *e) override;
    void deactive() override;
private:
    AccountsDetailWidget* updateAccountsDetailWidget(User *account);

    void onShowPasswordPage(User *account);
    void onShowSecurityQuestionsPage(User *account);
    void onShowCreateAccountPage();

private:
    UserModel  *m_model;
    AccountsWorker *m_worker;
    QWidget *m_page = nullptr;
    QVBoxLayout *m_mainContentLayout = nullptr;
    AccountsListWidget *m_accountsListWidget = nullptr;
    AccountsDetailWidget* m_accountsDetailWidget = nullptr;
    QStackedWidget *m_stackedWidget = nullptr;
};


}   // namespace DCC_NAMESPACE
