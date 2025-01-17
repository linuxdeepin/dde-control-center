// SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef APPINFOLISTMODEL_H
#define APPINFOLISTMODEL_H

#include <QAbstractListModel>

struct AppItemData
{
    QString name;
    QString icon;
    QString key;
    bool enable;
    bool state;
};

class AppInfoListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum AppItemRoles {
        NameRole = Qt::UserRole + 1,
        IconRole,
        KeyRole,
        EnableRole,
    };
    Q_ENUM(AppItemRoles)

    explicit AppInfoListModel(QObject *parent = nullptr);
    ~AppInfoListModel() override;

    void addAppItem(AppItemData *item);
    void removeAppItem(AppItemData* item);
    void clearItem();
    void updateAppItem(const QString &key, bool enable);

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[IconRole] = "icon";
        roles[KeyRole] = "key";
        roles[EnableRole] = "enable";
        return roles;
    }

private:
    QList<AppItemData*> m_appItemList;
};

#endif // APPINFOLISTMODEL_H
