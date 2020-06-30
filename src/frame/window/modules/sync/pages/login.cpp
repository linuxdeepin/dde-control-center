#include "login.h"

#include <DSuggestButton>
#include <DFontSizeManager>
#include <DTipLabel>
#include <DSysInfo>

#include <QVBoxLayout>
#include <QLabel>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
DWIDGET_USE_NAMESPACE
DCORE_USE_NAMESPACE

#include "window/utils.h"

QString systemName() {
    if (SystemTypeName == "Professional")
        return "UOS";

    return "Deepin";
}

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    //~ contents_path /cloudsync/Sync ID Sign In
    DSuggestButton *loginBtn = new DSuggestButton(tr("Sign In"));
    loginBtn->setMinimumWidth(302);
    DTipLabel *tip = new DTipLabel(tr("After signing in cloud account, you can use cloud sync, app store, email client, browser and other cloud services."));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch(3);
    QLabel *pLogo = new QLabel;
    pLogo->setPixmap(QIcon::fromTheme("dcc_cloud").pixmap(128, 100));
    m_mainLayout->addWidget(pLogo, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    QLabel *signintip = new QLabel(tr("Cloud Account Sign In"));
    QFont font = signintip->font();
    font.setBold(true);
    signintip->setFont(font);
    DFontSizeManager::instance()->bind(signintip, DFontSizeManager::T3);

    m_mainLayout->addWidget(signintip, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(6);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addSpacing(35);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(20);
    DTipLabel *sps = new DTipLabel(tr("Only supported in Mainland China at present"));
    m_mainLayout->addWidget(sps, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch(5);

    setLayout(m_mainLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::requestLoginUser);
}
