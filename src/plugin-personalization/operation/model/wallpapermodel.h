// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WALLPAPERMODEL_H
#define WALLPAPERMODEL_H

#include <QSortFilterProxyModel>
#include <QAbstractItemModel>
#include <QPixmap>

struct WallpaperItem {
    QString url;
    QString picPath;
    QString thumbnail;
    bool deleteAble;
    qint64 lastModifiedTime;
    bool configurable;
};

enum ItemTypeRole {
    Item_Url_Role = Qt::UserRole + 1,
    Item_PicPath_Role,
    Item_Thumbnail_Role,
    Item_deleteAble_Role,
    Item_lastModifiedTime_Role,
    Item_configurable_Role,
};

typedef QSharedPointer<WallpaperItem> WallpaperItemPtr;

class WallpaperSortModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit WallpaperSortModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {
            sort(0);
        }

    Q_INVOKABLE bool hasWallpaper(const QString &url) const {
        for (size_t i = 0; i < sourceModel()->rowCount(); i++) {
            if (url == sourceModel()->data(sourceModel()->index(i, 0), Item_Url_Role).toString()) {
                return true;
            }
        }
        return false;
    }

    Q_INVOKABLE QString getPicPathByUrl(const QString &url) const;
    Q_INVOKABLE bool getConfigAbleByUrl(const QString &url) const;

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override
    {

    return sourceModel()->data(source_left, Item_lastModifiedTime_Role).value<qint64>() >
        sourceModel()->data(source_right, Item_lastModifiedTime_Role).value<qint64>();
}
};

class WallpaperModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit WallpaperModel(QObject *parent = nullptr);
    QModelIndex index(int row, int column,
                              const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void insertItem(int pos, WallpaperItemPtr it);
    void removeItem(const QString &item);
    WallpaperItemPtr itemNode(const QModelIndex &idx) const;
    QModelIndex itemIndex(const QString &item) const;
    void resetData(const QList<WallpaperItemPtr> &list);    
protected:
    QHash<int, QByteArray> roleNames() const override;
private:
    QList<WallpaperItemPtr> items;
};

#endif // WALLPAPERMODEL_H
