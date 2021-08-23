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
#include "window/gsettingwatcher.h"
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
    //~ child_page General
    menus << tr("General");
    //~ contents_path /personalization/Icon Theme
    //~ child_page Icon Theme
    menus << tr("Icon Theme");
    //~ contents_path /personalization/Cursor Theme
    //~ child_page Cursor Theme
    menus << tr("Cursor Theme");
    //~ contents_path /personalization/Font
    //~ child_page Font
    menus << tr("Font");

    QStringList icons;
    icons << "dcc_common"
          << "dcc_Icon_theme"
          << "dcc_cursor_theme"
          << "dcc_Font";

    QStringList personalizationGsetting;
    personalizationGsetting << "personalizationGeneral"
          << "personalizationIconTheme"
          << "personalizationCursorTheme"
          << "personalizationFont";


    if(InsertPlugin::instance()->needPushPlugin("personalization"))
        InsertPlugin::instance()->pushPlugin(m_model,m_itemList);

    m_categoryListView->setModel(m_model);
    m_categoryListView->setEditTriggers(QAbstractItemView:: NoEditTriggers);

    m_centralLayout->setMargin(0);
    m_centralLayout->addWidget(m_categoryListView);
    setLayout(m_centralLayout);
    connect(m_categoryListView, &QListView::clicked, this, &PersonalizationList::onCategoryClicked);
    connect(m_categoryListView, &DListView::activated, m_categoryListView, &QListView::clicked);
    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, [=](int row) {
         bool isAllHiden = true;
         for (int i = 0; i < m_model->rowCount(); i++) {
             if (!m_categoryListView->isRowHidden(i))
                 isAllHiden = false;
         }

         if (m_categoryListView->selectionModel()->selectedRows().size() > 0) {
             int index = m_categoryListView->selectionModel()->selectedRows()[0].row();
             Q_EMIT requestUpdateSecondMenu(index == row);
         } else {
             Q_EMIT requestUpdateSecondMenu(false);
         }

         if (isAllHiden) {
             m_lastIndex = QModelIndex();
             m_categoryListView->clearSelection();
         }
     });

     for (int i = 0; i < icons.size(); i++) {
         DStandardItem *item = new DStandardItem(QIcon::fromTheme(icons.at(i)), menus.at(i));
         item->setAccessibleText(menus.at(i));

         item->setData(VListViewItemMargin, Dtk::MarginsRole);
         m_model->appendRow(item);

         GSettingWatcher::instance()->bind(personalizationGsetting.at(i), m_categoryListView, item);
     }

     m_itemList.push_back({icons.at(0),menus.at(0),QMetaMethod::fromSignal(&PersonalizationList::requestShowGeneral),nullptr,personalizationGsetting.at(0)});
     m_itemList.push_back({icons.at(1),menus.at(1),QMetaMethod::fromSignal(&PersonalizationList::requestShowIconTheme),nullptr,personalizationGsetting.at(1)});
     m_itemList.push_back({icons.at(2),menus.at(2),QMetaMethod::fromSignal(&PersonalizationList::requestShowCursorTheme),nullptr,personalizationGsetting.at(2)});
     m_itemList.push_back({icons.at(3),menus.at(3),QMetaMethod::fromSignal(&PersonalizationList::requestShowFonts),nullptr,personalizationGsetting.at(3)});

    //set default show page
    m_categoryListView->setCurrentIndex(m_model->indexFromItem(m_model->item(0)));
}

PersonalizationList::~PersonalizationList()
{

}

void PersonalizationList::onCategoryClicked(const QModelIndex &index)
{
    if (index == m_lastIndex) return;

    m_lastIndex = index;

    m_itemList[index.row()].itemSignal.invoke(m_itemList[index.row()].pulgin ? m_itemList[index.row()].pulgin : this);

    m_categoryListView->resetStatus(index);
}

void PersonalizationList::setCurrentIndex(int row)
{
    if (row > m_model->rowCount())
        row = 0;
    m_lastIndex = m_model->indexFromItem(m_model->item(row));
    m_categoryListView->setCurrentIndex(m_lastIndex);
}

void PersonalizationList::showDefaultWidget()
{
    for (int i = 0; i < m_model->rowCount(); i++) {
        if (!m_categoryListView->isRowHidden(i)) {
            m_categoryListView->activated(m_model->index(i, 0));
            break;
        }
    }
}

