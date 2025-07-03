// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "avatarlistmodel.h"
#include "user.h"

using namespace dccV25;

static QStringList g_avatarTypes {
    "icons/human/dimensional",
    "icons/human/flat",
    "icons/anmal",
    "icons/emoji",
    "icons/illustration",
    "icons/local"
};

AvatarTypesModel::AvatarTypesModel(QObject *)
{

}

int AvatarTypesModel::rowCount(const QModelIndex &) const
{
    return g_avatarTypes.count();
}

QVariant AvatarTypesModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= g_avatarTypes.size())
        return QVariant();

    const QString &avatarType = g_avatarTypes.value(index.row());

    QString section;
    if (avatarType.contains("human/dimensional")) {
        section = tr("Dimensional");
    } else if (avatarType.contains("human/flat")) {
        section = tr("Flat");
    } else {
        // none
    }

    switch (role) {
    case Qt::DisplayRole:
        return avatarType;
    case SectionRole:
        return section;
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> AvatarTypesModel::roleNames() const
{
    auto names = QAbstractListModel::roleNames();
    names[SectionRole] = "section";
    return names;
}

AvatarListModel::AvatarListModel(User *user, QObject *parent)
    : QAbstractListModel(parent)
    , m_user(user)
{
}

int AvatarListModel::rowCount(const QModelIndex &) const
{
    if (!m_user)
        return 0;

    return m_user->avatars().count();
}

QVariant AvatarListModel::data(const QModelIndex &index, int role) const
{
    if (!m_user)
        return QVariant();

    const auto &list = m_user->avatars();
    if (!index.isValid() || index.row() >= list.size())
        return QVariant();

    const QString &iconUrl = list.value(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return iconUrl;
    default:
        break;
    }

    return QVariant();
}
