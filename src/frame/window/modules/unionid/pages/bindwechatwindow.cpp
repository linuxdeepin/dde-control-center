#include "bindwechatwindow.h"
#include "modules/unionid/httpclient.h"
#include "window/modules/unionid/define.h"
#include "customwidget.h"

#include <DTitlebar>
#include <DSuggestButton>
#include <DFontSizeManager>
#include <DWindowCloseButton>

#include <QLabel>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <QScrollArea>
//#include <QNetworkReply>
const QColor windowColor = QColor::fromRgbF(0,0,0,0.1);
BindWeChatWindow::BindWeChatWindow(QWidget *prarent)
    : DAbstractDialog(prarent)
{
    setAttribute(Qt::WA_DeleteOnClose);
    DWindowCloseButton *closeButton = new DWindowCloseButton;
    closeButton->setFixedHeight(48);
    closeButton->setIconSize(QSize(48,48));
    closeButton->setContentsMargins(0,0,0,0);
    connect(closeButton,&DWindowCloseButton::clicked,this,&BindWeChatWindow::close);

    QHBoxLayout *titberLayout = new QHBoxLayout;
    titberLayout->addStretch();
    titberLayout->addWidget(closeButton);
    titberLayout->setContentsMargins(0,0,0,0);

    QFont font("NotoSansCJKsc");
    font.setPixelSize(24);
    font.setWeight(QFont::Medium);
    QLabel *titleLabel= new QLabel(QObject::tr("Link to WeChat"));
    titleLabel->setContentsMargins(0,13,0,41);
    titleLabel->setFont(font);

    m_qrCode = new UQrFrame;
    m_qrCode->setMinimumSize(QSize(176,176));
    m_qrCode->showQRcodePicture("");
    m_qrCode->setWidgetType(RefreshScanCode);
    m_qrCode->setContentsMargins(0,0,0,0);
    connect(m_qrCode,&UQrFrame::refreshsignal,this,&BindWeChatWindow::onRefreshQrCode);

    font.setPixelSize(15);
    font.setWeight(QFont::Normal);
    m_tipLabel = new QLabel(QObject::tr("Scan and log in by WeChat to get linked"));
    m_tipLabel->setContentsMargins(0,10,0,0);
    m_tipLabel->setAlignment(Qt::AlignCenter);
    m_tipLabel->setFont(font);

    QVBoxLayout *qrCodeLayout = new QVBoxLayout;
    qrCodeLayout->addWidget(m_qrCode,0,Qt::AlignCenter);
    qrCodeLayout->addWidget(m_tipLabel);
    qrCodeLayout->setContentsMargins(0,0,0,0);

    m_qrCodeWidget = new QWidget;
    m_qrCodeWidget->setLayout(qrCodeLayout);
    m_qrCodeWidget->setContentsMargins(0,0,0,0);

    m_avatar = new AvatarWidget;
    m_avatar->setFixedSize(QSize(104,104));
    m_avatar->setAvatarPath(m_userAvatar,true);
    m_avatar->setContentsMargins(0,0,0,0);

    font.setWeight(QFont::Medium);
    m_nameLabel = new QLabel(m_nickName);
    m_nameLabel->setAlignment(Qt::AlignCenter);
    m_nameLabel->setContentsMargins(0,8,0,0);
    m_nameLabel->setFont(font);

    font.setWeight(QFont::Normal);
    m_secTipLabel = new QLabel(QObject::tr("Linking successful"));
    m_secTipLabel->setAlignment(Qt::AlignCenter);
    m_secTipLabel->setContentsMargins(0,50,0,0);
    m_secTipLabel->setVisible(false);
    m_secTipLabel->setFont(font);

    m_resultTipLabel = new QLabel(QObject::tr("Confirm your login on WeChat"));
    m_resultTipLabel->setAlignment(Qt::AlignCenter);
    m_resultTipLabel->setContentsMargins(0,0,0,0);
    m_resultTipLabel->setFont(font);

    QVBoxLayout *avatarLayout = new QVBoxLayout;
    avatarLayout->addWidget(m_avatar,0,Qt::AlignCenter);
    avatarLayout->addWidget(m_nameLabel,0,Qt::AlignCenter);
    avatarLayout->addWidget(m_secTipLabel,0,Qt::AlignCenter);
    avatarLayout->addWidget(m_resultTipLabel,0,Qt::AlignCenter);
    avatarLayout->setContentsMargins(0,0,0,0);

    m_avatarWidget = new QWidget;
    m_avatarWidget->setLayout(avatarLayout);
    m_avatarWidget->setContentsMargins(0,0,0,0);

    m_indexLayout = new QStackedLayout;
    m_indexLayout->addWidget(m_qrCodeWidget);
    m_indexLayout->addWidget(m_avatarWidget);
    m_indexLayout->setCurrentWidget(m_qrCodeWidget);
    m_indexLayout->setContentsMargins(0,0,0,0);

    QWidget *indexWidget = new QWidget;
    indexWidget->setLayout(m_indexLayout);
    indexWidget->setContentsMargins(0,0,0,0);

//    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T4,QFont::Bold);
//    DFontSizeManager::instance()->bind(m_tipLabel, DFontSizeManager::T6,QFont::Normal);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(titberLayout, Qt::AlignTop);
    vlayout->addWidget(titleLabel,0,Qt::AlignCenter);
//    vlayout->addStretch();
    vlayout->addWidget(indexWidget,0,Qt::AlignCenter);
    vlayout->addStretch();
    vlayout->setContentsMargins(0,0,0,0);

    CustomWidget *mainWidget = new CustomWidget;
    mainWidget->setLayout(vlayout);
    mainWidget->setContentsMargins(0,0,0,0);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainWidget);
    mainLayout->setContentsMargins(0,0,0,0);

    setLayout(mainLayout);
    setMinimumSize(376,516);
    setContentsMargins(0,0,0,0);
    setWindowModality(Qt::ApplicationModal);

    m_qrCodeStatusTimer = new QTimer;
    connect(m_qrCodeStatusTimer,&QTimer::timeout,this,&BindWeChatWindow::onQrCodeStatusTimeOut);
    m_qrCodeStatusTimer->start(1000*60*2);

    m_queryTimer = new QTimer;
    connect(m_queryTimer,&QTimer::timeout,this,&BindWeChatWindow::onQueryTimeOut);
}

void BindWeChatWindow::setData(QString accessToken,QString hardwareID,QString weChatUnionId,
                               QString userAvatar,QString nickName)
{
    m_accessToken = accessToken;
    m_hardwareID = hardwareID;
    m_weChatUnionId = weChatUnionId;
    m_userAvatar = userAvatar;
    m_nickName = nickName;
    m_avatar->setAvatarPath(m_userAvatar,true);
    m_nameLabel->setText(m_nickName);
}

void BindWeChatWindow::onRequestQrCodeResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);

    qInfo() << "onRequestQrCodeResult" << HttpClient::instance()->solveJson(result);
    if (HttpClient::instance()->solveJson(result)) {
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
            m_qrCode->showQRcodePicture(nResult + m_codeId,QSize(158,158),m_qrCode->size());
            qInfo() << "showQRcodePicture" << nResult + m_codeId;
            m_qrCodeStatusTimer->start(1000*60*2);
            m_qrCode->setWidgetType(NormalScanCode);
            m_queryTimer->start(1000);
        }
    } else {
        qInfo() << "failed";
    }
}

void BindWeChatWindow::closeEvent(QCloseEvent *event)
{
    if (1 == m_windowStatus) {
        m_queryTimer->stop();
        QNetworkReply *reply = HttpClient::instance()->reportQrCodeStatus(m_codeId, 15,m_sessionId);
        connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onCloseReportQrCodeStatus);
        event->ignore();
        hide();
    } else if (0 == m_windowStatus) {
        return DAbstractDialog::closeEvent(event);
    }
}

void BindWeChatWindow::onQrCodeStatusTimeOut()
{
    m_qrCodeStatusTimer->stop();
    m_queryTimer->stop();
    m_tipLabel->setText(QObject::tr("Invalid QR code, click to refresh"));
    m_qrCode->setWidgetType(RefreshScanCode);
}

void BindWeChatWindow::onQueryTimeOut()
{
    QNetworkReply *reply = HttpClient::instance()->getQrCodeStatus(m_codeId);
    connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onQrCodeStatusResult);
}

void BindWeChatWindow::onScanSuccess()
{
    m_indexLayout->setCurrentWidget(m_avatarWidget);
}

void BindWeChatWindow::onRefreshQrCode()
{
    QNetworkReply *reply1 = HttpClient::instance()->requestQrCode(CLIENT_ID,m_hardwareID,3,REDIRECT_URI);
    connect(reply1,&QNetworkReply::finished,this,&BindWeChatWindow::onRequestQrCodeResult);
}

void BindWeChatWindow::onQrCodeStatusResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);
    reply->deleteLater();

    if (HttpClient::instance()->solveJson(result)) {
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
                    m_windowStatus = 0;
                    m_queryTimer->stop();
                    jsonValueResult = jsonObj.value("data");

                    if (jsonValueResult.isObject()){
                        jsonObj = jsonValueResult.toObject();
                        jsonValueResult = jsonObj.value("sessionId");
                        m_sessionId = jsonValueResult.toString();

                        jsonValueResult = jsonObj.value("nickName");
                        m_nickName = jsonValueResult.toString();
                        m_nameLabel->setText(m_nickName);

                        jsonValueResult = jsonObj.value("avatarUrl");
                        m_avatar->setAvatarPath(jsonValueResult.toString(),true);

                        if (!m_weChatUnionId.isEmpty()) {
                            QNetworkReply *reply = HttpClient::instance()->unbindAccount(0,1,m_accessToken,1,0,m_weChatUnionId);
                            connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onUnbindAccountResult);
                        } else {
                            QNetworkReply *reply = HttpClient::instance()->bindAccount(0,1,m_accessToken,1,2,m_sessionId,m_nickName);
                            connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onBindAccountResult);
                        }
                    }
                } else if (nResult == 15) {
                    m_windowStatus = 0;
                    m_queryTimer->stop();
                    m_indexLayout->setCurrentWidget(m_qrCodeWidget);
                    onRefreshQrCode();
                    m_secTipLabel->setVisible(true);
                } else if (nResult == 1) {
//                    jsonValueResult = jsonObj.value("data");

//                    if (jsonValueResult.isObject()){
//                        jsonObj = jsonValueResult.toObject();
//                        jsonValueResult = jsonObj.value("sessionId");
//                        m_sessionId = jsonValueResult.toString();
//                    }
                    m_nameLabel->clear();
                    m_avatar->setAvatarPath(":/themes/light/icons/avatar_24px.png",false);
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
    QString result = HttpClient::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        m_indexLayout->setCurrentWidget(m_avatarWidget);
        m_secTipLabel->setVisible(true);
        m_resultTipLabel->setText(QObject::tr("Go to \"Accounts\" and switch on \"Login by Union ID\""));

        Q_EMIT toTellrefreshUserInfo();
    }
}

void BindWeChatWindow::onGetUserInfoResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = HttpClient::instance()->checkReply(reply);

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
    QString result = HttpClient::instance()->checkReply(reply);
    reply->deleteLater();

    if (HttpClient::instance()->solveJson(result)) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("msg");

        if (jsonValueResult == "success") {
            QNetworkReply *reply = HttpClient::instance()->reportQrCodeStatus(m_codeId, 2,m_sessionId);
            connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onReportStatusResult);
        }
    }
}

void BindWeChatWindow::onUnbindAccountResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);
    reply->deleteLater();

    if (HttpClient::instance()->solveJson(result)) {
        QByteArray byteJson = result.toLocal8Bit();
        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(byteJson, &jsonError);
        QJsonObject jsonObj = jsonDoc.object();
        QJsonValue jsonValueResult = jsonObj.value("msg");

        if (jsonValueResult == "success") {
            QNetworkReply *reply = HttpClient::instance()->bindAccount(0,1,m_accessToken,1,2,m_sessionId,m_nickName);
            connect(reply,&QNetworkReply::finished,this,&BindWeChatWindow::onBindAccountResult);
        }
    }
}

void BindWeChatWindow::onCloseReportQrCodeStatus()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(QObject::sender());
    QString result = HttpClient::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        m_windowStatus = 0;
        close();
    }
}
