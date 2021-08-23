/*
 * Copyright (C) 2019 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     wubw <wubowen_cm@deepin.com>
 *
 * Maintainer: wubw <wubowen_cm@deepin.com>
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

#include "powerwidget.h"

#include "modules/mouse/widget/doutestwidget.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/power/powermodel.h"
#include "widgets/multiselectlistview.h"
#include "window/gsettingwatcher.h"
#include "window/utils.h"

#include <DStyleOption>

#include <QStandardItemModel>
#include <QTimer>
#include <QVBoxLayout>

using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

PowerWidget::PowerWidget(QWidget *parent)
    : QWidget(parent)
    , m_listView(new dcc::widgets::MultiSelectListView(this))
    , m_itemModel(new QStandardItemModel(this))
    , m_model(nullptr)
    , m_bhasBattery(false)
    , m_batteryIndex(0)
{
    initUi();
    initMembers();
    initConnections();
}

PowerWidget::~PowerWidget()
{
}

void PowerWidget::initUi()
{
    QVBoxLayout *powerLayout = new QVBoxLayout(this);
    powerLayout->setContentsMargins(0, 0, 0, 0);

    m_listView->setFrameShape(QFrame::NoFrame);
    m_listView->setModel(m_itemModel);
    m_listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listView->setViewportMargins(ScrollAreaMargins);
    m_listView->setIconSize(ListViweIconSize);
    powerLayout->addWidget(m_listView);
}

void PowerWidget::initMembers()
{
    if (!IsServerSystem) {
        //~ contents_path /power/General
        //~ child_page General
        m_menuIconText.append({"dcc_general_purpose", tr("General"), QMetaMethod::fromSignal(&PowerWidget::requestShowGeneral), nullptr, "general"});
    }
    //~ contents_path /power/Plugged In
    //~ child_page Plugged In
    m_menuIconText.append({"dcc_using_electric", tr("Plugged In"), QMetaMethod::fromSignal(&PowerWidget::requestShowUseElectric), nullptr, "pluggedIn"});
    //~ contents_path /power/On Battery
    //~ child_page On Battery
    m_menuIconText.append({"dcc_battery", tr("On Battery"), QMetaMethod::fromSignal(&PowerWidget::requestShowUseBattery), nullptr, "onBattery"});

    DStandardItem *item = nullptr;
    for (auto menu : m_menuIconText) {
        item = new DStandardItem(QIcon::fromTheme(menu.itemIcon), menu.itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_itemModel->appendRow(item);
        GSettingWatcher::instance()->bind(menu.gsettingsName, m_listView, item);
    }

    if (InsertPlugin::instance()->needPushPlugin("power"))
        InsertPlugin::instance()->pushPlugin(m_itemModel, m_menuIconText);
}

void PowerWidget::initConnections()
{
    connect(m_listView, &DListView::clicked, this, &PowerWidget::onItemClicked);
    connect(m_listView, &DListView::activated, m_listView, &QListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestShowSecondMenu, this, [ = ](int row) {
        if (row == m_batteryIndex && !this->m_bhasBattery) {
            m_listView->setRowHidden(row, true);
        }
    });
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [ = ](int row) {
        bool isAllHidden = true;
        for (int i = 0; i < m_itemModel->rowCount(); i++) {
            if (!m_listView->isRowHidden(i))
                isAllHidden = false;
        }

        if (m_listView->selectionModel()->selectedRows().size() > 0) {
            int index = m_listView->selectionModel()->selectedRows()[ 0 ].row();
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

void PowerWidget::initialize(bool hasBattery)
{
    m_bhasBattery = hasBattery;
    m_batteryIndex = IsServerSystem ? 1 : 2;
    m_listView->setRowHidden(m_batteryIndex, !hasBattery);
}

void PowerWidget::setModel(const PowerModel *model)
{
    m_model = model;
}

DListView *PowerWidget::getListViewPointer()
{
    return m_listView;
}

bool PowerWidget::getIsUseBattety()
{
    return m_bhasBattery;
}

void PowerWidget::showDefaultWidget()
{
    for (int i = 0; i < m_listView->model()->rowCount(); i++) {
        if (!m_listView->isRowHidden(i)) {
            m_listView->activated(m_listView->model()->index(i, 0));
            break;
        }
    }
}

void PowerWidget::onItemClicked(const QModelIndex &index)
{
    if (index == m_lastIndex)
        return;

    m_lastIndex = index;
    m_listView->setCurrentIndex(index);
    m_menuIconText[index.row()].itemSignal.invoke(m_menuIconText[index.row()].pulgin ? m_menuIconText[index.row()].pulgin : this);
    m_listView->resetStatus(index);
}

void PowerWidget::removeBattery(bool state)
{
    m_listView->setRowHidden(m_batteryIndex, !state);
    m_bhasBattery = state;

    /* 当电池被移除时，显示电池设置的上一个界面，如果电池在第一个，则显示电池界面移除后的第一个界面 */
    if (!state && m_lastIndex.row() == m_batteryIndex && m_batteryIndex > 0) {
        QModelIndex index = m_listView->model()->index(m_batteryIndex - 1, 0);
        onItemClicked(index);
    }
}
