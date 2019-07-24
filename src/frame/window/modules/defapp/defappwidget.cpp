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

#include "defappwidget.h"
#include "window/constant.h"
#include <QListView>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QListWidget>
#include <QDebug>

using namespace DCC_NAMESPACE::defapp;

DefaultAppsWidget::DefaultAppsWidget()
    : QWidget()
{
    setObjectName("Defapp");
    m_defAppCatView = new QListView(this);
    m_defAppCatView->setIconSize(QSize(Constant::HomeIconSize,Constant::HomeIconSize));
    m_defAppCatView->setSpacing(10);
    m_defAppCatView->setResizeMode(QListView::Adjust);
    m_defAppCatView->setMovement(QListView::Static);

    //Initialize second page view and model
    m_model = new QStandardItemModel(this);
    QStandardItem* item1    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Webpage"));
    QStandardItem* item2    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Mail"));
    QStandardItem* item3    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Text"));
    QStandardItem* item4    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Music"));
    QStandardItem* item5    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Video"));
    QStandardItem* item6    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Picture"));
    QStandardItem* item7    = new QStandardItem(QIcon(":/defapp/icons/select@2x.png"), tr("Terminal"));
    m_model->appendRow(item1);
    m_model->appendRow(item2);
    m_model->appendRow(item3);
    m_model->appendRow(item4);
    m_model->appendRow(item5);
    m_model->appendRow(item6);
    m_model->appendRow(item7);
    m_centralLayout = new QVBoxLayout(this);

    m_defAppCatView->setModel(m_model);
    m_defAppCatView->setEditTriggers(QAbstractItemView:: NoEditTriggers);

    m_centralLayout->addWidget(m_defAppCatView);


    setAccessibleName(tr("Default Applications"));
}

