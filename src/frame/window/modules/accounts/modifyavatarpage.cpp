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

#include "modifyavatarpage.h"
#include "modules/accounts/user.h"
#include "modules/accounts/avatarwidget.h"
#include "avataritemdelegate.h"

#include <QWidget>
#include <QListView>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QLabel>


using namespace DCC_NAMESPACE::accounts;

ModifyAvatarPage::ModifyAvatarPage(dcc::accounts::User *user, QWidget *parent)
    : QWidget(parent)
    , m_curUser(user)
    , m_mainContentLayout(new QVBoxLayout(this))
    , m_headLayout(new QVBoxLayout)
    , m_avatarlistLayout(new QVBoxLayout)
    , m_avatar(new dcc::accounts::AvatarWidget)
    , m_shortName(new QLabel)
    , m_fullName(new QLabel)
    , m_avatarListView(new QListView(this))
    , m_avatarItemModel(new QStandardItemModel())
    , m_avatarItemDelegate(new AvatarItemDelegate())
{
    initWidgets();
    initDatas();

}

void ModifyAvatarPage::initWidgets()
{
    m_shortName->setFixedHeight(20);
    m_fullName->setFixedHeight(20);
    m_avatar->setAlignment(Qt::AlignHCenter);

    m_headLayout->addWidget(m_avatar, 0, Qt::AlignHCenter);
    m_headLayout->addWidget(m_shortName, 0, Qt::AlignHCenter);
    m_headLayout->addWidget(m_fullName, 0, Qt::AlignHCenter);

    m_avatarlistLayout->addWidget(m_avatarListView, 0, Qt::AlignCenter);

    m_mainContentLayout->addLayout(m_headLayout);
    m_mainContentLayout->addLayout(m_avatarlistLayout);
    m_mainContentLayout->addStretch();

    m_avatarListView->setViewMode(QListView::IconMode);
    m_avatarListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    //m_avatarListView->setStyleSheet(QString("QListView::item{ background-color:red; padding:5px; margin:10px; }"));
    qDebug() << Q_FUNC_INFO << m_avatarListView->rect();
}

void ModifyAvatarPage::initDatas()
{
    //use m_curUser fill widget data
    m_avatar->setAvatarPath(m_curUser->currentAvatar());
    m_avatar->resize(640, 480);
    m_shortName->setText(m_curUser->name());
    m_fullName->setText(m_curUser->fullname());

    for(int i=0; i<14; i++) {
        QStandardItem* item = new QStandardItem();
        QString iconpath = QString::asprintf("/var/lib/AccountsService/icons/%d.png", i+1);
        item->setData(QVariant::fromValue(iconpath), Qt::UserRole+1);
        m_avatarItemModel->appendRow(item);
    }

    m_avatarListView->setItemDelegate(m_avatarItemDelegate);
    m_avatarListView->setModel(m_avatarItemModel);
}

