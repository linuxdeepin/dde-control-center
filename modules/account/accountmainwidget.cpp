#include "accountmainwidget.h"

AccountMainWidget::AccountMainWidget(QWidget *parent) : QFrame(parent)
{
    D_THEME_INIT_WIDGET(AccountMainWidget);

    m_mainLayout = new QVBoxLayout(this);
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setAlignment(Qt::AlignHCenter | Qt::AlignTop);

    DSeparatorHorizontal *sh = new DSeparatorHorizontal(this);

    m_header = new ModuleHeader("Account", false, this);

    m_listPanel = new UserListPanel(this);

    m_listScrollArea = new QScrollArea(this);
    m_listScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listScrollArea->setWidgetResizable(true);
    m_listScrollArea->setWidget(m_listPanel);

    m_mainLayout->addWidget(m_header);
    m_mainLayout->addWidget(sh);
    m_mainLayout->addWidget(m_listScrollArea);
    setStyleSheet("background-color: transparent;");
}

void AccountMainWidget::resizeEvent(QResizeEvent *event)
{
    int tmpWidth = event->size().width();
    m_header->setFixedWidth(tmpWidth);
    m_listPanel->setFixedWidth(tmpWidth);
    m_listScrollArea->setFixedWidth(tmpWidth);

    QFrame::resizeEvent(event);
}

