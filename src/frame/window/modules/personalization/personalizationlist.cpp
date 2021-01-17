/*
 * Copyright (C) 2017 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     LiLinling <lilinling_cm@deepin.com>
 *
 * Maintainer: LiLinling <lilinling_cm@deepin.com>
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
#include "personalizationlist.h"
#include "window/utils.h"
#include "widgets/multiselectlistview.h"
#include <DListView>

#include <QVBoxLayout>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

DWIDGET_USE_NAMESPACE

PersonalizationList::PersonalizationList(QWidget *parent)
    : QWidget(parent)
    , m_categoryListView(new dcc::widgets::MultiSelectListView())
    , m_model(new QStandardItemModel(this))
    , m_centralLayout(new QVBoxLayout())
{
    m_categoryListView->setAccessibleName("List_categorylist");
    m_categoryListView->setResizeMode(QListView::Adjust);
    m_categoryListView->setMovement(QListView::Static);
    m_categoryListView->setFrameShape(QFrame::NoFrame);
    m_categoryListView->setViewportMargins(ScrollAreaMargins);
    m_categoryListView->setIconSize(ListViweIconSize);

    //Initialize second page view and model
    QStringList menus;
    //~ contents_path /personalization/General
    menus << tr("General");
    //~ contents_path /personalization/Icon Theme
    menus << tr("Icon Theme");
    //~ contents_path /personalization/Cursor Theme
    menus << tr("Cursor Theme");
    //~ contents_path /personalization/Font
    menus << tr("Font");

    QStringList icons;
    icons << "dcc_common"
          << "dcc_Icon_theme"
          << "dcc_cursor_theme"
          << "dcc_Font";

    for (int i = 0; i < menus.size(); ++i) {
        DStandardItem *item = new DStandardItem(QIcon::fromTheme(icons.at(i)), menus.at(i));
        item->setAccessibleText(menus.at(i));

        item->setData(VListViewItemMargin, Dtk::MarginsRole);
        m_model->appendRow(item);
    }

    m_itemList.push_back({icons.at(0),menus.at(0),QMetaMethod::fromSignal(&PersonalizationList::requestShowGeneral)});
    m_itemList.push_back({icons.at(1),menus.at(1),QMetaMethod::fromSignal(&PersonalizationList::requestShowIconTheme)});
    m_itemList.push_back({icons.at(2),menus.at(2),QMetaMethod::fromSignal(&PersonalizationList::requestShowCursorTheme)});
    m_itemList.push_back({icons.at(3),menus.at(3),QMetaMethod::fromSignal(&PersonalizationList::requestShowFonts)});

    if(InsertPlugin::instance()->needPushPlugin("personalization"))
        InsertPlugin::instance()->pushPlugin(m_model,m_itemList);

    m_categoryListView->setModel(m_model);
    m_categoryListView->setEditTriggers(QAbstractItemView:: NoEditTriggers);

    m_centralLayout->setMargin(0);
    m_centralLayout->addWidget(m_categoryListView);
    setLayout(m_centralLayout);
    connect(m_categoryListView, &QListView::clicked, this, &PersonalizationList::onCategoryClicked);
    connect(m_categoryListView, &DListView::activated, m_categoryListView, &QListView::clicked);
    //set default show page
    m_categoryListView->setCurrentIndex(m_model->indexFromItem(m_model->item(0)));
}

void PersonalizationList::setDefaultWidget()
{
    m_itemList[0].itemSignal.invoke(m_itemList[0].plugin ? m_itemList[0].plugin : this);
}

void PersonalizationList::onCategoryClicked(const QModelIndex &index)
{
    if (index == m_lastIndex) return;

    m_lastIndex = index;

    m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].plugin ? m_itemList[index.row()].plugin : this);

    m_categoryListView->resetStatus(index);
}

void PersonalizationList::setCurrentIndex(int row)
{
    if (row > m_model->rowCount())
        row = 0;
    m_lastIndex = m_model->indexFromItem(m_model->item(row));
    m_categoryListView->setCurrentIndex(m_lastIndex);
}
