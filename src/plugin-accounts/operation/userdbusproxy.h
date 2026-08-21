//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef USERDBUSPROXY_H
#define USERDBUSPROXY_H

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;

class UserDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit UserDBusProxy(QString accountsUserPath, QObject *parent = nullptr);

    Q_PROPERTY(int AccountType READ accountType NOTIFY AccountTypeChanged)
    int accountType();

    Q_PROPERTY(bool AutomaticLogin READ automaticLogin NOTIFY AutomaticLoginChanged)
    bool automaticLogin();

    Q_PROPERTY(qulonglong CreatedTime READ createdTime NOTIFY CreatedTimeChanged)
    qulonglong createdTime();

    Q_PROPERTY(QStringList DesktopBackgrounds READ desktopBackgrounds NOTIFY DesktopBackgroundsChanged)
    QStringList desktopBackgrounds();

    Q_PROPERTY(QString FullName READ fullName NOTIFY FullNameChanged)
    QString fullName();

    Q_PROPERTY(QString Gid READ gid NOTIFY GidChanged)
    QString gid();

    Q_PROPERTY(QString GreeterBackground READ greeterBackground NOTIFY GreeterBackgroundChanged)
    QString greeterBackground();

    Q_PROPERTY(QStringList Groups READ groups NOTIFY GroupsChanged)
    QStringList groups();

    Q_PROPERTY(QStringList HistoryLayout READ historyLayout NOTIFY HistoryLayoutChanged)
    QStringList historyLayout();

    Q_PROPERTY(QString HomeDir READ homeDir NOTIFY HomeDirChanged)
    QString homeDir();

    Q_PROPERTY(QString IconFile READ iconFile NOTIFY IconFileChanged)
    QString iconFile();

    Q_PROPERTY(QStringList IconList READ iconList NOTIFY IconListChanged)
    QStringList iconList();

    Q_PROPERTY(QString Layout READ layout NOTIFY LayoutChanged)
    QString layout();

    Q_PROPERTY(QString Locale READ locale NOTIFY LocaleChanged)
    QString locale();

    Q_PROPERTY(bool Locked READ locked NOTIFY LockedChanged)
    bool locked();

    Q_PROPERTY(qulonglong LoginTime READ loginTime NOTIFY LoginTimeChanged)
    qulonglong loginTime();

    Q_PROPERTY(int LongDateFormat READ longDateFormat NOTIFY LongDateFormatChanged)
    int longDateFormat();

    Q_PROPERTY(int LongTimeFormat READ longTimeFormat NOTIFY LongTimeFormatChanged)
    int longTimeFormat();

    Q_PROPERTY(int MaxPasswordAge READ maxPasswordAge NOTIFY MaxPasswordAgeChanged)
    int maxPasswordAge();

    Q_PROPERTY(bool NoPasswdLogin READ noPasswdLogin NOTIFY NoPasswdLoginChanged)
    bool noPasswdLogin();

    Q_PROPERTY(QString PasswordHint READ passwordHint NOTIFY PasswordHintChanged)
    QString passwordHint();

    Q_PROPERTY(int PasswordLastChange READ passwordLastChange NOTIFY PasswordLastChangeChanged)
    int passwordLastChange();

    Q_PROPERTY(QString PasswordStatus READ passwordStatus NOTIFY PasswordStatusChanged)
    QString passwordStatus();

    Q_PROPERTY(QString Shell READ shell NOTIFY ShellChanged)
    QString shell();

    Q_PROPERTY(int ShortDateFormat READ shortDateFormat NOTIFY ShortDateFormatChanged)
    int shortDateFormat();

    Q_PROPERTY(int ShortTimeFormat READ shortTimeFormat NOTIFY ShortTimeFormatChanged)
    int shortTimeFormat();

    Q_PROPERTY(bool SystemAccount READ systemAccount NOTIFY SystemAccountChanged)
    bool systemAccount();

    Q_PROPERTY(QString Uid READ uid NOTIFY UidChanged)
    QString uid();

    Q_PROPERTY(QString UUID READ uuid NOTIFY UUIDChanged)
    QString uuid();

    Q_PROPERTY(bool Use24HourFormat READ use24HourFormat NOTIFY Use24HourFormatChanged)
    bool use24HourFormat();

    Q_PROPERTY(QString UserName READ userName NOTIFY UserNameChanged)
    QString userName();

    Q_PROPERTY(int WeekBegins READ weekBegins NOTIFY WeekBeginsChanged)
    int weekBegins();

    Q_PROPERTY(int WeekdayFormat READ weekdayFormat NOTIFY WeekdayFormatChanged)
    int weekdayFormat();

    Q_PROPERTY(QString XSession READ xSession NOTIFY XSessionChanged)
    QString xSession();

    Q_PROPERTY(bool QuickLogin READ quickLogin NOTIFY QuickLoginChanged)
    bool quickLogin() const;

    Q_PROPERTY(bool WechatAuthEnabled READ wechatAuth NOTIFY WechatAuthChanged)
    bool wechatAuth() const;

    inline QString path() { return m_accountsUserPath; }
    inline const QDBusInterface* interface() { return m_dBusAccountsUserInter; }

signals:
    // begin property changed signals
    void AccountTypeChanged(int  value) const;
    void AutomaticLoginChanged(bool  value) const;
    void CreatedTimeChanged(qulonglong  value) const;
    void DesktopBackgroundsChanged(const QStringList & value) const;
    void FullNameChanged(const QString & value) const;
    void GidChanged(const QString & value) const;
    void GreeterBackgroundChanged(const QString & value) const;
    void GroupsChanged(const QStringList & value) const;
    void HistoryLayoutChanged(const QStringList & value) const;
    void HomeDirChanged(const QString & value) const;
    void IconFileChanged(const QString & value) const;
    void IconListChanged(const QStringList & value) const;
    void LayoutChanged(const QString & value) const;
    void LocaleChanged(const QString & value) const;
    void LockedChanged(bool  value) const;
    void LoginTimeChanged(qulonglong  value) const;
    void LongDateFormatChanged(int  value) const;
    void LongTimeFormatChanged(int  value) const;
    void MaxPasswordAgeChanged(int  value) const;
    void NoPasswdLoginChanged(bool  value) const;
    void PasswordHintChanged(const QString & value) const;
    void PasswordLastChangeChanged(int  value) const;
    void PasswordStatusChanged(const QString & value) const;
    void ShellChanged(const QString & value) const;
    void ShortDateFormatChanged(int  value) const;
    void ShortTimeFormatChanged(int  value) const;
    void SystemAccountChanged(bool  value) const;
    void UidChanged(const QString & value) const;
    void UUIDChanged(const QString & value) const;
    void Use24HourFormatChanged(bool  value) const;
    void UserNameChanged(const QString & value) const;
    void WeekBeginsChanged(int  value) const;
    void WeekdayFormatChanged(int  value) const;
    void XSessionChanged(const QString & value) const;

    void QuickLoginChanged(bool value) const;
    void WechatAuthChanged();

public slots:
    QDBusPendingReply<> AddGroup(const QString &group);
    QDBusPendingReply<> DeleteGroup(const QString &group);
    QDBusPendingReply<> DeleteIconFile(const QString &iconFile);
    QDBusPendingReply<> EnableNoPasswdLogin(bool enabled);
    QDBusPendingReply<bool> IsPasswordExpired();
    QDBusPendingReply<> SetAutomaticLogin(bool enabled);
    QDBusPendingReply<> SetDesktopBackgrounds(const QStringList &backgrounds);
    QDBusPendingReply<> SetFullName(const QString &name);
    QDBusPendingReply<> SetGreeterBackground(const QString &background);
    QDBusPendingReply<> SetGroups(const QStringList &groups);
    QDBusPendingReply<> SetHistoryLayout(const QStringList &layouts);
    QDBusPendingReply<> SetHomeDir(const QString &home);
    QDBusPendingReply<> SetIconFile(const QString &iconFile);
    QDBusPendingReply<> SetLayout(const QString &layout);
    QDBusPendingReply<> SetLocale(const QString &locale);
    QDBusPendingReply<> SetLocked(bool locked);
    QDBusPendingReply<> SetMaxPasswordAge(int nDays);
    QDBusPendingReply<> SetPassword(const QString &password);
    QDBusPendingReply<> SetPasswordHint(const QString &hint);
    QDBusPendingReply<> SetShell(const QString &shell);
    QDBusPendingReply<QList<int>> GetSecretQuestions();
    QDBusPendingReply<> SetSecretQuestions(const QMap<int, QByteArray> &securityQuestions);
    QDBusPendingReply<> SetQuickLogin(bool enabled);
    QDBusPendingReply<> EnableWechatAuth(bool enabled);

private slots:
    void onPropertiesChanged(const QDBusMessage &message);
private:
    void init();

private:
    QDBusInterface *m_dBusAccountsUserInter;
    QString m_accountsUserPath;
};

#endif // USERDBUSPROXY_H
