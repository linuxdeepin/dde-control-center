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

    QPushButton* loginBtn = new QPushButton(tr("Sign In"));
    QLabel* tip = new QLabel;
    tip->setText(tr("Sign in with Deepin ID and turn on Cloud Sync to sync the specific system settings."));
    tip->setWordWrap(true);
    tip->setAlignment(Qt::AlignCenter);

    m_mainLayout->addStretch();
    m_mainLayout->addWidget(new QLabel(tr("Deepin ID Sign In")), 0, Qt::AlignHCenter);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(tip);
    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch();

    setLayout(m_mainLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::requestLoginUser);
}
