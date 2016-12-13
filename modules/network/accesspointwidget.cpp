#include "accesspointwidget.h"

#include <QVBoxLayout>
#include <QLabel>

#include <dimagebutton.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dcc::network;

AccessPointWidget::AccessPointWidget(QWidget *parent)
    : SettingsItem(parent),

      m_lockIcon(new QLabel),
      m_strengthIcon(new QLabel),
      m_apName(new QLabel),
      m_detailBtn(new DImageButton),

      m_mainLayout(new QVBoxLayout)
{

    QHBoxLayout *basicInfoLayout = new QHBoxLayout;
    basicInfoLayout->addWidget(m_lockIcon);
    basicInfoLayout->addWidget(m_strengthIcon);
    basicInfoLayout->addWidget(m_apName);
    basicInfoLayout->addStretch();
    basicInfoLayout->addWidget(m_detailBtn);
    basicInfoLayout->setSpacing(0);
    basicInfoLayout->setContentsMargins(0, 0, 0, 0);

    m_mainLayout->addLayout(basicInfoLayout);
    m_mainLayout->setSpacing(0);
    m_mainLayout->setContentsMargins(0, 0, 0, 0);

    setLayout(m_mainLayout);
}

void AccessPointWidget::setEncyrpt(const bool encyrpt)
{
    if (!encyrpt)
        m_lockIcon->clear();
}

void AccessPointWidget::setAPName(const QString &name)
{
    m_apName->setText(name);
}
