#include "syncwidget.h"

#include "pages/login.h"
#include "pages/index.h"

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::sync;

SyncWidget::SyncWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainLayout(new QVBoxLayout)
{
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);

    LoginPage *loginPage = new LoginPage;

    m_mainLayout->addWidget(loginPage, 0, Qt::AlignCenter);

    setLayout(m_mainLayout);
}
