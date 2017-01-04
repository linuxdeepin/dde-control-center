#ifndef USER_H
#define USER_H

#include <QObject>
#include <QSet>
#include <QString>

namespace dcc {
namespace accounts {

class User : public QObject
{
    Q_OBJECT

public:
    explicit User(QObject *parent = 0);

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    const QString name() const;
    void setName(const QString &name);

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

signals:
    void nameChanged(const QString &name) const;
    void currentAvatarChanged(const QString &avatar) const;
    void autoLoginChanged(const bool autoLogin) const;
    void avatarListChanged(const QList<QString> &avatars) const;
    void onlineChanged(const bool &online) const;

private:
    bool m_autoLogin;
    bool m_online;
    QString m_name;
    QString m_password;
    QString m_repeatPassword;
    QString m_currentAvatar;
    QList<QString> m_avatars;
};

} // namespace accounts
} // namespace dcc

#endif // USER_H
