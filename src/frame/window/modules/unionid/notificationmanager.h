#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QPoint>
#include <QDBusInterface>

#include <window/modules/unionid/pages/customfloatingmessage.h>
#include "modules/unionid/unionidworker.h"

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

    QPixmap getUserAvatar();

Q_SIGNALS:
    void toTellLoginUser();

public Q_SLOTS:
    void onUserAvatar(QPixmap avatar);

private Q_SLOTS:
    void onGetAccessToken();

    void onTokenTimeout();

    void onRefreshAccessToken();

private:
    Q_DISABLE_COPY(Notificationmanager)
    dcc::unionid::UnionidWorker *m_worker;
    dcc::unionid::UnionidModel  *m_model;
    QPoint windowPosition;
    CustomFloatingMessage *m_message;
    bool m_bIsNotificationExist;   
    QTimer *m_refreshTimer;
    QString m_userInfo;
    QString m_refreshToken;
    QPixmap m_avatar;
};

#endif // NOTIFICATIONMANAGER_H
