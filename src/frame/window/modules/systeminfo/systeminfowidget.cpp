/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     liuhong <liuhong_cm@deepin.com>
 *
 * Maintainer: liuhong <liuhong_cm@deepin.com>
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

#include "systeminfowidget.h"

#include <QVBoxLayout>
#include <QListView>
#include <QStandardItemModel>
#include <QIcon>
#include <QList>

using namespace DCC_NAMESPACE::systeminfo;

SystemInfoWidget::SystemInfoWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout)
    , m_listView(new QListView)
    , m_itemModel(new QStandardItemModel)
{
    initWidget();
    initData();
}

void SystemInfoWidget::initWidget()
{
    m_listView->setIconSize(QSize(50, 50));
    m_listView->setModel(m_itemModel);

    m_mainContentLayout->setContentsMargins(5, 5, 5, 0);
    m_mainContentLayout->setMargin(0);

    m_mainContentLayout->addWidget(m_listView);

    setLayout(m_mainContentLayout);
}

void SystemInfoWidget::initData()
{
    m_itemList = {
        {"dcc_on_sel", tr("About this machine"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowAboutNative)},
        {"dcc_version", tr("Version Agreement"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowVersionProtocol)},
        {"dcc_protocol", tr("End User License Agreement"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowEndUserLicenseAgreement)},
    };

    for (auto m : m_itemList) {
        QStandardItem *item = new QStandardItem;

        item->setIcon(QIcon::fromTheme(m.icon));
        item->setText(m.text);
        item->setEditable(false);
        m_itemModel->appendRow(item);
    }

    connect(m_listView, &QListView::clicked, this, [&](const QModelIndex &index){
        m_itemList[index.row()].method.invoke(this);
    });
}


