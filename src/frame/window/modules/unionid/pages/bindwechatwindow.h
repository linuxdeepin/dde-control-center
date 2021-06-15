#ifndef BINDWECHATWINDOW_H
#define BINDWECHATWINDOW_H
#include <DAbstractDialog>

#include <QStackedLayout>
#include "uqrwidget.h"
#include "avatarwidget.h"

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::unionid;

class BindWeChatWindow : public DAbstractDialog
{
    Q_OBJECT
public:
    static BindWeChatWindow* instance();

    void setData(QString accessToken,QString hardwareID,QString weChatUnionId,QString userAvatar,QString nickName);

Q_SIGNALS:
    void toTellrefreshUserInfo();

    void toTellClose();

public Q_SLOTS:
    void onRequestQrCodeResult();

    void onConnectStatusTimeOut();

protected:
    void closeEvent(QCloseEvent *event);

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

    void onCloseReportQrCodeStatus();
private:
    BindWeChatWindow(QWidget *prarent = nullptr);

private:
    UQrFrame *m_qrCode;
    QWidget *m_qrCodeWidget;
    DCC_NAMESPACE::unionid::AvatarWidget *m_avatar;
    QWidget *m_avatarWidget;
    QStackedLayout *m_indexLayout;
    QTimer *m_queryTimer;
    QTimer *m_qrCodeStatusTimer;
    QTimer *m_connectStatusTimer;
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
    QString m_nickName;
    QString m_userAvatar;
};

#endif // BINDWECHATWINDOW_H
