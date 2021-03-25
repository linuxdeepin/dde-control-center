#ifndef BINDWECHATWINDOW_H
#define BINDWECHATWINDOW_H
#include <DMainWindow>
#include <QStackedLayout>
#include "uqrwidget.h"
#include "avatarwidget.h"

using namespace DCC_NAMESPACE::unionid;
DWIDGET_USE_NAMESPACE

class BindWeChatWindow : public DMainWindow
{
    Q_OBJECT
public:
    BindWeChatWindow(DMainWindow *prarent = nullptr);

    void setData(QString accessToken,QString hardwareID,QString weChatUnionId,QString refreshToken);

Q_SIGNALS:
    void toTellrefreshUserInfo(QString);

public Q_SLOTS:
    void onRequestQrCodeResult();

private Q_SLOTS:
    void onQrCodeStatusTimeOut();

    void onQueryTimeOut();

    void onScanSuccess();

    void onRefreshQrCode();

    void onQrCodeStatusResult();

    void onReportStatusResult();

    void onGetUserInfoResult();

    void onBindAccountResult();

    void onUnbindAccountResult();

    void onRefreshAccessToken();

private:
    UQrFrame *m_qrCode;
    QWidget *m_qrCodeWidget;
    AvatarWidget *m_avatar;
    QWidget *m_avatarWidget;
    QStackedLayout *m_indexLayout;
    QTimer *m_queryTimer;
    QTimer *m_qrCodeStatusTimer;
    QLabel *m_tipLabel;
    QLabel *m_nameLabel;
    QLabel *m_secTipLabel;
    QLabel *m_resultTipLabel;
    QString m_codeId;
    int m_windowStatus;
    QString m_accessToken;
    QString m_hardwareID;
    QString m_weChatUnionId;
    QString m_sessionId;
    QString m_refreshToken;
};

#endif // BINDWECHATWINDOW_H
