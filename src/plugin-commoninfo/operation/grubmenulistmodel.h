// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GRUBMENULISTMODEL_H
#define GRUBMENULISTMODEL_H

#include <QAbstractListModel>

struct GrubMenuData
{
    QString text;
    bool checkStatus;
};

class GrubMenuListModel : public QAbstractListModel
{
    Q_OBJECT

    enum grubMenuListModelRoles{
        Text = Qt::UserRole + 1,
        CheckStatus
    };

public:
    explicit GrubMenuListModel(QObject *parent = nullptr);

    void initData(const QList<GrubMenuData> &data);

    void addGrubMenu(const GrubMenuData &data);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // 角色名映射
    QHash<int, QByteArray> roleNames() const override;

    void updateCheckIndex(const QString &text);
private:
    QList<GrubMenuData> m_grubMenuDataList;
};

#endif // GRUBMENULISTMODEL_H
