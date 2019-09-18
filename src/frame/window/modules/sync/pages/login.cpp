#include "login.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

LoginPage::LoginPage(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    //~ contents_path /cloudsync/Sign In
    QPushButton* loginBtn = new QPushButton(tr("Sign In"));
    QLabel* tip = new QLabel;
    //~ contents_path /cloudsync/Sign in with Deepin ID and turn on Cloud Sync to sync the specific system settings.
    tip->setText(tr("Sign in with Deepin ID and turn on Cloud Sync to sync the specific system settings."));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch();
    //~ contents_path /cloudsync/Deepin ID Sign In
    m_mainLayout->addWidget(new QLabel(tr("Deepin ID Sign In")), 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    //~ contents_path /cloudsync/Only supported in Mainland China at present
    m_mainLayout->addWidget(new QLabel(tr("Only supported in Mainland China at present")), 0, Qt::AlignHCenter);
    m_mainLayout->addStretch();

    setLayout(m_mainLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::requestLoginUser);
}
