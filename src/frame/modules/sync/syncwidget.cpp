#include "syncwidget.h"

using namespace dcc;
using namespace dcc::sync;

SyncWidget::SyncWidget()
    : ModuleWidget()
    , m_accountBtn(new QPushButton(tr("Login")))
{
    setObjectName("deepin ID");
    setTitle(tr("deepin ID"));

    m_centralLayout->setMargin(0);
    m_centralLayout->setSpacing(0);

    m_centralLayout->addWidget(m_accountBtn);

    connect(m_accountBtn, &QPushButton::clicked, this, &SyncWidget::requestLogin);
}
