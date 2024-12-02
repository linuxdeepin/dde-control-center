// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef AUDIOSERVERMODEL_H
#define AUDIOSERVERMODEL_H

#include <QObject>
#include <QAbstractListModel>

struct AudioServerData {
    QString name;
    QString serverName;
    bool checked;
};

class AudioServerModel : public QAbstractListModel
{
    Q_OBJECT
public:

    enum soundEffectsRoles{
        NameRole = Qt::UserRole + 1,
        ServerName,
        IsChecked
    };

    explicit AudioServerModel(QObject *parent = nullptr);

    void addData(const AudioServerData &data);
    void updateAllData();
    void updateCheckedService(const QString &name);
    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    QHash<int, QByteArray> roleNames() const override {
        QHash<int, QByteArray> roles;
        roles[NameRole] = "name";
        roles[ServerName] = "serverName";
        roles[IsChecked] = "isChecked";
        return roles;
    }

private:

    QList<AudioServerData> m_audioServerDatas;
};

#endif // AUDIOSERVERMODEL_H
