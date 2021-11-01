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
#include <com_deepin_daemon_accounts_user.h>

#define PASSWORD_LEVEL_ICON_NUM 3
#define PASSWORD_LEVEL_ICON_LIGHT_MODE_PATH ":/icons/dcc_deepin_password_strength_unactive_light_mode.svg"
#define PASSWORD_LEVEL_ICON_DEEP_MODE_PATH ":/icons/dcc_deepin_password_strength_unactive_deep_mode.svg"
#define PASSWORD_LEVEL_ICON_LOW_PATH ":/icons/dcc_deepin_password_strength_low.svg"
#define PASSWORD_LEVEL_ICON_MIDDLE_PATH ":/icons/dcc_deepin_password_strength_middle.svg"
#define PASSWORD_LEVEL_ICON_HIGH_PATH ":/icons/dcc_deepin_password_strength_high.svg"
#define UNREGISTERED 7500

DWIDGET_USE_NAMESPACE
DGUI_USE_NAMESPACE

class ResetPasswordDialog : public DDialog
{
    Q_OBJECT
    enum PasswordStrengthLevel {
        PASSWORD_STRENGTH_LEVEL_HIGH,
        PASSWORD_STRENGTH_LEVEL_MIDDLE,
        PASSWORD_STRENGTH_LEVEL_LOW
    };
public:
    ResetPasswordDialog() = default;
    explicit ResetPasswordDialog(QRect screenGeometry, QString uuid ="", QString app = "");
    ~ResetPasswordDialog() {}

    QRect screenGeometry() const;
    void setScreenGeometry(const QRect &screenGeometry);
    void clearInfo();
protected:
    void showEvent(QShowEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
private:
    void initWidget();
    void initData();
    int parseError(const QString& errorMsg);
    bool isContentEmpty(DLineEdit *);
    QString cryptUserPassword(const QString &password);
    bool checkPhoneEmailFormat(const QString &content);
    PasswordStrengthLevel getPasswordStrengthLevel(const QString &password);
    void updatePasswordStrengthLevelWidget();
    bool requestVerficationCode();
    bool verifyVerficationCode();
    void startCount();
    void quit();

private slots:
    void onPhoneEmailLineEditFocusChanged(bool);
    void onVerificationCodeBtnClicked();
    void onVerificationCodeLineEditFocusChanged(bool);
    void onNewPasswordLineEditFocusChanged(bool);
    void onRepeatPasswordLineEditFocusChanged(bool);
    void onPasswordTipLineEditFocusChanged(bool);
    void onResetPasswordBtnClicked();
    void onReadFromServerChanged(int);
    void startMonitor();
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
    PasswordStrengthLevel m_level;
    QString m_newPasswdLevelIconModePath;

    com::deepin::daemon::accounts::User *m_user;
    QTimer *m_timer;
    int m_count;
    QString m_ubid;
    bool m_isCodeCorrect;
    QFile filein;
    QString m_uuid;
    QTimer *m_monitorTimer;
    QString m_app;
};

class Manager : public QObject
{
    Q_OBJECT
public:
   explicit Manager(QString uuid = "", QString app = "");
    ~Manager() {}

    void start();
private Q_SLOTS:
    void setupDialog();
    void showDialog();
private:
    ResetPasswordDialog *m_dialog;
    QString m_uuid;
    QString m_app;
};

#endif // REMINDERDDIALOG_H
