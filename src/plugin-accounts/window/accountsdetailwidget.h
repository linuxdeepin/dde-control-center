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
#include "src/plugin-accounts/operation/user.h"
#include "avatarwidget.h"
#include "avatarlistwidget.h"

#include <DSpinBox>

#include <QScrollArea>
#include <QFocusEvent>

DWIDGET_USE_NAMESPACE
DWIDGET_BEGIN_NAMESPACE
class DToolButton;
class DWarningButton;
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
class QStandardItemModel;
class QScrollArea;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
class UserModel;
class ComboxWidget;
}


namespace DCC_NAMESPACE {
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
    explicit AccountsDetailWidget(User *user, UserModel *model, QWidget *parent = nullptr);
    virtual ~AccountsDetailWidget();
    void initHeadPart(QVBoxLayout *headLayout);
    void initBodyPart(QVBoxLayout *bodyLayout);
    void setAccountModel(UserModel *model);
    //获取其它用户是否开启自动登录开关
    QString getOtherUserAutoLogin();
    void setDeleteBtnStatus(const QString &key, const bool &status);
    void setModifyPwdBtnStatus(const QString &key);
    bool isSystemAdmin(User *user);

Q_SIGNALS:
    void requestShowPwdSettings(User *user);
    void requestShowSecurityQuestionsSettings(User *user);
    void requestSetAutoLogin(User *user, const bool autoLogin);
    void requestSetAdministrator(User *user, const bool asAdministrator);
    void requestNopasswdLogin(User *user, const bool nopasswdLogin);
    void requestDeleteAccount(User *user, const bool deleteHome);
    void requestBack();
    void requestSetAvatar(User *user, const QString &filePath);
    void requestSetFullname(User *user, const QString &fullname);
    void requestCleanThumbs(User *user);
    void requestSetGroups(User *user, const QStringList &usrGroups);
    void requsetSetPassWordAge(User *user, const int age);
    void noticeEnrollCompleted(QString username);
    void editingFinished(const QString& userFullName);
    void requestSecurityQuestionsCheck(User *user);

public Q_SLOTS:
    void resetDelButtonState();
    void onEditingFinished(bool isValid, const QString& userFullName);
    void onSecurityQuestionsCheckReplied(const QList<int> &questions);
    void updateItemCheckStatus(DStandardItem *item, const bool checked);

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
    User *m_curUser;
    UserModel *m_userModel;
    DLabel *m_fullName;//账户全名
    DToolButton *m_fullNameBtn;//账户全名编辑按钮
    DLineEdit *m_inputLineEdit;//账户全名编辑框
    AvatarListWidget *m_avatarListWidget;//图像列表
    DListView *m_groupListView;
    QStandardItemModel *m_groupItemModel;
    bool m_isServerSystem;
    DStandardItem *m_autoLoginItem;
    DStandardItem *m_nopasswdLoginItem;
    ComboxWidget *m_asAdministrator;//作为管理员
    QHBoxLayout *m_avatarLayout;
    DWarningButton *m_deleteAccount;
    QPushButton *m_modifyPassword;
    QScrollArea *m_scrollArea;
    QString m_groupName;
    User *m_curLoginUser;
    QLabel *m_bindStatusLabel;
    DListView *m_view;
    QStandardItemModel *m_viewModel;
};

}   // namespace DCC_NAMESPACE
