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

#include <DDialog>
#include <DPasswordEdit>
#include <DSuggestButton>
#include <DRegionMonitor>
#include <DSysInfo>
#include <com_deepin_daemon_accounts_user.h>
#include <com_deepin_daemon_accounts.h>

#include <QLocalSocket>

#include "deepin_pw_check.h"

#define PASSWORD_LEVEL_ICON_NUM 3
#define PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH ":/icons/dcc_deepin_password_strength_unactive_light_mode.svg"
#define PASSWORD_LEVEL_ICON_DEEP_MODE_PATH ":/icons/dcc_deepin_password_strength_unactive_deep_mode.svg"
#define PASSWORD_LEVEL_ICON_LOW_PATH ":/icons/dcc_deepin_password_strength_low.svg"
#define PASSWORD_LEVEL_ICON_MIDDLE_PATH ":/icons/dcc_deepin_password_strength_middle.svg"
#define PASSWORD_LEVEL_ICON_HIGH_PATH ":/icons/dcc_deepin_password_strength_high.svg"

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE
DCORE_USE_NAMESPACE

const DSysInfo::UosType UosType = DSysInfo::uosType();
const bool IsServerSystem = (DSysInfo::UosServer == UosType); //是否是服务器版

class PwqualityManager : public QObject
{
Q_OBJECT
public:
    typedef PW_ERROR_TYPE ERROR_TYPE;

    enum CheckType {
        Default,
        Grub2
    };

    /**
    * @brief PwqualityManager::instance 构造一个 单例
    * @return 返回一个静态实例
    */
    static PwqualityManager* instance();

    /**
    * @brief PwqualityManager::verifyPassword 校验密码
    * @param password 带检密码字符串
    * @return 若找到，返回text，反之返回空
    */
    ERROR_TYPE verifyPassword(const QString &user, const QString &password, CheckType checkType = Default);
    PASSWORD_LEVEL_TYPE GetNewPassWdLevel(const QString &newPasswd);
    QString getErrorTips(ERROR_TYPE type, CheckType checkType = Default);

private:
    PwqualityManager();
    PwqualityManager(const PwqualityManager&) = delete;

    int m_passwordMinLen;
    int m_passwordMaxLen;
};

class ResetPasswordDialog : public DDialog
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
public:
    explicit ResetPasswordDialog(QRect screenGeometry, const QString &userName, const QString &appName, const int &fd);
    ~ResetPasswordDialog() {}

    QRect screenGeometry() const;
    void setScreenGeometry(const QRect &screenGeometry);

protected:
    void showEvent(QShowEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    bool eventFilter(QObject *o, QEvent *e);

private:
    void initWidget();
    void initData();
    int parseError(const QString& errorMsg);
    QString getErrorTips(UNION_ID_ERROR_TYPE errorType);
    bool isContentEmpty(DLineEdit *);
    QString cryptUserPassword(const QString &password);
    bool checkPhoneEmailFormat(const QString &content);
    void updatePasswordStrengthLevelWidget(PASSWORD_LEVEL_TYPE level);
    int requestVerficationCode();
    int verifyVerficationCode();
    void startCount();
    void quit();

private slots:
    void onPhoneEmailLineEditFocusChanged(bool);
    void onVerificationCodeBtnClicked();
    void onNewPasswordLineEditChanged(const QString&);
    void onResetPasswordBtnClicked();
    void onReadFromServerChanged(int);
    void updatePosition();

private:
    QRect m_screenGeometry;
    DLineEdit *m_phoneEmailEdit;
    DLineEdit *m_verificationCodeEdit;
    DSuggestButton *m_sendCodeBtn;
    DPasswordEdit *m_newPasswordEdit;
    DPasswordEdit *m_repeatPasswordEdit;
    DLineEdit *m_passwordTipsEdit;
    QLabel *m_newPasswdLevelText;
    QLabel *m_newPasswdLevelIcons[PASSWORD_LEVEL_ICON_NUM];
    PASSWORD_LEVEL_TYPE m_level;
    QString m_newPasswdLevelIconModePath;
    com::deepin::daemon::Accounts *m_accountInter;
    com::deepin::daemon::accounts::User *m_userInter;
    int m_count;
    bool m_isCodeCorrect;
    QFile filein;
    QString m_user;
    QString m_userName;
    QString m_appName;
    int m_fd;
    QString m_ubid;
    QTimer *m_codeTimer;
    QTimer *m_monitorTimer;
    QLocalSocket *m_client;
    bool m_verifyCodeSuccess;
    DDialog m_tipDialog;
};

class Manager : public QObject
{
    Q_OBJECT
public:
   explicit Manager(const QString &userName, const QString &appName, const int &fd);
    ~Manager() {}

    void start();
private Q_SLOTS:
    void setupDialog();
    void showDialog();
private:
    ResetPasswordDialog *m_dialog;
    QString m_usrName;
    QString m_appName;
    int m_fd;
};

#endif // REMINDERDDIALOG_H
