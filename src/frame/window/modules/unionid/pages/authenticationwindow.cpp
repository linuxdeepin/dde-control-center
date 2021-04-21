#include "authenticationwindow.h"
#include "bindwechatwindow.h"
#include "modules/unionid/httpclient.h"
#include "window/modules/unionid/define.h"
#include "customwidget.h"
#include "window/modules/unionid/notificationmanager.h"

#include <DTitlebar>
#include <DFontSizeManager>
#include <DWindowCloseButton>

#include <QLabel>
#include <QVBoxLayout>
#include <QLineEdit>
#include <QTimer>
#include <QDBusInterface>

AuthenticationWindow::AuthenticationWindow(QWidget *parent)
    : DAbstractDialog(parent)
{
    m_second = 0;

    DWindowCloseButton *closeButton = new DWindowCloseButton;
    closeButton->setFixedHeight(48);
    closeButton->setIconSize(QSize(48,48));
    closeButton->setContentsMargins(0,0,0,0);
    connect(closeButton,&DWindowCloseButton::clicked,this,&AuthenticationWindow::close);

    QHBoxLayout *titberLayout = new QHBoxLayout;
    titberLayout->addStretch();
    titberLayout->addWidget(closeButton);
    titberLayout->setContentsMargins(0,0,0,0);

    QFont font("NotoSansCJKsc");
    font.setPixelSize(24);
    font.setWeight(QFont::DemiBold);

    QLabel * titleLabel= new QLabel(QObject::tr("Identity Verification"));
    titleLabel->setContentsMargins(0,13,0,41);
    titleLabel->setFont(font);

    font.setPixelSize(18);
    font.setWeight(QFont::Normal);
    QLabel *contentsLabel = new QLabel(QObject::tr("Verify your phone number"));
    contentsLabel->setAlignment(Qt::AlignCenter);
    contentsLabel->setWordWrap(true);
    contentsLabel->setContentsMargins(0,0,0,0);
    contentsLabel->setFont(font);

    font.setPixelSize(15);
    m_tipLabel = new QLabel(QObject::tr("Get a verification code by phone number xxx-xxxx-%1"));
    m_tipLabel->setContentsMargins(4,0,0,0);
    m_tipLabel->setMinimumSize(312,44);
    m_tipLabel->setAlignment(Qt::AlignCenter);
    m_tipLabel->setWordWrap(true);
    m_tipLabel->setFont(font);

    QPalette pa = m_tipLabel->palette();
    pa.setColor(QPalette::WindowText,QColor(121,129,144));
    m_tipLabel->setPalette(pa);

    m_lineEdit = new QLineEdit;
    m_lineEdit->setPlaceholderText(QObject::tr("Verification code"));
    m_lineEdit->setMinimumSize(QSize(197,40));
    m_lineEdit->setContentsMargins(0,0,0,0);
    m_lineEdit->setFont(font);
    connect(m_lineEdit,&QLineEdit::textChanged,this,&AuthenticationWindow::onLineEditTextChanged);

    m_getCodeButton = new DSuggestButton;
    m_getCodeButton->setText(QObject::tr("Get code"));
    m_getCodeButton->setMinimumSize(QSize(107,40));
    m_getCodeButton->setFont(font);
    connect(m_getCodeButton,&DSuggestButton::clicked,this,&AuthenticationWindow::onGetCodeButtonClicked);

    QHBoxLayout *hLineLayout = new QHBoxLayout;
    hLineLayout->addWidget(m_lineEdit);
    hLineLayout->addStretch();
    hLineLayout->addWidget(m_getCodeButton);
    hLineLayout->setContentsMargins(32,8,32,8);

    m_nextButton = new DSuggestButton;
    m_nextButton ->setMinimumSize(QSize(312,40));
    m_nextButton->setText(QObject::tr("Next"));
    m_nextButton->setContentsMargins(8,34,32,0);
    m_nextButton->setEnabled(false);
    m_nextButton->setFont(font);
    connect(m_nextButton,&DSuggestButton::clicked,this,&AuthenticationWindow::onNextButtonClicked);

    font.setPixelSize(12);
    m_warningLabel = new QLabel;
    m_warningLabel->setContentsMargins(32,0,0,0);
    m_warningLabel->setAlignment(Qt::AlignLeft);
    m_warningLabel->setMinimumHeight(18);
    m_warningLabel->setFont(font);

    QPalette pe = m_warningLabel->palette();
    pe.setColor(QPalette::WindowText,QColor(249,112,79));
    m_warningLabel->setPalette(pe);

//    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T4,QFont::Bold);
//    DFontSizeManager::instance()->bind(m_tipLabel, DFontSizeManager::T6,QFont::Normal);
//    DFontSizeManager::instance()->bind(m_warningLabel, DFontSizeManager::T7,QFont::Normal);
//    DFontSizeManager::instance()->bind(contentsLabel, DFontSizeManager::T5,QFont::Normal);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addLayout(titberLayout);
    vlayout->addWidget(titleLabel,0,Qt::AlignCenter);
    vlayout->addWidget(contentsLabel,0,Qt::AlignCenter);
    vlayout->addWidget(m_tipLabel,0,Qt::AlignCenter);
    vlayout->addLayout(hLineLayout);   
    vlayout->addWidget(m_warningLabel);
    vlayout->addWidget(m_nextButton,0,Qt::AlignCenter);
    vlayout->addStretch();
    vlayout->setContentsMargins(0,0,0,0);

    CustomWidget *mainWidget = new CustomWidget;
    mainWidget->setLayout(vlayout);
    mainWidget->setContentsMargins(0,0,0,0);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(mainWidget);
    mainLayout->setContentsMargins(0,0,0,0);
//    setCentralWidget(widget);
    setContentsMargins(0,0,0,0);
    setLayout(mainLayout);
    setFixedSize(376,516);
    setWindowModality(Qt::ApplicationModal);

    m_timer = new QTimer;
    connect(m_timer,&QTimer::timeout,this,&AuthenticationWindow::onTimeOut);
}

void AuthenticationWindow::setData(QString phoneNumber, QString weChatUnionId, QString accessToken,
                                   QString userAvatar, QString nickName)
{
    m_tipLabel->setText(QObject::tr("Get a verification code by phone number xxx-xxxx-%1").arg(phoneNumber.right(4)));
    m_phoneNumber = phoneNumber;
    m_accessToken = accessToken;
    m_weChatUnionId = weChatUnionId;
    m_userAvatar = userAvatar;
    m_nickName = nickName;
}

void AuthenticationWindow::onGetCodeButtonClicked()
{
    if (Notificationmanager::instance()->isOnLine()) {
        QNetworkReply *reply = HttpClient::instance()->sendSmsCode(m_phoneNumber, m_accessToken);
        connect(reply,&QNetworkReply::finished,this,&AuthenticationWindow::onSendSmsCodeResult);
    } else {
        Notificationmanager::instance()->showToast(this,Notificationmanager::NetworkError);
    }
}

void AuthenticationWindow::onSendSmsCodeResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = HttpClient::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        if (HttpClient::instance()->solveJson(result)) {
            m_getCodeButton->setEnabled(false);
            m_timer->start(1000);
        }
    }
}

void AuthenticationWindow::onLineEditTextChanged(QString text)
{
    if (!m_warningLabel->text().isEmpty()) {
        m_warningLabel->clear();
    }

    if (text.isEmpty()) {
        m_nextButton->setEnabled(false);
    } else {
        m_nextButton->setEnabled(true);
    }
}

void AuthenticationWindow::onNextButtonClicked()
{
    if (Notificationmanager::instance()->isOnLine()) {
        QNetworkReply *reply = HttpClient::instance()->verifySmsCode(m_phoneNumber,m_lineEdit->text(), m_accessToken);
        connect(reply,&QNetworkReply::finished,this,&AuthenticationWindow::onVerifySmsCodeResult);
    } else {
        Notificationmanager::instance()->showToast(this,Notificationmanager::NetworkError);
    }
}

void AuthenticationWindow::onVerifySmsCodeResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = HttpClient::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        if (HttpClient::instance()->solveJson(result)) {
            BindWeChatWindow *bindWeChatWindow = new BindWeChatWindow;
            connect(bindWeChatWindow,&BindWeChatWindow::toTellrefreshUserInfo,this,&AuthenticationWindow::toTellrefreshUserInfo);
            connect(bindWeChatWindow,&BindWeChatWindow::close,this,&AuthenticationWindow::close);

            QDBusInterface interface1("com.deepin.deepinid",
                                     "/com/deepin/deepinid",
                                     "com.deepin.deepinid",
                                      QDBusConnection::sessionBus());

            QVariant strReply = interface1.property("HardwareID");

            bindWeChatWindow->setData(m_accessToken,strReply.toString(),m_weChatUnionId,m_userAvatar,m_nickName);
            QNetworkReply *reply = HttpClient::instance()->requestQrCode(CLIENT_ID,strReply.toString(),3,REDIRECT_URI);
            connect(reply,&QNetworkReply::finished,bindWeChatWindow,&BindWeChatWindow::onRequestQrCodeResult);
            hide();
            bindWeChatWindow->show();
        } else {
            if (m_warningLabel->text().isEmpty()) {
                m_warningLabel->setText(QObject::tr("Wrong verification code"));
            }
        }
    }
}

void AuthenticationWindow::onTimeOut()
{
    m_second++;

    m_getCodeButton->setText(QString(QObject::tr("Resend (%1s)")).arg(60 - m_second));

    if (m_second == 60) {
        m_second = 0;
        m_timer->stop();
        m_getCodeButton->setEnabled(true);
        m_getCodeButton->setText(QObject::tr("Get code"));
    }
}

