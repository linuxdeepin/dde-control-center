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
#include "window/insertplugin.h"
#include "window/utils.h"
#include "widgets/multiselectlistview.h"


#include <DStandardItem>

#include <QVBoxLayout>
#include <QStandardItemModel>
#include <QList>
#include <QIcon>
#include <QDebug>

DWIDGET_USE_NAMESPACE

using namespace DCC_NAMESPACE;
using namespace commoninfo;
#define GSETTINGS_CONTENS_SERVER "iscontens-server"

CommonInfoWidget::CommonInfoWidget(QWidget *parent)
    : QWidget(parent)
    , m_vBoxLayout(new QVBoxLayout(this))
    , m_listView(new dcc::widgets::MultiSelectListView(this))
    , m_itemModel(new QStandardItemModel(this))
    , isContensServer(false)
{
    initWidget();
    initData();
}

dcc::widgets::MultiSelectListView *CommonInfoWidget::getCommonListView()
{
    return m_listView;
}

void CommonInfoWidget::initWidget()
{
    m_listView->setAccessibleName("List_commonlist");
    m_listView->setViewportMargins(ScrollAreaMargins);
    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setEditTriggers(QListView::NoEditTriggers);
    m_listView->setIconSize(ListViweIconSize);
    m_listView->setModel(m_itemModel);
    m_listView->setItemSize(ListViweSysItemSize);

    m_vBoxLayout->setContentsMargins(5, 5, 5, 0);
    m_vBoxLayout->setMargin(0);
    m_vBoxLayout->addWidget(m_listView);

    setLayout(m_vBoxLayout);
}

void CommonInfoWidget::initData()
{
#ifndef DCC_DISABLE_GRUB
    //~ contents_path /commoninfo/Boot Menu
    m_itemList.append({"dcc_boot_menu", tr("Boot Menu"),
                       QMetaMethod::fromSignal(&CommonInfoWidget::requestShowBootWidget)});
#endif

    if (QGSettings::isSchemaInstalled("com.deepin.dde.control-versiontype")) {
        m_moduleDevelop = new QGSettings("com.deepin.dde.control-versiontype", QByteArray(), this);
        isContensServer =  m_moduleDevelop->get(GSETTINGS_CONTENS_SERVER).toBool();
    }

    //以下模块只在非服务器版本使用
    if (!IsServerSystem && !IsCommunitySystem) {
        if (!isContensServer) {
            //~ contents_path /commoninfo/Developer Mode
            m_itemList.append({"dcc_developer_mode", tr("Developer Mode"),
                               QMetaMethod::fromSignal(&CommonInfoWidget::requestShowDeveloperModeWidget)});
        }
        //~ contents_path /commoninfo/User Experience Program
        m_itemList.append({"dcc_ue_plan", tr("User Experience Program"),
                           QMetaMethod::fromSignal(&CommonInfoWidget::requestShowUEPlanWidget)});

#if 0
        m_itemList.append({"", tr("Tablet Mode"),
                           QMetaMethod::fromSignal(&CommonInfoWidget::requestShowTabletModeWidget)
                          });
#endif
    }

    for (auto m : m_itemList) {
        DStandardItem *item = new DStandardItem;
        item->setIcon(QIcon::fromTheme(m.itemIcon));
        item->setText(m.itemText);
        m_itemModel->appendRow(item);
    }

    if(InsertPlugin::instance()->needPushPlugin("commoninfo"))
        InsertPlugin::instance()->pushPlugin(m_itemModel,m_itemList);

    connect(m_listView, &DListView::clicked, this, [&](const QModelIndex & index) {
        if (m_lastIndex == index) return;

        m_lastIndex = index;

        m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].pulgin ? m_itemList[index.row()].pulgin : this);
        m_listView->resetStatus(index);
    });
    connect(m_listView, &DListView::activated, m_listView, &QListView::clicked);

}
