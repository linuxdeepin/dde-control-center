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
#include "accountswidget.h"

#include <DSuggestButton>
#include <DLineEdit>
#include <DCommandLinkButton>
#include <DPasswordEdit>

#include "com_deepin_defender_hmiscreen.h"
#include "com_deepin_defender_daemonservice.h"

DWIDGET_USE_NAMESPACE

using Defender = com::deepin::defender::hmiscreen;
using DaemonService = com::deepin::defender::daemonservice;
#define PASSWORD_LEVEL_ICON_NUM 3
#define PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH ":/accounts/themes/common/icons/dcc_deepin_password_strength_unactive_light_mode.svg"
#define PASSWORD_LEVEL_ICON_DEEP_MODE_PATH ":/accounts/themes/common/icons/dcc_deepin_password_strength_unactive_deep_mode.svg"
#define PASSWORD_LEVEL_ICON_LOW_PATH ":/accounts/themes/common/icons/dcc_deepin_password_strength_low.svg"
#define PASSWORD_LEVEL_ICON_MIDDLE_PATH ":/accounts/themes/common/icons/dcc_deepin_password_strength_middle.svg"
#define PASSWORD_LEVEL_ICON_HIGH_PATH ":/accounts/themes/common/icons/dcc_deepin_password_strength_high.svg"
QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QHBoxLayout;
class QPushButton;
class QLabel;
QT_END_NAMESPACE


namespace dcc {
namespace widgets {
class SecurityLevelItem;
}
}

namespace DCC_NAMESPACE {
namespace accounts {
//修改密码页面
class ModifyPasswdPage : public QWidget
{
    Q_OBJECT

public:
    explicit ModifyPasswdPage(dcc::accounts::User *user, bool isCurrent = true, QWidget *parent = nullptr);
    ~ModifyPasswdPage();
    void initWidget();
    bool judgeTextEmpty(DPasswordEdit *edit);
    void clickSaveBtn();
    void onPasswordChangeFinished(const int exitCode, const QString &errorTxt);
    void setPasswordEditAttribute(DPasswordEdit *);
    void resetPassword(const QString &password, const QString &repeatPassword);

protected:
    void showEvent(QShowEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
private:
    void resetPasswordFinished(const QString &errorText);
    void onForgetPasswordBtnClicked();
    void onStartResetPasswordReplied(const QString &errorText);
    void onSecurityQuestionsCheckReplied(const QList<int> &questions);

Q_SIGNALS:
    void requestChangePassword(dcc::accounts::User *userInter, const QString &oldPassword, const QString &password, const QString &repeatPassword, const bool needResule = true);
    void requestResetPassword(dcc::accounts::User *userInter, const QString &password);
    void requestBack(DCC_NAMESPACE::accounts::AccountsWidget::ActionOption option = DCC_NAMESPACE::accounts::AccountsWidget::ClickCancel);
    void requestSetPasswordHint(dcc::accounts::User *, const QString &);
    void requestUOSID(QString &uosid);
    void requestUUID(QString &uuid);
    void requestLocalBindCheck(dcc::accounts::User *user, const QString &uosid, const QString &uuid);
    void requestStartResetPasswordExec(dcc::accounts::User *user);
    void requestSecurityQuestionsCheck(dcc::accounts::User *user);

public Q_SLOTS:
    void onLocalBindCheckUbid(const QString &ubid);
    void onLocalBindCheckError(const QString &error);

private:
    dcc::accounts::User *m_curUser;
    DPasswordEdit *m_oldPasswordEdit;
    DPasswordEdit *m_newPasswordEdit;
    DPasswordEdit *m_repeatPasswordEdit;
    DCommandLinkButton *m_forgetPasswordBtn;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_passwordTipsEdit;
    bool m_isCurrent;
    bool m_isBindCheckError;
    bool m_isSecurityQuestionsExist;
    dcc::widgets::SecurityLevelItem *m_securityLevelItem;
    QTimer m_enableBtnTimer;
};
}
}
