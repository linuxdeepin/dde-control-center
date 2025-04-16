// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef AVATARLISTMODEL_H
#define AVATARLISTMODEL_H

#include <QAbstractListModel>
namespace dccV25 {
class User;
class AvatarTypesModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit AvatarTypesModel(QObject *parent = nullptr);

    enum AvatarRole {
        SectionRole = Qt::UserRole + 1
    };

    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
};

class AvatarListModel : public QAbstractListModel
{
public:
    explicit AvatarListModel(User *user, QObject *parent = nullptr);

    void setUser(User *user) {
        if (m_user == user)
            return;

        m_user = user;
    }
    // QAbstractItemModel interface
public:
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;

protected:
    User *m_user = nullptr;
};

}
#endif // AVATARLISTMODEL_H
