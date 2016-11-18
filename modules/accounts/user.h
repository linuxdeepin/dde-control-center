#ifndef USER_H
#define USER_H

#include <QObject>
#include <QSet>
#include <QString>

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

    inline const QList<QString> avatarList() const { return m_avatarList.values(); }
    void appendAvatarList(const QList<QString> &avatars);

signals:
    void nameChanged(const QString &name) const;
    void autoLoginChanged(const bool autoLogin) const;
    void avatarListChanged(const QList<QString> &avatars) const;

private:
    bool m_autoLogin;
    QString m_name;
    QSet<QString> m_avatarList;
};

#endif // USER_H
