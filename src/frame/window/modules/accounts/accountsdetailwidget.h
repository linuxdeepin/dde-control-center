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
#include <dimagebutton.h>

class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
class QLineEdit;

using com::deepin::daemon::fprintd::Device;

namespace DCC_NAMESPACE {
namespace accounts {



class AccountsDetailWidget : public QWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(dcc::accounts::User *user, QWidget *parent = nullptr);
    void initWidgets();
    void initDatas();
    void updateLineEditDisplayStyle();

Q_SIGNALS:
    void requestShowPwdSettings(dcc::accounts::User *user);
    void requestSetAutoLogin(dcc::accounts::User *user, const bool autoLogin);
    void requestNopasswdLogin(dcc::accounts::User *user, const bool nopasswdLogin);
    void requestDeleteAccount(dcc::accounts::User *user, const bool deleteHome);
    void requestBack();
    void requestSetAvatar(dcc::accounts::User *user, const QString &filePath);
    void requestShowFullnameSettings(dcc::accounts::User *user, const QString &fullname);

private Q_SLOTS:
    void deleteUserClicked();

private:
    dcc::accounts::User *m_curUser;
    QVBoxLayout *m_headLayout;
    QHBoxLayout *m_modifydelLayout;
    QVBoxLayout *m_setloginLayout;

    QVBoxLayout *m_mainContentLayout;
    QHBoxLayout *m_fullnameLayout;
    QHBoxLayout *m_inputlineLayout;
    dcc::accounts::AvatarWidget *m_avatar;//账户图片
    QLabel *m_shortName;
    QLabel *m_fullName;
    QLineEdit *m_inputLineEdit;
    QPushButton *m_modifyPassword;//修改密码
    QPushButton *m_deleteAccount;//删除账户

    dcc::widgets::SwitchWidget *m_autoLogin;//自动登录
    dcc::widgets::SwitchWidget *m_nopasswdLogin;//无密码登录

    AvatarListWidget *m_avatarListWidget;
    Dtk::Widget::DImageButton *m_fullnameBtn;
    Dtk::Widget::DImageButton *m_inputeditBtn;
};

}   // namespace accounts
}   // namespace dccV20

#endif // ACCOUNTSDETAILWIDGET_H__V20
