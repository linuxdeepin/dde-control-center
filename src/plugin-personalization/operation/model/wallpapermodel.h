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
    bool selected;
};

enum ItemTypeRole {
    Item_Url_Role = Qt::UserRole + 1,
    Item_PicPath_Role,
    Item_Thumbnail_Role,
    Item_deleteAble_Role,
    Item_lastModifiedTime_Role,
    Item_configurable_Role,
    Item_Selected_Role,
};

typedef QSharedPointer<WallpaperItem> WallpaperItemPtr;

class WallpaperSortModel : public QSortFilterProxyModel
{
    Q_OBJECT
    Q_PROPERTY(int count READ rowCount NOTIFY countChanged)
public:
    explicit WallpaperSortModel(QObject *parent = nullptr)
        : QSortFilterProxyModel(parent) {
            sort(0);
        }

    void setSourceModel(QAbstractItemModel *sourceModel) override {
        if (this->sourceModel()) {
            // 断开旧源模型的信号
            disconnect(this->sourceModel(), &QAbstractItemModel::rowsInserted,
                       this, &WallpaperSortModel::countChanged);
            disconnect(this->sourceModel(), &QAbstractItemModel::rowsRemoved,
                       this, &WallpaperSortModel::countChanged);
            disconnect(this->sourceModel(), &QAbstractItemModel::modelReset,
                       this, &WallpaperSortModel::countChanged);
        }

        QSortFilterProxyModel::setSourceModel(sourceModel);

        if (sourceModel) {
            // 连接新源模型的信号
            connect(sourceModel, &QAbstractItemModel::rowsInserted,
                    this, &WallpaperSortModel::countChanged);
            connect(sourceModel, &QAbstractItemModel::rowsRemoved,
                    this, &WallpaperSortModel::countChanged);
            connect(sourceModel, &QAbstractItemModel::modelReset,
                    this, &WallpaperSortModel::countChanged);
        }
    }

    Q_INVOKABLE bool hasWallpaper(const QString &url) const {
        for (int i = 0; i < sourceModel()->rowCount(); i++) {
            if (url == sourceModel()->data(sourceModel()->index(i, 0), Item_Url_Role).toString()) {
                return true;
            }
        }
        return false;
    }

    Q_INVOKABLE QString getPicPathByUrl(const QString &url) const;
    Q_INVOKABLE bool getConfigAbleByUrl(const QString &url) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        return sourceModel() ? sourceModel()->rowCount(parent) : 0;
    }

signals:
    void countChanged();

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
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    void insertItem(int pos, WallpaperItemPtr it);
    void appendItem(WallpaperItemPtr it);
    void removeItem(const QString &item);
    void removeItem(WallpaperItemPtr item);
    WallpaperItemPtr itemNode(const QModelIndex &idx) const;
    QModelIndex itemIndex(const QString &item) const;
    QModelIndex itemIndex(WallpaperItemPtr item) const;
    void resetData(const QList<WallpaperItemPtr> &list);
    void updateSelected(const QStringList &selectedLists);
    void setThumbnail(WallpaperItemPtr item, const QString &thumbnail);
protected:
    QHash<int, QByteArray> roleNames() const override;
private:
    QList<WallpaperItemPtr> items;
};

#endif // WALLPAPERMODEL_H
