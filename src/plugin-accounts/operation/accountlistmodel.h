// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef ACCOUNTLISTMODEL_H
#define ACCOUNTLISTMODEL_H

#include <QAbstractListModel>
namespace dccV25 {
class AccountListModel : public QAbstractListModel
{
public:
    explicit AccountListModel(QObject *parent = nullptr);
    enum AccountListRole {
        UserIdRole = Qt::UserRole + 1,
        UserTypeRole,
        AvatarRole,
        OnlineRole
    };

public:
    void reset();
    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
};

class GroupListModel : public QAbstractListModel
{
    Q_OBJECT
    Q_PROPERTY(bool isCreatingGroup READ isCreatingGroup WRITE setCreatingGroup)
public:
    explicit GroupListModel(const QString &id, QObject *parent = nullptr);
    enum GroupListRole {
        UserIdRole = Qt::UserRole + 1,
        EditAbleRole,
        EnableRole
    };

public:
    inline QString userId() const {
        return m_userId;
    }
    void setUserId(const QString &id);
    void updateGroups(const QStringList &groups);

    virtual int rowCount(const QModelIndex &parent) const override;
    virtual QVariant data(const QModelIndex &index, int role) const override;
    virtual QHash<int, QByteArray> roleNames() const override;
    
    bool isCreatingGroup() const { return m_isCreatingGroup; }
    Q_INVOKABLE void setCreatingGroup(const bool &isCreatingGroup) { m_isCreatingGroup = isCreatingGroup; }

signals:
    void groupsUpdated();
private:
    QString m_userId;
    QStringList m_groups;
    bool m_isCreatingGroup = false;
};

}
#endif // ACCOUNTLISTMODEL_H
