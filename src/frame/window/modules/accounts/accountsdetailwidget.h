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
#include "modules/accounts/fingermodel.h"
#include "widgets/titlelabel.h"
#include "accountfingeitem.h"
#include "fingerwidget.h"

#include <DLineEdit>
#include <DDialog>
#include <DWarningButton>

DWIDGET_BEGIN_NAMESPACE
class DIconButton;
class DWarningButton;
class DCommandLinkButton;
class DLineEdit;
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
class FingerModel;
class UserModel;
}
}

using com::deepin::daemon::fprintd::Device;

namespace DCC_NAMESPACE {
namespace accounts {
class AccountsDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(dcc::accounts::User *user, QWidget *parent = nullptr);
    virtual ~AccountsDetailWidget();
    void initHeadPart(QVBoxLayout *headLayout);
    void initBodyPart(QVBoxLayout *bodyLayout);
    void setAccountModel(dcc::accounts::UserModel *model);
    void setFingerModel(dcc::accounts::FingerModel *model);
    //获取其它用户是否开启自动登录开关
    bool getOtherUserAutoLogin();
    void setDeleteBtnStatus(const QString &key, const bool &status);

Q_SIGNALS:
    void requestShowPwdSettings(dcc::accounts::User *user);
    void requestSetAutoLogin(dcc::accounts::User *user, const bool autoLogin);
    void requestNopasswdLogin(dcc::accounts::User *user, const bool nopasswdLogin);
    void requestDeleteAccount(dcc::accounts::User *user, const bool deleteHome);
    void requestBack();
    void requestSetAvatar(dcc::accounts::User *user, const QString &filePath);
    void requestSetFullname(dcc::accounts::User *user, const QString &fullname);
    void requestShowFingerSettings(dcc::accounts::User *user);
    void requestAddThumbs(const QString &name, const QString &thumb);
    void requestCleanThumbs(dcc::accounts::User *user);
    void requestSetGroups(dcc::accounts::User *user, const QStringList &usrGroups);
    void requsetSetPassWordAge(dcc::accounts::User *user, const int age);
    void requestDeleteFingerItem(const QString &userName, const QString& finger);
    void requestRenameFingerItem(const QString &userName, const QString& finger, const QString& newName);
    void noticeEnrollCompleted(QString username);

public Q_SLOTS:
    void resetDelButtonState();

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

private:
    dcc::accounts::User *m_curUser;
    dcc::accounts::UserModel *m_userModel;
    QLabel *m_fullName;//账户全名
    DTK_WIDGET_NAMESPACE::DIconButton *m_fullNameBtn;//账户全名编辑按钮
    DLineEdit *m_inputLineEdit;//账户全名编辑框
    dcc::accounts::FingerModel *m_model;
    FingerWidget *m_fingerWidget;//指纹界面
    AvatarListWidget *m_avatarListWidget;//图像列表
    DTK_WIDGET_NAMESPACE::DListView *m_groupListView;
    QStandardItemModel *m_groupItemModel;
    bool m_isServerSystem;
    dcc::widgets::SwitchWidget *m_autoLogin;
    dcc::widgets::SwitchWidget *m_nopasswdLogin;
    QHBoxLayout *m_avatarLayout;
    DDialog *m_tipDialog;
    DWarningButton *m_deleteAccount;
    QPushButton *m_modifyPassword;
    QGSettings *m_gsettings;
};

}   // namespace accounts
}   // namespace dccV20
