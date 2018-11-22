/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef ACCOUNTSDETAILWIDGET_H
#define ACCOUNTSDETAILWIDGET_H

#include "widgets/contentwidget.h"
#include "user.h"
#include "widgets/settingsgroup.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"

#include <com_deepin_daemon_fprintd_device.h>
#include <QPushButton>

using com::deepin::daemon::fprintd::Device;

namespace dcc {
namespace accounts {
class FingerModel;
class AccountsDetailWidget : public ContentWidget
{
    Q_OBJECT

public:
    explicit AccountsDetailWidget(User *user, QWidget *parent = 0);
    void setFingerModel(FingerModel *model);

Q_SIGNALS:
    void showPwdSettings(User *user) const;
    void showAvatarSettings(User *user) const;
    void showFullnameSettings(User *user) const;
    void showFingerSettings(User *user) const;

    void requestDeleteAccount(User *user, const bool deleteHome) const;
    void requestSetAutoLogin(User *user, const bool autoLogin) const;

    void requestChangeFrameAutoHide(const bool autoHide) const;
    void requestNopasswdLogin(User *user, const bool nopasswdLogin) const;

private Q_SLOTS:
    void deleteUserClicked();

private:
    User *m_user;
    dcc::widgets::SettingsGroup *m_accountSettings;
    dcc::widgets::NextPageWidget *m_modifyAvatar;
    dcc::widgets::NextPageWidget *m_modifyFullname;
    dcc::widgets::NextPageWidget *m_modifyPassword;
    dcc::widgets::SwitchWidget *m_autoLogin;
    dcc::widgets::SwitchWidget *m_nopasswdLogin;
    dcc::widgets::NextPageWidget *m_finger;
    QPushButton *m_deleteAccount;
};

}   // namespace accounts
}   // namespace dcc

#endif // ACCOUNTSDETAILWIDGET_H
