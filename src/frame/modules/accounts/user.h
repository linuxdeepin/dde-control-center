/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef USER_H
#define USER_H

#include <QObject>
#include <QSet>
#include <QString>

static const QString NO_PASSWORD { "NP" };

namespace dcc {
namespace accounts {

class User : public QObject
{
    Q_OBJECT

public:
    explicit User(QObject *parent = 0);

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString fullname READ fullname WRITE setFullname NOTIFY fullnameChanged)

    const QString name() const;
    void setName(const QString &name);

    const QString fullname() const { return m_fullname; }
    void setFullname(const QString &fullname);

    inline bool autoLogin() const { return m_autoLogin; }
    void setAutoLogin(const bool autoLogin);

    inline const QList<QString> &avatars() const { return m_avatars; }
    void setAvatars(const QList<QString> &avatars);

    inline const QString currentAvatar() const { return m_currentAvatar; }
    void setCurrentAvatar(const QString &avatar);

    inline QString password() const { return m_password; }
    void setPassword(const QString &password);

    inline QString repeatPassword() const { return m_repeatPassword; }
    void setRepeatPassword(const QString &repeatPassword);

    inline bool online() const { return m_online; }
    void setOnline(bool online);

    bool nopasswdLogin() const;
    void setNopasswdLogin(bool nopasswdLogin);

    const QString displayName() const;

    inline bool isCurrentUser() const { return m_isCurrentUser; }
    void setIsCurrentUser(bool isCurrentUser);

    inline QString passwordStatus() const { return m_passwordStatus; }
    void setPasswordStatus(const QString& status);

Q_SIGNALS:
    void passwordModifyFinished(const int exitCode) const;
    void nameChanged(const QString &name) const;
    void fullnameChanged(const QString &name) const;
    void currentAvatarChanged(const QString &avatar) const;
    void autoLoginChanged(const bool autoLogin) const;
    void avatarListChanged(const QList<QString> &avatars) const;
    void onlineChanged(const bool &online) const;
    void nopasswdLoginChanged(const bool nopasswdLogin) const;
    void isCurrentUserChanged(bool isCurrentUser);
    void passwordStatusChanged(const QString& password) const;

private:
    bool m_isCurrentUser;
    bool m_autoLogin;
    bool m_online;
    bool m_nopasswdLogin;
    QString m_name;
    QString m_fullname;
    QString m_password;
    QString m_repeatPassword;
    QString m_currentAvatar;
    QString m_passwordStatus; // NP: no password, P have a password, L user is locked
    QList<QString> m_avatars;
};

} // namespace accounts
} // namespace dcc

#endif // USER_H
