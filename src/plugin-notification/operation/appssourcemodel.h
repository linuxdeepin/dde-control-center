// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef APPSSOURCEMODEL_H
#define APPSSOURCEMODEL_H

#include <QAbstractItemModel>
namespace DCC_NAMESPACE{

class AppItemModel;

enum AppsSourceModelRole
{
    AppIdRole = Qt::UserRole + 1,
    AppNameRole,
    AppIconRole,
    EnableNotificationRole,
    EnablePreviewRole,
    EnableSoundRole,
    ShowNotificationDesktopRole,
    ShowNotificationCenterRole,
    LockScreenShowNotificationRole,
    TransliteratedRole
};

class AppsSourceModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit AppsSourceModel(QObject *parent = nullptr);

    // Basic functionality:
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex index(int row,
                      int column,
                      const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

    void appAdded(AppItemModel* item);
    void appRemoved(const QString &appName);

private:
    QList<AppItemModel *> m_appItemModels;
};
}
#endif // APPSSOURCEMODEL_H
