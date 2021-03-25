#include "bindwechatwindow.h"
#include "modules/unionid/requestservice.h"
#include "window/modules/unionid/define.h"
#include <DTitlebar>
#include <QLabel>
#include <QVBoxLayout>
#include <DSuggestButton>
#include <QGraphicsView>
#include <QScrollArea>
#include <DFontSizeManager>
//#include <QNetworkReply>

const QColor windowColor = QColor::fromRgbF(0,0,0,0.1);
BindWeChatWindow::BindWeChatWindow(DMainWindow *prarent)
    : DMainWindow(prarent)
{
    QGraphicsView *widget = new QGraphicsView;
    widget->setFrameShape(QFrame::NoFrame);
    QColor backgroundColor(255,255,255);
    backgroundColor.setAlphaF(0.9);
    QPalette pa = widget->palette();
    pa.setColor(QPalette::Window,backgroundColor);
    auto flag = windowFlags();
    flag &= ~Qt::WindowMinMaxButtonsHint;
    flag |= Qt::WindowStaysOnTopHint;
    //flag |= Qt::FramelessWindowHint;//特效模式下，设置无边框的时候会让登陆客户端第一次打开的时候显示直角
    setWindowFlags(flag);
    this->titlebar()->setMenuVisible(false);
    this->titlebar()->setMenuDisabled(true);
    // TODO: workaround for old version dtk, remove as soon as possible.
    this->titlebar()->setDisableFlags(Qt::WindowSystemMenuHint);
    this->titlebar()->setBackgroundTransparent(true);
    widget->setAutoFillBackground(true);
    widget->setPalette(pa);
    QPalette titlePa = titlebar()->palette();
    titlePa.setColor(QPalette::Text,backgroundColor);
    titlebar()->setPalette(titlePa);

    QLabel *titleLabel= new QLabel(QObject::tr("Link to WeChat"));
    titleLabel->setContentsMargins(0,0,0,72);

    m_qrCode = new UQrFrame;
    m_qrCode->setMinimumSize(QSize(160,160));
    connect(m_qrCode,&UQrFrame::refreshsignal,this,&BindWeChatWindow::onRefreshQrCode);

    m_tipLabel = new QLabel(QObject::tr("Scan and log in by WeChat to get linked"));
    m_tipLabel->setContentsMargins(0,16,0,0);
    m_tipLabel->setAlignment(Qt::AlignCenter);

    QVBoxLayout *qrCodeLayout = new QVBoxLayout;
    qrCodeLayout->addWidget(m_qrCode,0,Qt::AlignCenter);
    qrCodeLayout->addWidget(m_tipLabel);

    m_qrCodeWidget = new QWidget;
    m_qrCodeWidget->setLayout(qrCodeLayout);

    m_avatar = new AvatarWidget;
    m_avatar->setFixedSize(QSize(104,104));
    m_avatar->setAvatarPath(":/images/desktop.jpg");
    m_avatar->setContentsMargins(0,0,0,0);

    m_nameLabel = new QLabel(QObject::tr("123456"));
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setContentsMargins(0,8,0,0);

    m_secTipLabel = new QLabel(QObject::tr("Linking successful"));
    m_secTipLabel->setAlignment(Qt::AlignCenter);
    m_secTipLabel->setContentsMargins(0,50,0,0);
    m_secTipLabel->setVisible(false);

    m_resultTipLabel = new QLabel(QObject::tr("Confirm your login on WeChat"));
    m_resultTipLabel->setAlignment(Qt::AlignCenter);
    m_resultTipLabel->setContentsMargins(0,0,0,0);

    QVBoxLayout *avatarLayout = new QVBoxLayout;
    avatarLayout->addWidget(m_avatar,0,Qt::AlignCenter);
    avatarLayout->addWidget(m_nameLabel,0,Qt::AlignCenter);
    avatarLayout->addWidget(m_secTipLabel,0,Qt::AlignCenter);
    avatarLayout->addWidget(m_resultTipLabel,0,Qt::AlignCenter);
    avatarLayout->setContentsMargins(0,0,0,0);

    m_avatarWidget = new QWidget;
    m_avatarWidget->setLayout(avatarLayout);

    m_indexLayout = new QStackedLayout;
    m_indexLayout->addWidget(m_qrCodeWidget);
    m_indexLayout->addWidget(m_avatarWidget);
    m_indexLayout->setCurrentWidget(m_qrCodeWidget);

    QWidget *indexWidget = new QWidget;
    indexWidget->setLayout(m_indexLayout);

    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T4,QFont::Bold);
    DFontSizeManager::instance()->bind(m_tipLabel, DFontSizeManager::T6,QFont::Normal);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(titleLabel,0,Qt::AlignCenter);
    vlayout->addWidget(indexWidget,0,Qt::AlignCenter);
    vlayout->addStretch();
    vlayout->setContentsMargins(0,0,0,0);

    widget->setLayout(vlayout);
    setCentralWidget(widget);
    setMinimumSize(376,516);
    setWindowModality(Qt::WindowModal);

    m_qrCodeStatusTimer = new QTimer;
    connect(m_qrCodeStatusTimer,&QTimer::timeout,this,&BindWeChatWindow::onQrCodeStatusTimeOut);
    m_qrCodeStatusTimer->start(1000*60*2);

    m_queryTimer = new QTimer;
    connect(m_queryTimer,&QTimer::timeout,this,&BindWeChatWindow::onQueryTimeOut);
}

void BindWeChatWindow::setData(QString accessToken,QString hardwareID,QString weChatUnionId,QString refreshToken)
{
    m_accessToken = accessToken;
    m_hardwareID = hardwareID;
    m_weChatUnionId = weChatUnionId;
    m_refreshToken = refreshToken;
}

void BindWeChatWindow::onRequestQrCodeResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = RequestService::instance()->checkReply(reply);

    qInfo() << "onRequestQrCodeResult" << RequestService::instance()->solveJson(result);
    if (RequestService::instance()->solveJson(result)) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("data");

        qInfo() << "onRequestQrCodeResult22222" << jsonValueResult.isObject();
        if (jsonValueResult.isObject())
        {
            QJsonObject jsonObj1 = jsonValueResult.toObject();
            jsonValueResult = jsonObj1.value("url");
            qInfo() << "url" << jsonValueResult;
            QString nResult = jsonValueResult.toString();
            jsonValueResult = jsonObj1.value("codeId");
            m_codeId = jsonValueResult.toString();
            qInfo() << "codeId" << jsonValueResult;
            m_qrCode->showQRcodePicture(nResult + m_codeId);
            qInfo() << "showQRcodePicture" << nResult + m_codeId;
            m_qrCodeStatusTimer->start(1000*60*2);
            m_qrCode->setWidgetType(NormalScanCode);
            m_queryTimer->start(1000);
        }
    } else {
        qInfo() << "failed";
    }
}

void BindWeChatWindow::onQrCodeStatusTimeOut()
{
    m_qrCodeStatusTimer->stop();
    m_tipLabel->setText(QObject::tr("Invalid QR code,click to refresh"));
    m_qrCode->setWidgetType(RefreshScanCode);
}

void BindWeChatWindow::onQueryTimeOut()
{
    QNetworkReply *reply = RequestService::instance()->getQrCodeStatus(m_codeId);
    connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onQrCodeStatusResult);
}

void BindWeChatWindow::onScanSuccess()
{
    m_indexLayout->setCurrentWidget(m_avatarWidget);
}

void BindWeChatWindow::onRefreshQrCode()
{
    QNetworkReply *reply1 = RequestService::instance()->requestQrCode(CLIENT_ID,m_hardwareID,3,REDIRECT_URI);
    connect(reply1,&QNetworkReply::finished,this,&BindWeChatWindow::onRequestQrCodeResult);
}

void BindWeChatWindow::onQrCodeStatusResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = RequestService::instance()->checkReply(reply);

    if (RequestService::instance()->solveJson(result)) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("data");

        if (jsonValueResult.isObject())
        {
            jsonObj = jsonValueResult.toObject();
            jsonValueResult = jsonObj.value("scanner");

            if (jsonValueResult.isObject())
            {
                jsonObj = jsonValueResult.toObject();
                jsonValueResult = jsonObj.value("qrCodeStatus");
                int nResult = jsonValueResult.toInt();

                if (nResult == 2) {
                    m_queryTimer->stop();
                    jsonValueResult = jsonObj.value("data");

                    if (jsonValueResult.isObject()){
                        jsonObj = jsonValueResult.toObject();
                        jsonValueResult = jsonObj.value("sessionId");
                        m_sessionId = jsonValueResult.toString();

                        QNetworkReply *reply = RequestService::instance()->unbindAccount(0,1,m_accessToken,1,0,m_weChatUnionId);
                        connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onUnbindAccountResult);
                    }
                } else if (nResult == 15) {
                    m_queryTimer->stop();
                    m_windowStatus = 1;

                } else if (nResult == 1) {
                    m_qrCodeStatusTimer->stop();
                    m_windowStatus = 1;
                    m_indexLayout->setCurrentWidget(m_avatarWidget);
                    m_secTipLabel->setVisible(false);
                    m_resultTipLabel->setText(QObject::tr("Confirm your login on WeChat"));
                }
            }
        }
    }
}

void BindWeChatWindow::onReportStatusResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = RequestService::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        m_indexLayout->setCurrentWidget(m_avatarWidget);
        m_secTipLabel->setVisible(true);
        m_resultTipLabel->setText(QObject::tr("Go to \"Accounts\" and switch on \"Login by Union ID\""));

        QNetworkReply *reply = RequestService::instance()->refreshAccessToken(CLIENT_ID,m_refreshToken);
        connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onRefreshAccessToken);
    }
}

void BindWeChatWindow::onGetUserInfoResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = RequestService::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("phone_number");
    }
}

void BindWeChatWindow::onBindAccountResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = RequestService::instance()->checkReply(reply);
    reply->deleteLater();

    if (RequestService::instance()->solveJson(result)) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("msg");

        if (jsonValueResult == "success") {
            QNetworkReply *reply = RequestService::instance()->reportQrCodeStatus(m_codeId, 2);
            connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onReportStatusResult);
        }
    }
}

void BindWeChatWindow::onUnbindAccountResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = RequestService::instance()->checkReply(reply);
    reply->deleteLater();

    if (RequestService::instance()->solveJson(result)) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("msg");

        if (jsonValueResult == "success") {
            QNetworkReply *reply = RequestService::instance()->bindAccount(0,1,m_accessToken,1,2,m_sessionId);
            connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onBindAccountResult);
        }
    }
}

void BindWeChatWindow::onRefreshAccessToken()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = RequestService::instance()->checkReply(reply);
    reply->deleteLater();

    if (RequestService::instance()->solveJson(result)) {
        Q_EMIT toTellrefreshUserInfo(result);
    }
}
