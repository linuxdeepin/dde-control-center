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
#include "window/gsettingwatcher.h"
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

CommonInfoWidget::~CommonInfoWidget()
{

}

dcc::widgets::MultiSelectListView *CommonInfoWidget::getCommonListView()
{
    return m_listView;
}

void CommonInfoWidget::showDefaultWidget()
{
    for (int i = 0; i < m_itemModel->rowCount(); i++) {
        if (!m_listView->isRowHidden(i)) {
            m_listView->activated(m_itemModel->index(i, 0));
            break;
        }
    }
}

void CommonInfoWidget::initWidget()
{
    setAccessibleName("CommonInfoWidget");
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
    m_itemList.append({"dcc_boot_menu", tr("Boot Menu"),
                       QMetaMethod::fromSignal(&CommonInfoWidget::requestShowBootWidget), nullptr, "bootMenu"});
#endif

    //以下模块只在非服务器版本使用
    if (!IsServerSystem && !IsCommunitySystem) {
        if (DSysInfo::uosEditionType() != DSysInfo::UosEuler || DSysInfo::uosEditionType() != DSysInfo::UosEnterpriseC) {
            //~ contents_path /commoninfo/Developer Mode
            m_itemList.append({"dcc_developer_mode", tr("Developer Mode"),
                               QMetaMethod::fromSignal(&CommonInfoWidget::requestShowDeveloperModeWidget), nullptr, "developerMode"});
        }
        m_itemList.append({"dcc_ue_plan", tr("User Experience Program"),
                           QMetaMethod::fromSignal(&CommonInfoWidget::requestShowUEPlanWidget), nullptr, "userExperienceProgram"});

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
        GSettingWatcher::instance()->bind(m.gsettingsName, m_listView, item);
    }

    if (InsertPlugin::instance()->needPushPlugin("commoninfo"))
        InsertPlugin::instance()->pushPlugin(m_itemModel, m_itemList);

    connect(m_listView, &DListView::clicked, this, [&](const QModelIndex & index) {
        if (m_lastIndex == index) return;

        m_lastIndex = index;

        m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].pulgin ? m_itemList[index.row()].pulgin : this);
        m_listView->resetStatus(index);
    });
    connect(m_listView, &DListView::activated, m_listView, &QListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [=](int row) {
        bool isAllHidden = true;
        for (int i = 0; i < m_itemModel->rowCount(); i++) {
            if (!m_listView->isRowHidden(i))
                isAllHidden = false;
        }

        if (m_listView->selectionModel()->selectedRows().size() > 0) {
            int index = m_listView->selectionModel()->selectedRows()[0].row();
            Q_EMIT requestUpdateSecondMenu(index == row);
        } else {
            Q_EMIT requestUpdateSecondMenu(false);
        }

        if (isAllHidden) {
            m_lastIndex = QModelIndex();
            m_listView->clearSelection();
        }
    });
}
