/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "customconfigpage.h"
#include "widgets/translucentframe.h"

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
    m_deleteBtn->setObjectName("DeleteBtn");
    m_modifyTips = new QLabel;
    m_modifyTips->setText(tr("Please switch to current mode before modifying the settings"));
    m_deleteTips = new QLabel;
    m_deleteTips->setText(tr("Please switch to another mode before deleting the settings"));

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addSpacing(10);
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

void CustomConfigPage::onCurrentConfigChanged(const bool custom, const QString config)
{
    const bool current = config == m_configName;

    m_modifyBtn->setEnabled(current && custom);
    m_modifyTips->setVisible(!m_modifyBtn->isEnabled());
    m_deleteBtn->setEnabled(!current || !custom);
    m_deleteTips->setVisible(!m_deleteBtn->isEnabled());
}

void CustomConfigPage::onDeleteBtnClicked()
{
    Q_EMIT requestDeleteConfig(m_configName);
}

void CustomConfigPage::onModifyBtnClicked()
{
    Q_EMIT requestModifyConfig(m_configName);
}

}

}
