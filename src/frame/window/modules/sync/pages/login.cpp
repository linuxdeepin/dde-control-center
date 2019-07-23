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

    m_mainLayout->addStretch();
    m_mainLayout->addWidget(new QLabel("Cloud Sync"), 0, Qt::AlignHCenter);
    m_mainLayout->addWidget(new QPushButton("Login"), 0, Qt::AlignHCenter);
    m_mainLayout->addStretch();

    setLayout(m_mainLayout);
}
