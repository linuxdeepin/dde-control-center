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

    QHBoxLayout *titleLayout = new QHBoxLayout;
    titleLayout->addWidget(backBtn);
    titleLayout->addWidget(m_title);

    QVBoxLayout *centeralLayout = new QVBoxLayout;
    centeralLayout->addLayout(titleLayout);

    connect(backBtn, &QPushButton::clicked, this, &ContentWidget::back);

    setLayout(centeralLayout);
}
