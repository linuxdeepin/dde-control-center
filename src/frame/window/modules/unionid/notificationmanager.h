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

    void getAccessToken(const QString &code, const QString &state);

    void startRefreshToken(const QString &refreshToken,int expires_in);

private Q_SLOTS:
    void onGetAccessToken();

    void onTokenTimeout();

    void onRefreshAccessToken();

private:
    Q_DISABLE_COPY(Notificationmanager)
    QPoint windowPosition;
    CustomFloatingMessage *m_message;
    bool m_bIsNotificationExist;   
    QTimer *m_refreshTimer;
    QString m_userInfo;
    QString m_refreshToken;
};

#endif // NOTIFICATIONMANAGER_H
