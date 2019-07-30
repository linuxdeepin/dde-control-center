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

#include "avatarlistwidget.h"
#include "modules/accounts/user.h"
#include "avataritemdelegate.h"

#include <QWidget>;
#include <QListView>;
#include <QStandardItemModel>;
#include <QVBoxLayout>;
#include <QHBoxLayout>;
#include <QPushButton>;
#include <QLabel>;
#include <QDebug>;


using namespace DCC_NAMESPACE::accounts;

AvatarListWidget::AvatarListWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout())
    , m_avatarListView(new QListView())
    , m_avatarItemModel(new QStandardItemModel())
    , m_avatarItemDelegate(new AvatarItemDelegate())
{
    initWidgets();
    initDatas();

}

void AvatarListWidget::initWidgets()
{
    setLayout(m_mainContentLayout);

    m_mainContentLayout->addWidget(m_avatarListView, 0, Qt::AlignCenter);
    m_mainContentLayout->addStretch();

    m_avatarListView->setViewMode(QListView::IconMode);
    m_avatarListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    m_avatarListView->setDragDropMode(QAbstractItemView::NoDragDrop);
    m_avatarListView->setDragEnabled(false);
    m_avatarListView->setSpacing(15);

    connect(m_avatarListView, &QListView::clicked, this, &AvatarListWidget::onItemClicked);
}

void AvatarListWidget::initDatas()
{
    for (int i = 0; i < 14; i++) {
        QStandardItem *item = new QStandardItem();
        QString iconpath = QString::asprintf("/var/lib/AccountsService/icons/%d.png", i + 1);
        item->setData(QVariant::fromValue(iconpath), Qt::UserRole + 1);
        m_avatarItemModel->appendRow(item);
    }

    m_avatarListView->setItemDelegate(m_avatarItemDelegate);
    m_avatarListView->setModel(m_avatarItemModel);
}

void AvatarListWidget::onItemClicked(const QModelIndex &index)
{
    QString iconPath = index.data(Qt::UserRole + 1).value<QString>();
    Q_EMIT requestSetAvatar(iconPath);
}
