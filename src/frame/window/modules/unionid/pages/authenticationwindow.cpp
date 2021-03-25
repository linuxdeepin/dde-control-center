#include "authenticationwindow.h"
#include "bindwechatwindow.h"
#include "modules/unionid/requestservice.h"
#include "window/modules/unionid/define.h"
#include <DTitlebar>
#include <QLabel>
#include <QVBoxLayout>
#include <QGraphicsView>
#include <DFontSizeManager>
#include <QLineEdit>
#include <QTimer>
#include <QDBusInterface>

AuthenticationWindow::AuthenticationWindow(DMainWindow *prarent)
    : DMainWindow(prarent)
{
    m_second = 0;
    QGraphicsView *widget = new QGraphicsView;
    widget->setFrameShape(QFrame::NoFrame);
    QColor color(255,255,255);
    color.setAlphaF(0.9);
    QPalette pa = widget->palette();
    pa.setColor(QPalette::Window,color);

    setWindowFlag(Qt::Dialog);
    auto flag = windowFlags();
    flag &= ~Qt::WindowMinMaxButtonsHint;
    //flag |= Qt::WindowStaysOnTopHint;
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
    titlePa.setColor(QPalette::Base,color);
    titlebar()->setPalette(titlePa);
//    qInfo() << "QPalette::Base" << pa.color(QPalette::Base) << pa.color(QPalette::Background);

    QLabel * titleLabel= new QLabel(QObject::tr("Identity Varification"));
    titleLabel->setContentsMargins(0,0,0,72);

    QLabel *contentsLabel = new QLabel(QObject::tr("Varification your phone number"));
    contentsLabel->setContentsMargins(0,0,0,8);

    m_tipLabel = new QLabel(QObject::tr("Get a verification code by phone number %1"));
    m_tipLabel->setContentsMargins(0,0,0,16);

    m_lineEdit = new QLineEdit;
    m_lineEdit->setPlaceholderText(QObject::tr("Verification code"));
    m_lineEdit->setMinimumSize(QSize(197,40));
    m_lineEdit->setContentsMargins(0,0,0,0);
    connect(m_lineEdit,&QLineEdit::textChanged,this,&AuthenticationWindow::onLineEditTextChanged);

    m_getCodeButton = new DSuggestButton;
    m_getCodeButton->setText(QObject::tr("Get code"));
    m_getCodeButton->setMinimumSize(QSize(107,40));
    connect(m_getCodeButton,&DSuggestButton::clicked,this,&AuthenticationWindow::onGetCodeButtonClicked);

    QHBoxLayout *hLineLayout = new QHBoxLayout;
    hLineLayout->setContentsMargins(32,0,32,34);
    hLineLayout->addWidget(m_lineEdit);
    hLineLayout->addStretch();
    hLineLayout->addWidget(m_getCodeButton);

    m_nextButton = new DSuggestButton;
    m_nextButton ->setMinimumSize(QSize(312,40));
    m_nextButton->setText(QObject::tr("Next"));
    m_nextButton->setContentsMargins(32,0,32,0);
    m_nextButton->setEnabled(false);
    connect(m_nextButton,&DSuggestButton::clicked,this,&AuthenticationWindow::onNextButtonClicked);

    QLabel *warningLabel = new QLabel(QObject::tr("Wrong verification code"));
    warningLabel->setContentsMargins(0,0,0,0);
    warningLabel->setVisible(false);

    QPalette pe = warningLabel->palette();
    pe.setColor(QPalette::Text,QColor(249,112,79));
    warningLabel->setPalette(pe);

    DFontSizeManager::instance()->bind(titleLabel, DFontSizeManager::T4,QFont::Bold);
    DFontSizeManager::instance()->bind(m_tipLabel, DFontSizeManager::T6,QFont::Normal);
    DFontSizeManager::instance()->bind(contentsLabel, DFontSizeManager::T6,QFont::Normal);

    QVBoxLayout *vlayout = new QVBoxLayout;
    vlayout->addWidget(titleLabel,0,Qt::AlignCenter);
    vlayout->addWidget(contentsLabel,0,Qt::AlignCenter);
    vlayout->addWidget(m_tipLabel,0,Qt::AlignCenter);
    vlayout->addLayout(hLineLayout);
    vlayout->addWidget(m_nextButton,0,Qt::AlignCenter);
    vlayout->addStretch();
    vlayout->setContentsMargins(0,0,0,0);

    widget->setLayout(vlayout);
    setCentralWidget(widget);
    setFixedSize(376,516);
    //   setWindowModality(Qt::WindowModal);
    m_timer = new QTimer;
    connect(m_timer,&QTimer::timeout,this,&AuthenticationWindow::onTimeOut);
}

void AuthenticationWindow::setData(QString phoneNumber, QString weChatUnionId, QString acccessToken,QString refreshToken)
{
    m_tipLabel->setText(QObject::tr("Get a verification code by phone number %1").arg(phoneNumber));
    m_phoneNumber = phoneNumber;
    m_acccessToken = acccessToken;
    m_weChatUnionId = weChatUnionId;
    m_refreshToken = refreshToken;
}

void AuthenticationWindow::onGetCodeButtonClicked()
{
    QNetworkReply *reply = RequestService::instance()->sendSmsCode(m_phoneNumber, m_acccessToken);
    connect(reply,&QNetworkReply::finished,this,&AuthenticationWindow::onSendSmsCodeResult);
}

void AuthenticationWindow::onSendSmsCodeResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = RequestService::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        if (RequestService::instance()->solveJson(result)) {
            m_getCodeButton->setEnabled(false);
            m_timer->start(1000);
        }
    }
}

void AuthenticationWindow::onLineEditTextChanged(QString text)
{
    qInfo() << "onLineEditTextChanged " << text;
    if (m_tipLabel->isVisible()) {
        m_tipLabel->setVisible(false);
    }

    if (text.isEmpty()) {
        m_nextButton->setEnabled(false);
    } else {
        m_nextButton->setEnabled(true);
    }
}

void AuthenticationWindow::onNextButtonClicked()
{
    QNetworkReply *reply = RequestService::instance()->verifySmsCode(m_phoneNumber,m_lineEdit->text(), m_acccessToken);
    connect(reply,&QNetworkReply::finished,this,&AuthenticationWindow::onVerifySmsCodeResult);
}

void AuthenticationWindow::onVerifySmsCodeResult()
{
    QNetworkReply *reply = static_cast<QNetworkReply *>(sender());
    QString result = RequestService::instance()->checkReply(reply);

    if (!result.isEmpty()) {
        if (RequestService::instance()->solveJson(result)) {
            BindWeChatWindow *bindWeChatWindow = new BindWeChatWindow;
            connect(bindWeChatWindow,&BindWeChatWindow::toTellrefreshUserInfo,this,&AuthenticationWindow::toTellrefreshUserInfo);
            connect(bindWeChatWindow,&BindWeChatWindow::close,this,&AuthenticationWindow::close);

            QDBusInterface interface1("com.deepin.deepinid",
                                     "/com/deepin/deepinid",
                                     "com.deepin.deepinid",
                                      QDBusConnection::sessionBus());

            QVariant strReply = interface1.property("HardwareID");

            bindWeChatWindow ->setData(m_acccessToken,strReply.toString(),m_weChatUnionId,m_refreshToken);

            if (m_weChatUnionId.isEmpty()) {
                QNetworkReply *reply = RequestService::instance()->requestQrCode(CLIENT_ID,strReply.toString(),3,REDIRECT_URI);
                connect(reply,&QNetworkReply::finished,bindWeChatWindow,&BindWeChatWindow::onRequestQrCodeResult);
                hide();
                bindWeChatWindow->show();
            } else {
                    QNetworkReply *reply = RequestService::instance()->requestQrCode(CLIENT_ID,strReply.toString(),3,REDIRECT_URI);
                    connect(reply,&QNetworkReply::finished,bindWeChatWindow,&BindWeChatWindow::onRequestQrCodeResult);
                    hide();
                    bindWeChatWindow->show();
            }

        } else {
            m_tipLabel->setVisible(true);
        }
    }
}

void AuthenticationWindow::onTimeOut()
{
    m_second++;

    m_getCodeButton->setText(QString(QObject::tr("%1s")).arg(60 - m_second));

    if (m_second == 60) {
        m_second = 0;
        m_timer->stop();
        m_getCodeButton->setEnabled(true);
        m_getCodeButton->setText(QObject::tr("Get code"));
    }
}

