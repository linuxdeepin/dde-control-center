#include "login.h"

#include <DSuggestButton>

#include <QVBoxLayout>
#include <QLabel>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;
DWIDGET_USE_NAMESPACE

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    //~ contents_path /cloudsync/Sign In
    DSuggestButton *loginBtn = new DSuggestButton(tr("Sign In"));
    loginBtn->setMinimumWidth(302);
    QLabel *tip = new QLabel;
    QFont ft = tip->font();
    ft.setPointSize(12);
    tip->setFont(ft);
    //~ contents_path /cloudsync/Sign in with Deepin ID and turn on Cloud Sync to sync the specific system settings.
    tip->setText(tr("Sign in with Deepin ID and turn on Cloud Sync to sync the specific system settings."));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch(3);
    QLabel *pLogo = new QLabel;
    pLogo->setPixmap(QIcon::fromTheme("dcc_cloud").pixmap(128, 100));
    m_mainLayout->addWidget(pLogo, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    //~ contents_path /cloudsync/Deepin ID Sign In
    QLabel *signintip = new QLabel(tr("Deepin ID Sign In"));
    QFont font = signintip->font();
    font.setBold(true);
    font.setPointSize(20);
    signintip->setFont(font);
    m_mainLayout->addWidget(signintip, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(2);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addSpacing(35);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(20);
    m_mainLayout->addWidget(new QLabel(tr("Only supported in Mainland China at present")), 0, Qt::AlignHCenter);
    m_mainLayout->addStretch(5);

    setLayout(m_mainLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::requestLoginUser);
}
