// SPDX-FileCopyrightText: 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#include "usergroupspage.h"
#include "groupitem.h"
#include "widgets/translucentframe.h"

#include <QVBoxLayout>

#include <grp.h>

using namespace dcc;
using namespace dcc::widgets;
using namespace dcc::accounts;
using namespace DCC_NAMESPACE::accounts;

const QString Sudo = "sudo";

DWIDGET_USE_NAMESPACE

UserGroupsPage::UserGroupsPage(User *user, dcc::accounts::UserModel *userModel, ContentWidget *parent)
    : ContentWidget(parent)
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

    if (!(item && item->isEnabled()))
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
    int managerAccountsCount = getAdministratorAccountsCount();
    for (int i = 0; i < m_groupItemModel->rowCount(); ++i) {
        QStandardItem *item = m_groupItemModel->item(i, 0);
        if (nullptr == item)
            continue;
        bool value = false;
        do {
            if (item->text() == m_groupName) {
                break;
            }
            if (item->text() == Sudo) {
                if (!(m_curUser && m_curUser->online()) && !(managerAccountsCount == 1 && m_curUser->userType() == User::UserType::Administrator)) {
                    value = true;
                }
            } else {
                value = true;
            }
        } while (0);
        item->setEnabled(value);
    }
}

void UserGroupsPage::initWidget()
{
    //~ contents_path /accounts/Group
    m_groupTip = new QLabel(tr("Group"));
    this->setAccessibleName("UserGroupsPage");
    m_groupListView->setModel(m_groupItemModel);
    m_groupListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_groupListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_groupListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_groupListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    DFontSizeManager::instance()->bind(m_groupTip, DFontSizeManager::T5, QFont::DemiBold);

    TranslucentFrame * mainWidget = new TranslucentFrame(this);
    setContentsMargins(0, 10, 0, 10);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->setMargin(0);

    m_groupTip->setContentsMargins(10, 10, 10, 10);
    m_layout->addWidget(m_groupTip);

    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(10, 0, 10, 0);
    vLayout->addWidget(m_groupListView);
    m_layout->addLayout(vLayout);

    mainWidget->setLayout(m_layout);
    setContent(mainWidget);
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

int UserGroupsPage::getAdministratorAccountsCount()
{
    if (!m_userModel) {
        return 0;
    }
    int count = 0;
    for (auto data : m_userModel->userList()) {
        if (data->userType() == User::UserType::Administrator) {
            count++;
        }
    }
    return count;
}
