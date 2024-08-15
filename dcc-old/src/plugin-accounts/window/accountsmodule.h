//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "interface/pagemodule.h"
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
class ModuleObjectItem;

class AccountsPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Accounts" FILE "AccountsPlugin.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)
public:
    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString location() const override;
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
class AccountsModule : public PageModule
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
    void active() override;
    void deactive() override;
    bool isSystemAdmin(User *user);

Q_SIGNALS:
    void currentUserChanged(User *user, User *oldUser);
    void deactivated();

protected Q_SLOTS:
    QWidget *initAccountsList(ModuleObject *module);
    QWidget *initCreateAccount(ModuleObject *module);
    QWidget *initAvatar(ModuleObject *module);
    QWidget *initFullName(ModuleObject *module);
    QWidget *initFullNameEdit(ModuleObject *module);
    QWidget *initFullNameIcon(ModuleObject *module);
    QWidget *initName(ModuleObject *module);
    QWidget *initChangePassword(ModuleObject *module);
    QWidget *initDeleteAccount(ModuleObject *module);
    QWidget *initAccountType(ModuleObject *module);
    QWidget *initValidityDays(ModuleObject *module);

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
    void updateFullnameVisible(uint32_t flag, bool state);
    void onShowSafetyPage(const QString &errorTips);
    void onLoginModule(ModuleObject *module);
    void updateLoginModule();

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

    DCC_NAMESPACE::ModuleObjectItem *m_autoLoginModule;
    DCC_NAMESPACE::ModuleObjectItem *m_loginWithoutPasswordModule;

    DCC_NAMESPACE::ModuleObject *m_fullNameModule;
    DCC_NAMESPACE::ModuleObject *m_fullNameEditModule;
    DCC_NAMESPACE::ModuleObject *m_fullNameIconModule;
    DCC_NAMESPACE::ModuleObject *m_changePasswordModule;
    DCC_NAMESPACE::ModuleObject *m_deleteAccountModule;
    DCC_NAMESPACE::ModuleObject *m_accountTypeModule;
    DCC_NAMESPACE::ModuleObject *m_validityDaysModule;
};

} // namespace DCC_NAMESPACE
