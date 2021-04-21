#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QPoint>
#include <QDBusInterface>

#include <window/modules/unionid/pages/customfloatingmessage.h>

DWIDGET_USE_NAMESPACE

class Notificationmanager : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QPoint windowPosition READ getWindowPosition WRITE setWindowPosition)

public:
    enum ErrorType {
        NetworkError = 0,
        ConnectionError,
        ConnectionTimeout,
        SystemError
    };

    explicit Notificationmanager(QObject *parent = nullptr);

    static Notificationmanager *instance();
    // toast提示
    void showToast(QWidget *parent, ErrorType type);

    void setWindowPosition(QPoint pos);

    QPoint getWindowPosition() const;

    bool isOnLine();

    void setNotificationStatus();

    void setUserInfo(QString usrInfo);

    QString getUserInfo();

//public Q_SLOTS:
//    void networkInfoChanged(QDBusMessage message);

private:
    Q_DISABLE_COPY(Notificationmanager)
    QPoint windowPosition;
    CustomFloatingMessage *m_message;
    bool m_bIsNotificationExist;
    QString m_userInfo;
};

#endif // NOTIFICATIONMANAGER_H
