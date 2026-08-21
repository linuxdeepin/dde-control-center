//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef USER_H
#define USER_H

#include <QObject>
#include <QSet>
#include <QString>

static const QString NO_PASSWORD { "NP" };

namespace dccV25 {

enum SecurityLever {
    Standard,
    Sysadm,
    Secadm,
    Audadm,
    Auditadm
};

class User : public QObject
{
    Q_OBJECT

public:
    enum UserType {
        StandardUser = 0,
        Administrator,
        Customized
    };

    explicit User(QObject *parent = nullptr);

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString fullname READ fullname WRITE setFullname NOTIFY fullnameChanged)

    const inline QString id() const { return m_id; }
    void setId(const QString &id);

    const QString name() const;
    void setName(const QString &name);

    const QString fullname() const { return m_fullname; }
    void setFullname(const QString &fullname);

    inline bool autoLogin() const { return m_autoLogin; }
    void setAutoLogin(const bool autoLogin);

    inline bool quickLogin() const { return m_quickLogin; }
    void setQuickLogin(const bool quickLogin);

    inline const QList<QString> &avatars() const { return m_avatars; }
    void setAvatars(const QList<QString> &avatars);

    inline const QStringList &groups() const { return m_groups; }
    void setGroups(const QStringList &groups);

    inline const QString currentAvatar() const { return m_currentAvatar; }
    void setCurrentAvatar(const QString &avatar);

    inline QString password() const { return m_password; }
    void setPassword(const QString &password);

    inline QString repeatPassword() const { return m_repeatPassword; }
    void setRepeatPassword(const QString &repeatPassword);

    inline QString passwordHint() const { return m_passwordHint; }
    void setPasswordHint(const QString &passwordHint);

    inline bool online() const { return m_online; }
    void setOnline(bool online);

    bool nopasswdLogin() const;
    void setNopasswdLogin(bool nopasswdLogin);

    const QString displayName() const;

    inline bool isCurrentUser() const { return m_isCurrentUser; }
    void setIsCurrentUser(bool isCurrentUser);

    inline QString passwordStatus() const { return m_passwordStatus; }
    void setPasswordStatus(const QString& status);

    inline quint64 createdTime() const { return m_createdTime; }
    void setCreatedTime(const quint64 & createdtime);

    inline int userType() const { return m_userType; }
    void setUserType(const int userType);
    inline bool isPasswordExpired() const { return m_isPasswordExpired; }
    void setIsPasswordExpired(bool isExpired);

    inline int passwordAge() const { return m_pwAge; }
    void setPasswordAge(const int age);

    int charactertypes(QString password);

    inline QString gid() const { return m_gid; }
    void setGid(const QString &gid);

    SecurityLever securityLever() const;
    void setSecurityLever(const SecurityLever &securityLever);

Q_SIGNALS:
    void passwordModifyFinished(const int exitCode, const QString &errorTxt) const;
    void nameChanged(const QString &name) const;
    void fullnameChanged(const QString &name) const;
    void currentAvatarChanged(const QString &avatar) const;
    void autoLoginChanged(const bool autoLogin) const;
    void quickLoginChanged(const bool quickLogin) const;
    void avatarListChanged(const QList<QString> &avatars) const;
    void groupsChanged(const QStringList &groups) const;
    void onlineChanged(const bool &online) const;
    void nopasswdLoginChanged(const bool nopasswdLogin) const;
    void isCurrentUserChanged(bool isCurrentUser);
    void passwordStatusChanged(const QString& password) const;
    void createdTimeChanged(const quint64 & createtime);
    void userTypeChanged(const int userType);
    void isPasswordExpiredChanged(const bool isExpired) const;
    void passwordAgeChanged(const int age) const;
    void gidChanged(const QString &gid);
    void passwordResetFinished(const QString &errorText) const;
    void startResetPasswordReplied(const QString &errorText);
    void setSecurityQuestionsReplied(const QString &errorText);
    void startSecurityQuestionsCheckReplied(const QList<int> &questios);

private:
    bool m_isCurrentUser;
    bool m_autoLogin;
    bool m_quickLogin;
    bool m_online;
    bool m_nopasswdLogin;
    int m_userType;
    bool m_isPasswordExpired{false};
    int m_pwAge{-1};
    QString m_name;
    QString m_fullname;
    QString m_password;
    QString m_repeatPassword;
    QString m_currentAvatar;
    QString m_passwordStatus; // NP: no password, P have a password, L user is locked
    QList<QString> m_avatars;
    QStringList m_groups;
    quint64 m_createdTime;
    QString m_gid;
    QString m_passwordHint;
    QString m_id;
    SecurityLever m_securityLever;


    Q_ENUM(SecurityLever);
};
} // namespace dccV25

#endif // USER_H
