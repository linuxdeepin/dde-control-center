// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef UPDATELISTMODEL_H
#define UPDATELISTMODEL_H

#include <QAbstractListModel>

struct UpdateItemData
{
    UpdateItemData() {}

    QString title;
    QString titleDescription;
    QString updateLog;
    QString releaseTime;

    bool checked;

    int updateStatus;
};


class UpdateListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum updateRoles {
        Title = Qt::UserRole + 1,
        TitleDescription,
        UpdateLog,
        ReleaseTime,
        Checked,
        UpdateStatus
    };

    explicit UpdateListModel(QObject *parent = nullptr);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    void addUpdateData(const UpdateItemData& itemData);

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[Title] = "title";
        roles[TitleDescription] = "titleDescription";
        roles[UpdateLog] = "updateLog";
        roles[ReleaseTime] = "releaseTime";
        roles[Checked] = "checked";
        roles[UpdateStatus] = "updateStatus";
        return roles;
    }

private:

    QList<UpdateItemData> m_updateLists;
};

#endif // UPDATELISTMODEL_H
