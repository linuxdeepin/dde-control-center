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

    m_mainLayout->addStretch();
    m_mainLayout->addWidget(new QLabel("Cloud Sync"), 0, Qt::AlignHCenter);
    m_mainLayout->addWidget(loginBtn, 0, Qt::AlignHCenter);
    m_mainLayout->addStretch();

    setLayout(m_mainLayout);

    connect(loginBtn, &QPushButton::clicked, this, &LoginPage::requestLoginUser);
}
