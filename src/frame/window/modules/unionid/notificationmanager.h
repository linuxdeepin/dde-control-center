#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QPoint>
#include <QDBusInterface>
#include <QNetworkAccessManager>

#include "customping.h"
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

    static Notificationmanager *instance();
    // toast提示
    void showToast(QWidget *parent, ErrorType type, const QString &msg = "");

    void setWindowPosition(QPoint pos);

    QPoint getWindowPosition() const;

    bool isOnLine();

    void setUserInfo(QString usrInfo);

    QString getUserInfo();

    void getAccessToken(const QString &code, const QString &state);

    void startRefreshToken(const QString &refreshToken,int expires_in);

    QPixmap getUserAvatar();

    void setFirstLogin();

    bool firstIsLogin();

    bool isLogin();

Q_SIGNALS:
    void toTellLoginUser();

    void toTellSwitchWidget(QVariantMap);

    //发送信号到线程，重启ping
    void ProcessFinished();

public Q_SLOTS:
    void onSetNotificationStatus();
    //展示ping的结果
    void showResult();

    //返回ping的状态
    void showState(QProcess::ProcessState state);

    //展示ping的错误
    void showError();

    //2s内接收不到ping结果设置isonline
    void timeout();

    //qprocess结束重新开始
    void slots_restartProcess(int exitCode, QProcess::ExitStatus status);

    void onUserAvatar(QPixmap avatar);

private Q_SLOTS:
    void onGetAccessToken();

    void onTokenTimeout();

    void onRefreshAccessToken();

private:
    explicit Notificationmanager(QObject *parent = nullptr);

    QPoint windowPosition;
    CustomFloatingMessage *m_message;
    bool m_bIsNotificationExist;   
    QTimer *m_refreshTimer;
    QString m_userInfo;
    QString m_refreshToken;
    QPixmap m_avatar;
    bool m_bIsLogin;

    //检测网络链接的定时器
    CustomPing *m_myping;
    QTimer *m_timer_isconnect;
    bool m_isConnect = true;
    QNetworkAccessManager *m_accessManager;
    int m_timeouttime = 1500;
};

#endif // NOTIFICATIONMANAGER_H
