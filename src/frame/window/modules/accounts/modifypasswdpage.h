// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
class QLocalServer;
class QLocalSocket;
QT_END_NAMESPACE

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
    void requestCheckPwdLimitLevel();

public Q_SLOTS:
    void onLocalBindCheckUbid(const QString &ubid);
    void onLocalBindCheckError(const QString &error);
    void onNewConnection();

private:
    dcc::accounts::User *m_curUser;
    DPasswordEdit *m_oldPasswordEdit;
    DPasswordEdit *m_newPasswordEdit;
    DPasswordEdit *m_repeatPasswordEdit;
    DCommandLinkButton *m_forgetPasswordBtn;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_passwordTipsEdit;
    bool m_isCurrent;
    QTimer m_enableBtnTimer;
    QLocalServer *m_localServer;
    QLocalSocket *m_client;
};
}
}
