#include "confrimwidget.h"
#include "constants.h"

#include <QHBoxLayout>
#include <QPalette>

ConfrimWidget::ConfrimWidget(QWidget *parent)
    : QWidget(parent)
{
    m_disconnect = new DTextButton(tr("Disconnect"));
    m_disconnect->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    m_ignore = new DTextButton(tr("Ignore"));
    m_ignore->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_ignore);
    mainLayout->setAlignment(m_ignore, Qt::AlignVCenter);
    mainLayout->addWidget(m_disconnect);
    mainLayout->setAlignment(m_disconnect, Qt::AlignVCenter);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(15, 5, 15, 5);

    QPalette p(QColor("#252627"));
    setPalette(p);
    setAutoFillBackground(true);
    setLayout(mainLayout);
    setFixedWidth(DCC::ModuleContentWidth);

    connect(m_disconnect, &DTextButton::clicked, this, &ConfrimWidget::disconnect);
    connect(m_ignore, &DTextButton::clicked, this, &ConfrimWidget::ignore);
}

void ConfrimWidget::setDisconnectVisible(bool visible)
{
    m_disconnect->setVisible(visible);
}
