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

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QPixmap>
#include <QList>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QDebug>

using namespace DCC_NAMESPACE::accounts;

AvatarListWidget::AvatarListWidget(QWidget *parent)
    : QWidget(parent)
    , m_mainContentLayout(new QVBoxLayout())
    , m_avatarListView(new QListView())
    , m_avatarItemModel(new QStandardItemModel())
    , m_avatarItemDelegate(new AvatarItemDelegate())
    , m_prevSelectIndex(-1)
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
    QString dirpath("/var/lib/AccountsService/icons/");
    QDir dir(dirpath);
    QStringList hideList;
    hideList << "default.png" << "guest.png";
    QStringList filters;
    filters << "*.png";//设置过滤类型
    dir.setNameFilters(filters);//设置文件名的过滤
    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        if (hideList.contains(list.at(i).fileName())) {
            continue;
        }
        QString iconpath = list.at(i).filePath();
        m_iconpathList.push_back(iconpath);
        QStandardItem *item = new QStandardItem();
        item->setData(QVariant::fromValue(QPixmap(iconpath)), Qt::DecorationRole);
        m_avatarItemModel->appendRow(item);
    }

    QStandardItem *item = new QStandardItem();
    item->setData(QVariant::fromValue(true), AvatarListWidget::AddAvatarRole);
    m_avatarItemModel->appendRow(item);

    m_avatarListView->setItemDelegate(m_avatarItemDelegate);
    m_avatarListView->setModel(m_avatarItemModel);
}

void AvatarListWidget::setUserModel(dcc::accounts::User *user)
{
    m_curUser = user;
}

void AvatarListWidget::onItemClicked(const QModelIndex &index)
{
    if (index.data(AvatarListWidget::AddAvatarRole).toBool() == true) {
        Q_EMIT requestAddNewAvatar(m_curUser);
    } else {
        if (m_prevSelectIndex != -1) {
            m_avatarItemModel->item(m_prevSelectIndex)->setCheckState(Qt::Unchecked);
        }
        m_prevSelectIndex = index.row();
        m_avatarItemModel->item(index.row())->setCheckState(Qt::Checked);
        Q_EMIT requestSetAvatar(m_iconpathList.at(index.row()));
    }
    update();
}
