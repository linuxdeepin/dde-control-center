#ifndef THREADOBJ_H
#define THREADOBJ_H

#include <QObject>

class ThreadObj : public QObject
{
    Q_OBJECT
public:
    explicit ThreadObj(QObject *parent = nullptr);

Q_SIGNALS:
    void toTellLoginUserFinished();

    void toTellLogoutUserFinished();

public Q_SLOTS:
    void onLoginUser();

    void onLogoutUser(QString qstrAvatar, QString qstrNickName);
};

#endif // THREADOBJ_H
