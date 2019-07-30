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

#ifndef ACCOUNTSDETAILWIDGET_H__V20
#define ACCOUNTSDETAILWIDGET_H__V20

#include "window/namespace.h"
#include "widgets/contentwidget.h"
#include "modules/accounts/user.h"
#include "modules/accounts/avatarwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "avatarlistwidget.h"

#include <com_deepin_daemon_fprintd_device.h>
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QEvent;

using namespace dcc::accounts;
using namespace dcc::widgets;
using com::deepin::daemon::fprintd::Device;

namespace DCC_NAMESPACE {
namespace accounts {



class AccountsDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(User *user, QWidget *parent = nullptr);
    void initWidgets();
    void initDatas();


Q_SIGNALS:
    void requestShowPwdSettings(User *user);
    void requestSetAutoLogin(User *user, const bool autoLogin);
    void requestNopasswdLogin(User *user, const bool nopasswdLogin);
    void requestDeleteAccount(User *user, const bool deleteHome);
    void requestChangeFrameAutoHide();
    void requestSetAvatar(User *user, const QString &filePath);

private Q_SLOTS:
    void deleteUserClicked();

private:
    User *m_curUser;
    QVBoxLayout *m_headLayout;
    QHBoxLayout *m_modifydelLayout;
    QVBoxLayout *m_setloginLayout;

    QVBoxLayout *m_mainContentLayout;

    AvatarWidget *m_avatar;//账户图片
    QLabel *m_shortName;
    QLabel *m_fullName;

    QPushButton *m_modifyPassword;//修改密码
    QPushButton *m_deleteAccount;//删除账户

    SwitchWidget *m_autoLogin;//自动登录
    SwitchWidget *m_nopasswdLogin;//无密码登录

    AvatarListWidget *m_avatarListWidget;
};

}   // namespace accounts
}   // namespace dccV20

#endif // ACCOUNTSDETAILWIDGET_H__V20
