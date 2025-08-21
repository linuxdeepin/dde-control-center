// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "accountlistmodel.h"
#include "accountscontroller.h"

using namespace dccV25;
AccountListModel::AccountListModel(QObject *parent)
    : QAbstractListModel(parent)
{
}

void AccountListModel::reset()
{
    beginResetModel();
    endResetModel();
}

int AccountListModel::rowCount(const QModelIndex &) const
{
    AccountsController *controller = dynamic_cast<AccountsController *>(parent());
    if (!controller)
        return 0;

    return controller->userIdList().count();
}

QVariant AccountListModel::data(const QModelIndex &index, int role) const
{
    AccountsController *controller = dynamic_cast<AccountsController *>(parent());
    if (!controller)
        return QVariant();

    const QStringList &ids = controller->userIdList();
    const QString &id = ids.value(index.row());
    QString name = controller->fullName(id);
    if (name.isEmpty())
        name = controller->userName(id);

    switch (role) {
    case Qt::DisplayRole:
        return name;
    case UserIdRole:
        return id;
    case UserTypeRole:
        return controller->userTypeName(id);
    case AvatarRole:
        return controller->avatar(id);
    case OnlineRole:
        return controller->isOnline(id);
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> AccountListModel::roleNames() const
{
    auto names = QAbstractListModel::roleNames();
    names[UserIdRole] = "userId";
    names[UserTypeRole] = "userType";
    names[AvatarRole] = "avatar";
    names[OnlineRole] = "online";
    return names;
}

//////////////////
/// GroupListModel
GroupListModel::GroupListModel(const QString &id, QObject *parent)
    :QAbstractListModel(parent)
    , m_userId(id)
{

    AccountsController *controller = dynamic_cast<AccountsController *>(parent);
    if (controller) {
        m_groups = controller->groups(m_userId);
        connect(controller, &AccountsController::groupsUpdateFailed, this, [this](const QString &groupName) {
            int idx = m_groups.indexOf(groupName);
            if (idx > 0)
                Q_EMIT dataChanged(index(idx), index(idx));
        });
        connect(controller, &AccountsController::groupsChanged, this, [this, controller](const QString &userId, const QStringList &) {
            if (userId == m_userId)
                updateGroups(controller->groups(userId));
        });
        connect(controller, &AccountsController::groupsUpdate, this, [this, controller]() {
            updateGroups(controller->groups(m_userId));
        });
        connect(controller, &AccountsController::requestCreateGroup, this, [this, controller](const QString &userId) {
            if (userId != m_userId)
                return;

            if (m_groups.count() > 1 && m_groups.last().isEmpty())
                return;

            m_isCreatingGroup = true;
            int index = m_groups.count();
            beginInsertRows(QModelIndex(), index, index);
            m_groups << "";
            endInsertRows();
            Q_EMIT groupsUpdated();
        });
        connect(controller, &AccountsController::requestClearEmptyGroup, this, [this, controller](const QString &userId) {
            if (userId != m_userId)
                return;

            if (m_groups.isEmpty())
                return;

            if (!m_groups.last().isEmpty())
                return;

            int index = m_groups.count() - 1;
            beginRemoveRows(QModelIndex(), index, index);
            m_groups.removeLast();
            endRemoveRows();
        });
    }
}

void GroupListModel::setUserId(const QString &id)
{
    if (m_userId == id)
        return;

    AccountsController *controller = dynamic_cast<AccountsController *>(parent());
    if (!controller)
        return;

    m_userId = id;

    updateGroups(controller->groups(id));
}

void GroupListModel::updateGroups(const QStringList &groups) {
    if (m_groups == groups)
        return;

    beginResetModel();
    m_groups = groups;
    endResetModel();

    Q_EMIT groupsUpdated();
}

int GroupListModel::rowCount(const QModelIndex &) const
{
    if (m_userId.isEmpty())
        return 0;

    return m_groups.count();
}

QVariant GroupListModel::data(const QModelIndex &index, int role) const
{
    AccountsController *controller = dynamic_cast<AccountsController *>(parent());
    if (!controller)
        return QVariant();

    const QString groupName = m_groups.value(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return groupName;
    case UserIdRole:
        return m_userId;
    case EditAbleRole:
        return controller->groupEditAble(m_userId, groupName);
    case EnableRole:
        return controller->groupEnabled(m_userId, groupName);
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> GroupListModel::roleNames() const
{
    auto names = QAbstractListModel::roleNames();
    names[UserIdRole] = "userId";
    names[EditAbleRole] = "groupEditAble";
    names[EnableRole] = "groupEnabled";
    return names;
}
