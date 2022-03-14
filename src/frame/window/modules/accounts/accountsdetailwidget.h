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
#include "widgets/contentwidget.h"
#include "modules/accounts/user.h"
#include "modules/accounts/avatarwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "avatarlistwidget.h"
#include "widgets/titlelabel.h"

#include <DLineEdit>
#include <DWarningButton>
#include <DToolButton>
#include <DSpinBox>

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
class DWarningButton;
class DCommandLinkButton;
class DLineEdit;
class DLabel;
class DSpinBox;
DWIDGET_END_NAMESPACE

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QLineEdit;
class QEvent;
class QCommandLinkButton;
class QStackedWidget;
class QGSettings;
QT_END_NAMESPACE

namespace dcc {
namespace accounts {
class UserModel;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
class AccountSpinBox : public DSpinBox
{
    Q_OBJECT

public:
    explicit AccountSpinBox(QWidget *parent = nullptr);
protected:
    virtual QString textFromValue(int val) const;
    void focusInEvent(QFocusEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;
};


class AccountsDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(dcc::accounts::User *user, dcc::accounts::UserModel *model, QWidget *parent = nullptr);
    virtual ~AccountsDetailWidget();
    void initHeadPart(QVBoxLayout *headLayout);
    void initBodyPart(QVBoxLayout *bodyLayout);
    void setAccountModel(dcc::accounts::UserModel *model);
    //获取其它用户是否开启自动登录开关
    QString getOtherUserAutoLogin();
    void setDeleteBtnStatus(const QString &key, const bool &status);
    void setModifyPwdBtnStatus(const QString &key);
    bool isSystemAdmin(dcc::accounts::User *user);

Q_SIGNALS:
    void requestShowPwdSettings(dcc::accounts::User *user);
    void requestShowSecurityQuestionsSettings(dcc::accounts::User *user);
    void requestSetAutoLogin(dcc::accounts::User *user, const bool autoLogin);
    void requestSetAdministrator(dcc::accounts::User *user, const bool asAdministrator);
    void requestNopasswdLogin(dcc::accounts::User *user, const bool nopasswdLogin);
    void requestDeleteAccount(dcc::accounts::User *user, const bool deleteHome);
    void requestBack();
    void requestSetAvatar(dcc::accounts::User *user, const QString &filePath);
    void requestSetFullname(dcc::accounts::User *user, const QString &fullname);
    void requestCleanThumbs(dcc::accounts::User *user);
    void requestSetGroups(dcc::accounts::User *user, const QStringList &usrGroups);
    void requsetSetPassWordAge(dcc::accounts::User *user, const int age);
    void noticeEnrollCompleted(QString username);
    void editingFinished(const QString& userFullName);
    void requestSecurityQuestionsCheck(dcc::accounts::User *user);

public Q_SLOTS:
    void resetDelButtonState();
    void onEditingFinished(bool isValid, const QString& userFullName);
    void onSecurityQuestionsCheckReplied(const QList<int> &questions);

protected:
    void initUserInfo(QVBoxLayout *layout);
    void initSetting(QVBoxLayout *layout);
    void initGroups(QVBoxLayout *layout);
    bool eventFilter(QObject *obj, QEvent *event) override;
    void resizeEvent(QResizeEvent *event);

private Q_SLOTS:
    void deleteUserClicked();
    void changeUserGroup(const QStringList &groups);
    void userGroupClicked(const QModelIndex &index);
    void setGroupInfo(const QStringList &group);
    void onGidChanged(const QString &gid);

private:
    dcc::accounts::User *m_curUser;
    dcc::accounts::UserModel *m_userModel;
    DLabel *m_fullName;//账户全名
    DTK_WIDGET_NAMESPACE::DToolButton *m_fullNameBtn;//账户全名编辑按钮
    DLineEdit *m_inputLineEdit;//账户全名编辑框
    AvatarListWidget *m_avatarListWidget;//图像列表
    DTK_WIDGET_NAMESPACE::DListView *m_groupListView;
    QStandardItemModel *m_groupItemModel;
    bool m_isServerSystem;
    dcc::widgets::SwitchWidget *m_autoLogin;
    dcc::widgets::SwitchWidget *m_nopasswdLogin;
    dcc::widgets::SwitchWidget *m_asAdministrator;//作为管理员
    TitleLabel *m_accountSettingsTitle;//账户设置
    QHBoxLayout *m_avatarLayout;
    DWarningButton *m_deleteAccount;
    QPushButton *m_modifyPassword;
    QGSettings *m_gsettings;
    QScrollArea *m_scrollArea;
    QString m_groupName;
    dcc::accounts::User *m_curLoginUser;
    QLabel *m_bindStatusLabel;
};

}   // namespace accounts
}   // namespace dccV20
