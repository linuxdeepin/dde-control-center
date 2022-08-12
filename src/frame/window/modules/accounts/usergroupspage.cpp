/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     dengbo <dengbo@uniontech.com>
 *
 * Maintainer: dengbo <dengbo@uniontech.com>
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

#include "usergroupspage.h"
#include "groupitem.h"

#include <QVBoxLayout>

#include <grp.h>

using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

DWIDGET_USE_NAMESPACE

UserGroupsPage::UserGroupsPage(User *user, dcc::accounts::UserModel *userModel, QWidget *parent)
    : QWidget(parent)
    , m_groupTip(new QLabel(tr("Group")))
    , m_layout (new QVBoxLayout(this))
    , m_curUser(user)
    , m_userModel(userModel)
    , m_groupListView(new DListView(this))
    , m_groupItemModel(new QStandardItemModel(this))
{
    initWidget();
    initData();
}

UserGroupsPage::~UserGroupsPage()
{

}

void UserGroupsPage::changeUserGroup(const QStringList &groups)
{
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        item->setCheckState(item && groups.contains(item->text()) ? Qt::Checked : Qt::Unchecked);
        item->setEnabled(item->text() != m_groupName);
    }
    m_groupItemModel->sort(0);
}

void UserGroupsPage::userGroupClicked(const QModelIndex &index)
{
    QStandardItem *item = m_groupItemModel->item(index.row(), index.column());
    //不可移除主组
    if (!item || item->text() == m_groupName)
        return;

    QStringList curUserGroup;
    int row_count = m_groupItemModel->rowCount();
    for (int i = 0; i < row_count; ++i) {
        QStandardItem *itemGroup = m_groupItemModel->item(i, 0);
        if (itemGroup && itemGroup->checkState()) {
            curUserGroup << itemGroup->text();
        }
    }

    Qt::CheckState state = item->checkState();
    state == Qt::Checked ? (void)curUserGroup.removeOne(item->text()) : curUserGroup.append(item->text());

    Q_EMIT requestSetGroups(m_curUser, curUserGroup);
}

void UserGroupsPage::setGroupInfo(const QStringList &group)
{
    for (const QString &item : group) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        m_groupItemModel->appendRow(it);
    }
    changeUserGroup(m_curUser->groups());
}

void UserGroupsPage::onGidChanged(const QString &gid)
{
    bool ok;
    int iGid = gid.toInt(&ok, 10);
    if (!ok)
        return;

    const group *group = getgrgid(static_cast<__gid_t>(iGid));
    if (nullptr == group || nullptr == group->gr_name)
        return;

    m_groupName = QString(group->gr_name);
    for (int i = 0; i < m_groupItemModel->rowCount(); ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        if (nullptr == item)
            continue;
        item->setEnabled(item->text() != m_groupName);
    }
}

void UserGroupsPage::initWidget()
{
    this->setAccessibleName("UserGroupsPage");
    m_groupListView->setModel(m_groupItemModel);
    m_groupListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_groupListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_groupListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_groupListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_groupListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    DFontSizeManager::instance()->bind(m_groupTip, DFontSizeManager::T5, QFont::DemiBold);

    m_layout->addWidget(m_groupTip);
    m_layout->addSpacing(10);
    m_layout->addWidget(m_groupListView);
    QHBoxLayout *layout = new QHBoxLayout(this);
    QWidget *widget = new QWidget;
    widget->setLayout(m_layout);
    layout->addWidget(widget);
    setLayout(layout);
}

void UserGroupsPage::initData()
{
    QStringList userGroup = m_userModel->getAllGroups();

    m_groupItemModel->clear();
    for (QString item : userGroup) {
        GroupItem *it = new GroupItem(item);
        it->setCheckable(false);
        m_groupItemModel->appendRow(it);
    }

    changeUserGroup(m_curUser->groups());
    onGidChanged(m_curUser->gid());

    connect(m_curUser, &User::groupsChanged, this, &UserGroupsPage::changeUserGroup);
    connect(m_curUser, &User::gidChanged, this, &UserGroupsPage::onGidChanged);
    connect(m_userModel, &UserModel::allGroupsChange, this, &UserGroupsPage::setGroupInfo);
    connect(m_groupListView, &DListView::clicked, this, &UserGroupsPage::userGroupClicked);
}

