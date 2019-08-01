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
#include "window/constant.h"

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::personalization;

PersonalizationList::PersonalizationList(QWidget *parent) : QWidget(parent)
{
    setObjectName("personanization");
    m_categoryListView = new QListView(this);
    m_categoryListView->setIconSize(QSize(Constant::HomeIconSize,Constant::HomeIconSize));
    m_categoryListView->setSpacing(10);
    m_categoryListView->setResizeMode(QListView::Adjust);
    m_categoryListView->setMovement(QListView::Static);

    //Initialize second page view and model
    m_model = new QStandardItemModel(this);
    QStandardItem* item1    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("General"));
    QStandardItem* item2    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Icon Themes"));
    QStandardItem* item3    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Cursor Themes"));
    QStandardItem* item4    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Fonts"));

    m_model->appendRow(item1);
    m_model->appendRow(item2);
    m_model->appendRow(item3);
    m_model->appendRow(item4);
    m_centralLayout = new QVBoxLayout(this);

    m_categoryListView->setModel(m_model);
    m_categoryListView->setEditTriggers(QAbstractItemView:: NoEditTriggers);

    m_centralLayout->addWidget(m_categoryListView);
    connect(m_categoryListView, &QListView::clicked, this, &PersonalizationList::onCategoryClicked);
}

void PersonalizationList::onCategoryClicked(const QModelIndex &index)
{
    switch (index.row()) {
    case 0:
        Q_EMIT requestShowGeneral();
        break;
    case 1:
        Q_EMIT requestShowIconTheme();
        break;
    case 2:
        Q_EMIT requestShowCursorTheme();
        break;
    case 3:
        Q_EMIT requestShowFonts();
        break;
    default:
        break;
    }
}

