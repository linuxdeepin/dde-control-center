// SPDX-FileCopyrightText: 2021 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef RESETPASSWORDDIALOG_H
#define RESETPASSWORDDIALOG_H

#include <QLocalSocket>
#include <QLocalServer>
#include <QStackedLayout>

#include <DDialog>

#include <com_deepin_daemon_accounts_user.h>
#include <com_deepin_daemon_accounts.h>

DWIDGET_USE_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DButtonBox;
DWIDGET_END_NAMESPACE

class UnionIDWidget;
class SecurityKeyWidget;
class ResetPasswordWorker;

class ResetPasswordDialog : public DDialog
{
    Q_OBJECT

public:
    explicit ResetPasswordDialog(QRect screenGeometry, const QString &userName, const QString &appName, const int &fd);
    ~ResetPasswordDialog() {}

    QRect screenGeometry() const;
    void setScreenGeometry(const QRect &screenGeometry);
    void onCancelBtnClicked();

protected:
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent *event);
    bool eventFilter(QObject *o, QEvent *e);

private:
    void initWidget(const QString &userName);
    void initData();
    void startCount();
    void quit();
    const QString getPassword();
    void updateResetPasswordDialog();

Q_SIGNALS:
    void requestSecurityQuestions();

private:
    void onResetPasswordBtnClicked();
    void updatePosition();

public Q_SLOTS:
    void onGetSecurityQuestionsReplied(const QList<int> securityQuestions);

private Q_SLOTS:
    void onReadFromServerChanged(int);
    void onNewConnection();

private:
    DButtonBox *m_buttonBox = nullptr;
    UnionIDWidget *m_UnionIDWidget;
    bool m_isSecurityQuestionsExist;
    QRect m_screenGeometry;
    com::deepin::daemon::Accounts *m_accountInter;
    com::deepin::daemon::accounts::User *m_userInter;
    QFile filein;
    QString m_userPath;
    QString m_userName;
    QString m_appName;
    int m_fd;
    QTimer *m_monitorTimer;
    QLocalSocket *m_client, *m_dccClient;
    QLocalServer *m_localServer;
    DDialog m_tipDialog;
    bool m_isClose;
    QStackedLayout *m_stackedLayout;
    ResetPasswordWorker *m_resetPasswordWorker;
    QList<int> m_securityQuestions;
    SecurityKeyWidget *m_securityKeyWidget;
};

class Manager : public QObject
{
    Q_OBJECT
public:
   explicit Manager(const QString &userName, const QString &appName, const int &fd);
    ~Manager() {}

    void start();
    static void exit(int retCode);
private Q_SLOTS:
    void setupDialog();
    void showDialog();

private:
    static ResetPasswordDialog *m_dialog;
    QString m_usrName;
    QString m_appName;
    int m_fd;
};

#endif // ResetPasswordDialog
