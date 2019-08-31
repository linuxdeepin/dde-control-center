/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wuchuanfei <wuchuanfei_cm@deepin.com>
 *
 * Maintainer: wuchuanfei <wuchuanfei_cm@deepin.com>
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

#include "commoninfowidget.h"

#include <DStandardItem>

#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QList>
#include <QIcon>
#include <QDebug>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace commoninfo;

CommonInfoWidget::CommonInfoWidget(QWidget *parent)
    : QWidget(parent)
    , m_vBoxLayout(new QVBoxLayout(this))
    , m_listView(new DListView(this))
    , m_itemModel(new QStandardItemModel(this))
{
    initWidget();
    initData();
}

void CommonInfoWidget::initWidget()
{
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setEditTriggers(QListView::NoEditTriggers);
    m_listView->setModel(m_itemModel);

    m_vBoxLayout->setContentsMargins(5, 5, 5, 0);
    m_vBoxLayout->setMargin(0);
    m_vBoxLayout->addWidget(m_listView);

    setLayout(m_vBoxLayout);
}

void CommonInfoWidget::initData()
{
    m_itemList = {
        {
            "dcc_boot_menu", tr("Boot Menu"),
            QMetaMethod::fromSignal(&CommonInfoWidget::requestShowBootWidget)
        },
        {
            "dcc_developer_mode", tr("Developer Mode"),
            QMetaMethod::fromSignal(&CommonInfoWidget::requestShowDeveloperModeWidget)
        },
        {
            "dcc_ue_plan", tr("User Experience Program"),
            QMetaMethod::fromSignal(&CommonInfoWidget::requestShowUEPlanWidget)
        },
// 以下内容为平板模式做预留
//        {
//            "", tr("Tablet Plan"),
//            QMetaMethod::fromSignal(&CommonInfoWidget::requestShowTabletModeWidget)
//        },
    };

    for (auto m : m_itemList) {
        DStandardItem *item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(m.itemIcon));
        item->setText(m.itemText);
        m_itemModel->appendRow(item);
    }

    connect(m_listView, &DListView::clicked, this, [&](const QModelIndex & index) {
        m_itemList[index.row()].itemSignal.invoke(this);
    });
}
