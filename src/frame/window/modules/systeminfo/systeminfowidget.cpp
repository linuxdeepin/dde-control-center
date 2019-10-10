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

#include <DStandardItem>

#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QIcon>
#include <QList>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE::systeminfo;

SystemInfoWidget::SystemInfoWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout)
    , m_listView(new DListView)
    , m_itemModel(new QStandardItemModel)
{
    initWidget();
    initData();
}

void SystemInfoWidget::initWidget()
{
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setEditTriggers(QListView::NoEditTriggers);
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
        //~ contents_path /systeminfo/About This PC
        {"dcc_on_sel", tr("About This PC"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowAboutNative)},
//        //~ contents_path /systeminfo/Edition License
//        {"dcc_version", tr("Edition License"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowVersionProtocol)},
//        //~ contents_path /systeminfo/End User License Agreement
//        {"dcc_protocol", tr("End User License Agreement"), QMetaMethod::fromSignal(&SystemInfoWidget::requestShowEndUserLicenseAgreement)},
    };

    for (auto m : m_itemList) {
        DStandardItem *item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(m.icon));
        item->setText(m.text);
        m_itemModel->appendRow(item);
    }

    connect(m_listView, &DListView::clicked, this, [&](const QModelIndex & index) {
        m_itemList[index.row()].method.invoke(this);
    });
}

void SystemInfoWidget::setCurrentIndex(int index)
{
    QModelIndex mindex = m_itemModel->index(index, 0);
    m_listView->setFocus();
    m_listView->setCurrentIndex(mindex);
    Q_EMIT m_listView->clicked(mindex);
}
