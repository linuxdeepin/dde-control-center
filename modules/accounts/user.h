#ifndef USER_H
#define USER_H

#include <QObject>

class User : public QObject
{
    Q_OBJECT

public:
    explicit User(QObject *parent = 0);

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)

    QString name() const;
    void setName(const QString &name);

signals:
    void nameChanged(const QString &name);

private:
    QString m_name;
};

#endif // USER_H
