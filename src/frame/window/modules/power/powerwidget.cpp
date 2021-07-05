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
#include "window/utils.h"
#include "modules/power/powermodel.h"
#include "modules/mouse/widget/palmdetectsetting.h"
#include "modules/mouse/widget/doutestwidget.h"
#include "widgets/multiselectlistview.h"
#include <DStyleOption>

#include <QTimer>
#include <QVBoxLayout>
#include <QStandardItemModel>

using namespace dcc::power;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::power;

PowerWidget::PowerWidget(QWidget *parent)
    : QWidget(parent)
    , m_listview(new dcc::widgets::MultiSelectListView(this))
    , m_model(nullptr)
    , m_bhasBattery(false)
{

}

PowerWidget::~PowerWidget()
{

}

void PowerWidget::initialize(bool hasBattery)
{
    m_bhasBattery = hasBattery;

    //平板项目需求，隐藏通用和使用电源选项
    if (!IsServerSystem) {
        m_menuIconText = {
    #ifndef USE_TABLET
            //~ contents_path /power/General
            {"dcc_general_purpose", tr("General"), QMetaMethod::fromSignal(&PowerWidget::requestShowGeneral)},
            //~ contents_path /power/Plugged In
            {"dcc_using_electric", tr("Plugged In"), QMetaMethod::fromSignal(&PowerWidget::requestShowUseElectric)},
    #endif
            {"dcc_battery", tr("On Battery"), QMetaMethod::fromSignal(&PowerWidget::requestShowUseBattery)},
        };
    } else {
        m_menuIconText = {
    #ifndef USE_TABLET
            //~ contents_path /power/Plugged In
            {"dcc_using_electric", tr("Plugged In"), QMetaMethod::fromSignal(&PowerWidget::requestShowUseElectric)},
    #endif
            {"dcc_battery", tr("On Battery"), QMetaMethod::fromSignal(&PowerWidget::requestShowUseBattery)},
        };
    }

    auto model = new QStandardItemModel(this);
    DStandardItem *item = nullptr;
    for (auto menu : m_menuIconText) {
        item = new DStandardItem(QIcon::fromTheme(menu.itemIcon),menu.itemText);
        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        model->appendRow(item);
    }

    if(InsertPlugin::instance()->needPushPlugin("power"))
        InsertPlugin::instance()->pushPlugin(model,m_menuIconText);

    m_listview->setFrameShape(QFrame::NoFrame);
    m_listview->setModel(model);
    m_listview->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_listview->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_listview->setViewportMargins(ScrollAreaMargins);
    m_listview->setIconSize(ListViweIconSize);
    if (!IsServerSystem)
        m_listview->setRowHidden(2, !hasBattery);
    else {
        m_listview->setRowHidden(1, !hasBattery);
    }

    connect(m_listview, &DListView::clicked, this, &PowerWidget::onItemClicked);
    connect(m_listview, &DListView::activated, m_listview, &QListView::clicked);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(m_listview);
    setLayout(layout);
}

void PowerWidget::setModel(const PowerModel *model)
{
    m_model = model;
}

DListView *PowerWidget::getListViewPointer()
{
    return m_listview;
}

bool PowerWidget::getIsUseBattety()
{
    return m_bhasBattery;
}

void PowerWidget::setDefaultWidget()
{
    m_listview->setCurrentIndex(m_listview->model()->index(0, 0));
    m_listview->clicked(m_listview->model()->index(0, 0));
}

void PowerWidget::onItemClicked(const QModelIndex &index)
{
    if (index == m_lastIndex) return;

    m_lastIndex = index;
    m_listview->setCurrentIndex(index);
    m_menuIconText[index.row()].itemSignal.invoke(m_menuIconText[index.row()].pulgin ? m_menuIconText[index.row()].pulgin : this);
    m_listview->resetStatus(index);
}

void PowerWidget::removeBattery(bool state)
{
    m_listview->setRowHidden(2, !state);
    m_bhasBattery = state;
    Q_EMIT requestShowUseElectric();
}
