#include "contentwidget.h"

#include <QVBoxLayout>
#include <QPushButton>

ContentWidget::ContentWidget(QWidget *parent)
    : QWidget(parent)
{
    QPushButton *backBtn = new QPushButton;
    backBtn->setText("Back");

    m_title = new QLabel;
    m_title->setText("Title");

    m_contentArea = new QScrollArea;
    m_contentArea->setFrameStyle(QFrame::NoFrame);
    m_contentArea->setStyleSheet("background-color:red;");

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(backBtn);
    titleLayout->addWidget(m_title);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addLayout(titleLayout);
    centeralLayout->addWidget(m_contentArea);
    centeralLayout->setMargin(0);
    centeralLayout->setSpacing(0);

    connect(backBtn, &QPushButton::clicked, this, &ContentWidget::back);

    setLayout(centeralLayout);
}
