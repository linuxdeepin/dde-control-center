#include "confrimwidget.h"

#include <QHBoxLayout>

ConfrimWidget::ConfrimWidget(QWidget *parent)
    : QWidget(parent)
{
    m_disconnect = new DTextButton(tr("Disconnect"));
    m_ignore = new DTextButton(tr("Ignore"));

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_ignore);
    mainLayout->setAlignment(m_ignore, Qt::AlignVCenter);
    mainLayout->addWidget(m_disconnect);
    mainLayout->setAlignment(m_disconnect, Qt::AlignVCenter);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 3, 15, 3);

    setLayout(mainLayout);

    connect(m_disconnect, &DTextButton::clicked, this, &ConfrimWidget::disconnect);
    connect(m_ignore, &DTextButton::clicked, this, &ConfrimWidget::ignore);
}

