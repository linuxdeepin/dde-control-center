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

    switch (role) {
    case Qt::DisplayRole:
        return controller->userName(id);
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
