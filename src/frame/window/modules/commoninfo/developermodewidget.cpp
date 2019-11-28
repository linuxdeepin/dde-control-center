/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     sunkang <sunkang_cm@deepin.com>
 *
 * Maintainer: sunkang <sunkang_cm@deepin.com>
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

#include "developermodewidget.h"
#include "window/modules/commoninfo/commoninfomodel.h"

#include "widgets/switchwidget.h"
#include "widgets/labels/tipslabel.h"

#include <DTipLabel>

#include <QVBoxLayout>
#include <QTimer>
#include<QPushButton>
#include<QDebug>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace commoninfo;
DWIDGET_USE_NAMESPACE

DeveloperModeWidget::DeveloperModeWidget(QWidget *parent)
    : QWidget(parent)
    , m_model(nullptr)
{
    QVBoxLayout *vBoxLayout = new QVBoxLayout;
    m_pDeveloperMode = new SwitchWidget(tr("Developer Mode"));
    m_pDeveloperMode->addBackground();
    DTipLabel *titleLabel = new DTipLabel(tr("To join the developer mode, you need to bind the deepin ID. To join the developer mode, you will lose your guarantee. Please note!"));
    titleLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    titleLabel->setWordWrap(true);

    vBoxLayout->setMargin(0);
    vBoxLayout->setSpacing(0);
    vBoxLayout->setContentsMargins(6, 0, 6, 0);
    vBoxLayout->addWidget(m_pDeveloperMode);
    vBoxLayout->addSpacing(10);
    vBoxLayout->addWidget(titleLabel);
    vBoxLayout->addStretch();
    setLayout(vBoxLayout);

    connect(m_pDeveloperMode, &SwitchWidget::clicked, this, &DeveloperModeWidget::onCheckedChanged);
}

void DeveloperModeWidget::setModel(CommonInfoModel *model)
{
    m_model = model;
    connect(model, &CommonInfoModel::developerModeStateChanged, this, &DeveloperModeWidget::updateDeveloperModeState);
}

void DeveloperModeWidget::onCheckedChanged()
{
    if (m_model && m_model->developerModeState()) {
        return;
    }
    Q_EMIT enableDeveloperMode(true);
}

void DeveloperModeWidget::updateDeveloperModeState(const bool state)
{
    if (state) {
        m_pDeveloperMode->setChecked(true);
        //开发者模式不可逆,这里将控件disable
        m_pDeveloperMode->setDisabled(true);
    } else {
        m_pDeveloperMode->setChecked(false);
    }
}
