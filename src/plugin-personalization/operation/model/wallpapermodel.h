// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WALLPAPERMODEL_H
#define WALLPAPERMODEL_H

#include <qabstractitemmodel.h>
#include <qt5/QtCore/qnamespace.h>
#include <QObject>
#include <QPixmap>

struct ItemNode {
    QString item;
    bool selectable;
    bool deletable;
};

typedef QSharedPointer<ItemNode> ItemNodePtr;

class WallpaperModel : public QAbstractItemModel
{
    Q_OBJECT
public:

    enum ItemTypeRole {
        Item_Url_Role = Qt::UserRole + 1,
    };
    explicit WallpaperModel(QObject *parent = nullptr);
    QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void insertItem(int pos, ItemNodePtr it);
    void removeItem(const QString &item);
    ItemNodePtr itemNode(const QModelIndex &idx) const;
    QModelIndex itemIndex(const QString &item) const;
    void resetData(const QList<ItemNodePtr> &list);
protected:
    QHash<int, QByteArray> roleNames() const override;
private:
    QList<ItemNodePtr> items;
};

#endif // WALLPAPERMODEL_H