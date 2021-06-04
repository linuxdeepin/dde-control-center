#ifndef NOTIFICATIONMANAGER_H
#define NOTIFICATIONMANAGER_H

#include <QObject>
#include <QPoint>
#include <QDBusInterface>
#include <QNetworkAccessManager>

#include "customping.h"
#include "threadobj.h"
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

    ~Notificationmanager() override;

    static Notificationmanager *instance();
    // toast提示
    void showToast(QWidget *parent, ErrorType type, const QString &msg = "");

    // 记录窗口位置
    void setWindowPosition(QPoint pos);

    // 得到窗口位置
    QPoint getWindowPosition() const;

    // 获得网络连接状态
    bool isOnLine();

    // 记录已登录用户信息
    void setUserInfo(QString usrInfo);

    // 得到已登录用户信息
    QString getUserInfo();

    // 记录用户微信昵称
    void setWeChatName(QString weChatName);

    // 得到用户微信昵称
    QString getWeChatName();

    // 获取AT
    void getAccessToken(const QString &code, const QString &state);

    // 定时刷新token
    void startRefreshToken(const QString &refreshToken,int expires_in);

    // 获得用户头像
    QPixmap getUserAvatar();

    // 设置启动后首次登录标志
    void setFirstLogin();

    // 获得启动后首次登录标志
    bool firstIsLogin();

    // 获得登录状态
    bool isLogin();

    // 记录登录类型（外部应用或自身登录）
    void setLoginType(bool type);

    // 是否外部登录
    bool bIsExternalLogin();

    void setUidWidgetIsExist(bool bIsExists);

    bool uidWidgetIsExist();

Q_SIGNALS:
    // 通知登录的信号
    void toTellLoginUser();

    // 通知切换界面的信号
    void toTellSwitchWidget(QVariantMap);

    //发送信号到线程，重启ping
    void ProcessFinished();

    //通知获取AT完成的信号
    void toTellGetATFinished(bool);

    void toTellLoginUserFinished();

    void toTellLogoutUser(QString,QString);

    void toTellGetAccessToken(const QString&);

    void toTellGetPictureFromUrl(const QString&);

    void toTellBindAccountInfo(const int&, const int&, const QString&);

    void toTellRefreshAccessToken(const QString&, const QString&);

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

    // 获得用户头像的槽
    void onUserAvatar(QPixmap avatar);

    void onUserInfoChanged(const QVariantMap &userInfo);

    // 获得AT,RT和用户信息的槽
    void onGetAccessToken(QString result);

    // 刷新获得AT,RT和用户信息的槽
    void onRefreshAccessToken(QString result);

    void onGetBindAccountInfo(QString result);

    void readAvatarFromUrl(QByteArray result);

private Q_SLOTS:
    // 定时刷新AT,RT和用户信息的槽
    void onTokenTimeout();

private:
    explicit Notificationmanager(QObject *parent = nullptr);

    QPoint windowPosition;///< 记录的窗口位置
    CustomFloatingMessage *m_message;///< toast提示控件
    bool m_bIsNotificationExist;///< toast提示控件存在状态
    QTimer *m_refreshTimer;///< 刷新AT,RT和用户信息的定时器
    QString m_userInfo;///< 记录的用户信息
    QString m_nickName;///< 记录的用户昵称
    QString m_weChatName;///< 记录的用户微信昵称
    QString m_refreshToken;///< 记录的RT
    QPixmap m_avatar;///< 记录的用户头像
    QString m_requrstAvatar;///< 请求的用户头像
    bool m_bIsFirstLogin;///< 登录状态
    bool m_bIsExternalLogin;///< 是否外部登录
    QString m_code;///< 返回的code
    QString m_weChatUnionId;///< 微信ID
    int m_requrestCount;///< 请求计数
    bool m_uidWidgetIsExist;///< uid界面是否存在

    //检测网络链接的定时器
    CustomPing *m_myping;///< 检测网络连接状态的线程
    QTimer *m_timer_isconnect;///< 检测网络连接超时的定时器
    bool m_isConnect = true;///< 网络连接状态
    int m_timeouttime = 1500;///< 网络连接超时限制时间

    ThreadObj *m_threadobj;
    QThread *m_thread;
};

#endif // NOTIFICATIONMANAGER_H
