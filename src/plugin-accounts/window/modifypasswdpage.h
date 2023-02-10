//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#pragma once

#include "interface/namespace.h"
#include "src/plugin-accounts/operation/user.h"
#include "src/plugin-accounts/operation/usermodel.h"

#include <QTimer>

#include <DSuggestButton>
#include <DLineEdit>
#include <DCommandLinkButton>
#include <DPasswordEdit>
#include <DAbstractDialog>

DWIDGET_USE_NAMESPACE

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


namespace DCC_NAMESPACE {
class SecurityLevelItem;
}

namespace DCC_NAMESPACE {
//修改密码页面
class ModifyPasswdPage : public DAbstractDialog
{
    Q_OBJECT

public:
    explicit ModifyPasswdPage(User *user, bool isCurrent = true, QWidget *parent = nullptr);
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
    void requestChangePassword(User *userInter, const QString &oldPassword, const QString &password, const QString &repeatPassword, const bool needResule = true);
    void requestResetPassword(User *userInter, const QString &password);
    void requestBack(UserModel::ActionOption option = UserModel::ClickCancel);
    void requestSetPasswordHint(User *, const QString &);
    void requestUOSID(QString &uosid);
    void requestUUID(QString &uuid);
    void requestLocalBindCheck(User *user, const QString &uosid, const QString &uuid);
    void requestStartResetPasswordExec(User *user);
    void requestSecurityQuestionsCheck(User *user);
    void requestCheckPwdLimitLevel();

public Q_SLOTS:
    void onLocalBindCheckUbid(const QString &ubid);
    void onLocalBindCheckError(const QString &error);

private:
    User *m_curUser;
    DPasswordEdit *m_oldPasswordEdit;
    DPasswordEdit *m_newPasswordEdit;
    DPasswordEdit *m_repeatPasswordEdit;
    DCommandLinkButton *m_forgetPasswordBtn;
    DTK_WIDGET_NAMESPACE::DLineEdit *m_passwordTipsEdit;
    bool m_isCurrent;
    bool m_isBindCheckError;
    SecurityLevelItem *m_securityLevelItem;
    QTimer m_enableBtnTimer;
};
}
