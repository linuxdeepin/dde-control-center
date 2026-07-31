// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "accountlistmodel.h"
#include "accountscontroller.h"

namespace
{
    constexpr const char* const kTypeHeader = "header";
    constexpr const char* const kTypeFooter = "footer";
    constexpr const char* const kTypeGroup = "group";
}

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

/////////////////////////
/// GroupListProxyModel
GroupListProxyModel::GroupListProxyModel(QObject *parent)
    : QAbstractProxyModel(parent)
{
}

void GroupListProxyModel::setSourceModel(QAbstractItemModel *model)
{
    auto *oldModel = sourceModel();
    if (oldModel == model)
        return;

    beginResetModel();

    if (oldModel) {
        disconnect(oldModel, nullptr, this, nullptr);
    }

    QAbstractProxyModel::setSourceModel(model);

    if (auto *newModel = sourceModel()) {
        auto *groupModel = qobject_cast<GroupListModel *>(newModel);
        if (groupModel) {
            connect(groupModel, &GroupListModel::groupsUpdated, this, &GroupListProxyModel::groupsUpdated);
        }
        connect(newModel, &QAbstractItemModel::rowsAboutToBeInserted, this, [this](const QModelIndex &, int first, int last) {
            beginInsertRows(QModelIndex(), toProxyRow(first), toProxyRow(last));
        });
        connect(newModel, &QAbstractItemModel::rowsInserted, this, [this](const QModelIndex &, int, int) {
            endInsertRows();
        });
        connect(newModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, [this](const QModelIndex &, int first, int last) {
            beginRemoveRows(QModelIndex(), toProxyRow(first), toProxyRow(last));
        });
        connect(newModel, &QAbstractItemModel::rowsRemoved, this, [this](const QModelIndex &, int, int) {
            endRemoveRows();
        });
        connect(newModel, &QAbstractItemModel::dataChanged, this, [this](const QModelIndex &topLeft, const QModelIndex &bottomRight, const QList<int> &roles) {
            Q_EMIT dataChanged(index(toProxyRow(topLeft.row()), 0), index(toProxyRow(bottomRight.row()), 0), roles);
        });
        connect(newModel, &QAbstractItemModel::modelAboutToBeReset, this, [this]() {
            beginResetModel();
        });
        connect(newModel, &QAbstractItemModel::modelReset, this, [this]() {
            endResetModel();
        });
    }

    endResetModel();
}

QModelIndex GroupListProxyModel::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid() || !sourceModel())
        return QModelIndex();

    int row = proxyIndex.row();
    if (isHeaderRow(row) || isFooterRow(row))
        return QModelIndex();

    return sourceModel()->index(toSourceRow(row), proxyIndex.column());
}

QModelIndex GroupListProxyModel::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid() || !sourceModel())
        return QModelIndex();

    return createIndex(toProxyRow(sourceIndex.row()), sourceIndex.column());
}

int GroupListProxyModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !sourceModel())
        return 0;

    return sourceModel()->rowCount() + 2;
}

int GroupListProxyModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !sourceModel())
        return 0;

    return sourceModel()->columnCount();
}

QModelIndex GroupListProxyModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();

    if (row < 0 || row >= rowCount() || column < 0)
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex GroupListProxyModel::parent(const QModelIndex &) const
{
    return QModelIndex();
}

QVariant GroupListProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    int row = index.row();

    if (role == TypeRole) {
        if (isHeaderRow(row))
            return QString::fromUtf8(kTypeHeader);
        if (isFooterRow(row))
            return QString::fromUtf8(kTypeFooter);
        return QString::fromUtf8(kTypeGroup);
    }

    if (isHeaderRow(row) || isFooterRow(row))
        return QVariant();

    QModelIndex sourceIndex = mapToSource(index);
    if (!sourceIndex.isValid())
        return QVariant();

    return sourceModel()->data(sourceIndex, role);
}

QHash<int, QByteArray> GroupListProxyModel::roleNames() const
{
    auto names = sourceModel() ? sourceModel()->roleNames() : QHash<int, QByteArray>();
    names[TypeRole] = "type";
    return names;
}

bool GroupListProxyModel::isCreatingGroup() const
{
    auto *groupModel = qobject_cast<GroupListModel *>(sourceModel());
    return groupModel ? groupModel->isCreatingGroup() : false;
}

void GroupListProxyModel::setCreatingGroup(bool isCreatingGroup)
{
    auto *groupModel = qobject_cast<GroupListModel *>(sourceModel());
    if (groupModel)
        groupModel->setCreatingGroup(isCreatingGroup);
}

int GroupListProxyModel::toProxyRow(int sourceRow) const 
{
    return sourceRow + 1;
}

int GroupListProxyModel::toSourceRow(int proxyRow) const 
{
    return proxyRow - 1;
}

bool GroupListProxyModel::isHeaderRow(int row) const 
{
    return row < 1;
}

bool GroupListProxyModel::isFooterRow(int row) const
{
    return row >= rowCount() - 1;
}
