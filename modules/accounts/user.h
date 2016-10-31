#ifndef USER_H
#define USER_H

#include <QObject>

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

signals:
    void nameChanged(const QString &name) const;
    void autoLoginChanged(const bool autoLogin) const;

private:
    bool m_autoLogin;
    QString m_name;
};

#endif // USER_H
