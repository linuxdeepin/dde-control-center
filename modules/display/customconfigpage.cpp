#include "customconfigpage.h"
#include "translucentframe.h"

#include <QVBoxLayout>

using namespace dcc::widgets;

namespace dcc {

namespace display {

CustomConfigPage::CustomConfigPage(const QString &config, QWidget *parent)
    : ContentWidget(parent),

      m_configName(config)
{
    m_modifyBtn = new QPushButton;
    m_modifyBtn->setText(tr("Modify"));
    m_deleteBtn = new QPushButton;
    m_deleteBtn->setText(tr("Delete"));
    m_modifyTips = new QLabel;
    m_modifyTips->setText(tr("Please switch to current mode before modifying the settings"));
    m_deleteTips = new QLabel;
    m_deleteTips->setText(tr("Please switch to another mode before deleting the settings"));

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_modifyBtn);
    centralLayout->addWidget(m_modifyTips);
    centralLayout->addWidget(m_deleteBtn);
    centralLayout->addWidget(m_deleteTips);
    centralLayout->setMargin(0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(config);

    connect(m_modifyBtn, &QPushButton::clicked, this, &CustomConfigPage::onModifyBtnClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CustomConfigPage::onDeleteBtnClicked);
    connect(m_deleteBtn, &QPushButton::clicked, this, &CustomConfigPage::back);
}

void CustomConfigPage::onCurrentConfigChanged(const QString config)
{
    const bool current = config == m_configName;

    m_modifyBtn->setEnabled(current);
    m_modifyTips->setVisible(!current);
    m_deleteBtn->setEnabled(!current);
    m_deleteTips->setVisible(current);
}

void CustomConfigPage::onDeleteBtnClicked()
{
    emit requestDeleteConfig(m_configName);
}

void CustomConfigPage::onModifyBtnClicked()
{
    emit requestModifyConfig(m_configName);
}

}

}
