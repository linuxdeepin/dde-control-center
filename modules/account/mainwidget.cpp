#include "mainwidget.h"

MainWidget::MainWidget(QWidget *parent) : QFrame(parent)
{
    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    DSeparatorHorizontal *sh = new DSeparatorHorizontal(this);

    m_header = new ModuleHeader("Account", false, this);
    m_header->setFixedWidth(320);

    AccountListPanel *listPanel = new AccountListPanel(this);
    listPanel->setFixedWidth(320);

    m_mainLayout->addWidget(m_header);
    m_mainLayout->addWidget(sh);
    m_mainLayout->addWidget(listPanel);
}

