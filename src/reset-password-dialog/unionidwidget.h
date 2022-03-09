/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     wangfeia <wangfeia@uniontech.com>
 *
 * Maintainer: wangfeia <wangfeia@uniontech.com>
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

#ifndef REMINDERDDIALOG_H
#define REMINDERDDIALOG_H

#include <QStackedLayout>

#include <DDialog>
#include <DPasswordEdit>
#include <DSuggestButton>

#include "passwordwidget.h"


DWIDGET_USE_NAMESPACE


class UnionIDWidget : public QWidget
{
    Q_OBJECT

    enum UNION_ID_ERROR_TYPE {
        UNION_ID_ERROR_NO_ERR = 0,
        UNION_ID_ERROR_SYSTEM_ERROR = 7500,      // 系统错误（dbus错误或者服务端异常）
        UNION_ID_ERROR_PARA_ERROR = 7501,        // 参数错误（UOSID UBID Captcha Account等错误）
        UNION_ID_ERROR_LOGIN_EXPIRED = 7502,     // 登录过期
        UNION_ID_ERROR_NO_PERMISSION = 7503,     // 权限不足
        UNION_ID_ERROR_NETWORK_ERROR = 7506,     // 网络异常（请求不通或者其他异常）
        UNION_ID_ERROR_CONFIGURE_ERROR = 7512,   // 本地配置文件错误
        UNION_ID_ERROR_REQUEST_REACHED = 7513,   // 推送验证码过于频繁
        UNION_ID_ERROR_USER_UNBIND = 7514,       // 推送验证码的账号未绑定
    };
    enum UNION_ID_STATUS {
        UNION_ID_STATUS_CONNECTING = 0,
        UNION_ID_STATUS_CONNECT_FAILED,
        UNION_ID_STATUS_RESET_PASSWD,
        UNION_ID_STATUS_UNBIND
    };

public:
    explicit UnionIDWidget(const QString &userPath, const QString &userName, QWidget *parent = nullptr);
    ~UnionIDWidget() {}
    bool onResetPasswordBtnClicked();
    inline const QString getPassword() { return m_passwordWidget->getPassword(); }
    void loadPage();

private:
    void initWidget();
    void initData();
    int parseError(const QString& errorMsg);
    QString getErrorTips(UNION_ID_ERROR_TYPE errorType);
    bool isContentEmpty(DLineEdit *);
    bool checkPhoneEmailFormat(const QString &content);
    void startCount();
    void setIconPath(DGuiApplicationHelper::ColorType themeType);

Q_SIGNALS:
    void verifyVerficationCodeFinished(int);
    void pageChanged(bool isResetPasswordPage);
    void requestAsyncBindCheck();
    void requestAsyncVerficationCode(const QString &phoneEmail);
    void requestVerifyVerficationCode(const QString &phoneEmail, const QString &code);

public Q_SLOTS:
    void onBindCheckUbidReplied(const QString&);
    void onBindCheckReplied(int);
    void onVerficationCodeCountReplied(int);
    void onRequestVerficationCodeReplied(int ret);
    void onRequestVerifyVerficationCodeReplied(int ret);

private Q_SLOTS:
    void onPhoneEmailLineEditFocusChanged(bool);
    void onVerificationCodeBtnClicked();

private:
    QLabel *m_unBindIcon;
    QStackedLayout *m_stackedLayout;
    DLineEdit *m_phoneEmailEdit;
    DLineEdit *m_verificationCodeEdit;
    DSuggestButton *m_sendCodeBtn;
    PasswordWidget *m_passwordWidget;
    int m_count;
    QString m_userPath;
    QString m_userName;
    QString m_ubid;
    QTimer *m_codeTimer;
    bool m_verifyCodeSuccess;
    QString m_iconPath;
};

#endif // REMINDERDDIALOG_H
