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
#include "useritem.h"
#include "useritempic.h"
#include "onlineicon.h"

#include <DIconButton>
#include <DLabel>

#include <QVBoxLayout>
#include <QLabel>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

UserItem::UserItem(QWidget *parent)
    : QWidget(parent)
    , m_onlineIcon(nullptr)
{
    m_mainLayout = new  QVBoxLayout();
    m_mainLayout->setMargin(0);

    m_title = new DLabel(this);
    m_title->setAlignment(Qt::AlignCenter);
    m_title->setElideMode(Qt::TextElideMode::ElideRight);
    m_itemPic = new UserItemPic(this);
    m_onlineIcon = new OnlineIcon(this);
    m_onlineIcon->setFixedSize(9, 9);
    m_onlineIcon->setVisible(true);

    connect(m_itemPic, &UserItemPic::clicked, this, [=] {
        Q_EMIT selectedChanged(true);
    });

    //icon themes and cursor thems
    QHBoxLayout *titlebuttonLayout = new QHBoxLayout();
    titlebuttonLayout->addWidget(m_onlineIcon, Qt::AlignLeft);
    titlebuttonLayout->addWidget(m_title, Qt::AlignCenter);
    m_mainLayout->addWidget(m_itemPic,Qt::AlignVCenter);
    m_mainLayout->addLayout(titlebuttonLayout);
    m_mainLayout->setSpacing(5);
    setLayout(m_mainLayout);
}

void UserItem::setTitle(const QString &title)
{
    m_title->setText(title);
    update();
}

void UserItem::setSelected(bool selected)
{
    m_itemPic->setSelected(selected);
    update();
}

void UserItem::setOnline(bool online)
{
    m_onlineIcon->setVisible(online);
    update();
}

void UserItem::setPic(const QString &picPath)
{
    m_itemPic->setPath(picPath);
    update();
}

void UserItem::setId(const QVariant &id)
{
    m_id = id;
}
