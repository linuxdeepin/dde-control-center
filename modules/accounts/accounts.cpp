#include "accounts.h"

#include <QPushButton>

Accounts::Accounts(dde::FrameProxyInterface *frame)
    : ModuleWidget(frame)
{

    m_nextPage = new QPushButton;
    m_nextPage->setText("Next Page >");
    m_centeralLayout->addWidget(m_nextPage);

    QLabel *lbl = new QLabel;
    lbl->setText("account detail widget");
    lbl->setStyleSheet("background-color:gray;");
    m_detail = new ContentWidget;
    m_detail->setTitle("account detail");
    m_detail->setContent(lbl);

    for (int i(0); i != 20; ++i)
    {
        QPushButton *b = new QPushButton;
        b->setText("test " + QString::number(i));
        m_centeralLayout->addWidget(b);
    }

    setTitle(tr("Accounts"));

    connect(m_nextPage, &QPushButton::clicked, [this] { pushWidget(m_detail); });
}
