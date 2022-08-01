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

#include <DLineEdit>
#include <DSpinBox>

class QVBoxLayout;
class QStackedWidget;
class QStandardItemModel;
class AccountsModel;

DWIDGET_BEGIN_NAMESPACE
class DLabel;
class DStandardItem;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {

class SettingsItem;
class User;
class AccountsWorker;
class UserModel;
class AccountsDetailWidget;
class AccountsListWidget;
class ModifyPasswdPage;
class SettingsGroup;
class DCCListView;

class AccountsPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Accounts" FILE "AccountsPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual int location() const override;
};
///////////////////////////////////////
class AccountSpinBox : public DTK_WIDGET_NAMESPACE::DSpinBox
{
    Q_OBJECT

public:
    explicit AccountSpinBox(QWidget *parent = nullptr);
protected:
    virtual QString textFromValue(int val) const override;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
};
///////////////////////////////////////
class AccountsModule : public ModuleObject
{
    Q_OBJECT

public:
    enum PassErrorCode {
        ErrCodeEmpty = 1,
        ErrCodeInvalidChar,
        ErrCodeFirstCharInvalid,
        ErrCodeExist,
        ErrCodeNameExist,
        ErrCodeSystemUsed,
        ErrCodeLen
    };

    explicit AccountsModule(QObject *parent = nullptr);
    ~AccountsModule();
    AccountsWorker *work() { return m_worker; }
    UserModel *model() { return m_model; }

protected:
    virtual void active() override;
    bool isSystemAdmin(User *user);

Q_SIGNALS:
    void currentUserChanged(User *user, User *oldUser);

protected Q_SLOTS:
    void initAccountsList(QWidget *w);
    void initAvatar(QWidget *w);
    void initFullName(QWidget *w);
    void initName(QWidget *w);
    void initModifyButton(QWidget *w);
    void initAccountType(SettingsGroup *accountSettingsGrp);
    void initAutoLogin(DCCListView *listview);
    void initValidityDays(SettingsGroup *pwGroup);

    void onCreateAccount();
    void onModifyPassword();
    void onDeleteUser();
    void onModifyIcon();
    void setCurrentUser(User *user);
    void setGroupInfo(const QStringList &group);
    void userGroupClicked(const QModelIndex &index);
    void changeUserGroup(const QStringList &groups);
    void onGidChanged(const QString &gid);
    bool onEditingFinished(bool isValid, DTK_WIDGET_NAMESPACE::DLineEdit *fullNameEdit);
    void setFullname(const QString &fullName, DTK_WIDGET_NAMESPACE::DLabel *fullNameLabel);
    void onShowSafetyPage(const QString &errorTips);

protected:
    QString getOtherUserAutoLogin(); //获取其它用户是否开启自动登录开关
    bool deleteUserBtnEnable();      // 可以删除用户

private:
    UserModel *m_model;
    AccountsWorker *m_worker;
    User *m_curLoginUser; // 当前登录用户
    User *m_curUser;      // 当前选中用户
    AccountsModel *m_accountsmodel;

    QStandardItemModel *m_groupItemModel;
    QString m_groupName;
    bool m_checkAuthorizationing;
};

} // namespace DCC_NAMESPACE
