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
#include "modules/accounts/user.h"
#include "widgets/lineeditwidget.h"
#include "accountswidget.h"

#include <ddialog.h>
#include <dpasswordedit.h>
#include <DSuggestButton>

#include <QWidget>

#include "com_deepin_defender_hmiscreen.h"
#include "com_deepin_defender_daemonservice.h"

using Defender = com::deepin::defender::hmiscreen;
using DaemonService = com::deepin::defender::daemonservice;

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
QT_END_NAMESPACE

namespace DCC_NAMESPACE {
namespace accounts {

//修改密码页面
class ModifyPasswdPage : public QWidget
{
    Q_OBJECT
public:
    explicit ModifyPasswdPage(dcc::accounts::User *user, QWidget *parent = nullptr);
    ~ModifyPasswdPage();
    void initWidget();
    void clickSaveBtn();
    void onPasswordChangeFinished(const int exitCode);
    QString validatePassword(const QString &password);

    enum ModifyPwdRetFlag {
        ModifyNewPwdSuccess = 0,
        InputOldPwdError  = 10
    };

protected:
    void showEvent(QShowEvent *event);

private:
    bool onPasswordEditFinished(DPasswordEdit *passwdEdit, DPasswordEdit *repeatpasswdEdit);

Q_SIGNALS:
    void requestChangePassword(dcc::accounts::User *userInter, const QString &oldPassword, const QString &password);
    void requestBack(DCC_NAMESPACE::accounts::AccountsWidget::ActionOption option = DCC_NAMESPACE::accounts::AccountsWidget::ClickCancel);

private:
    dcc::accounts::User *m_curUser;
    DTK_WIDGET_NAMESPACE::DPasswordEdit *m_oldPasswordEdit;
    DTK_WIDGET_NAMESPACE::DPasswordEdit *m_newPasswordEdit;
    DTK_WIDGET_NAMESPACE::DPasswordEdit *m_repeatPasswordEdit;
};

}
}
