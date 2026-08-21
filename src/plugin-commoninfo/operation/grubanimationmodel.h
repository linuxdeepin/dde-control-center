// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef GRUBANIMATIONMODEL_H
#define GRUBANIMATIONMODEL_H

#include <QAbstractListModel>

struct  GrubAnimationData {
    QString imagePath;
    QString text;
    bool checkStatus;
    bool startAnimation;
    double scale;
    int plymouthScale;
};

class GrubAnimationModel : public QAbstractListModel
{
    Q_OBJECT

    enum grubAnimationDataRoles{
        ImagePath = Qt::UserRole + 1,
        Text,
        CheckStatus,
        StartAnimation,
        Scale,
        PlymouthScale,
    };

public:
    explicit GrubAnimationModel(QObject *parent = nullptr);

    void initData(const QList<GrubAnimationData> &data);

    void addAnimation(const GrubAnimationData& data);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // 角色名映射
    QHash<int, QByteArray> roleNames() const override;

    void addModel(const GrubAnimationData &data);

    void updateCheckIndex(int plymouthScale, bool startAnimation);

private:
    QList<GrubAnimationData> m_grubAnimationDataList;
};

#endif // GRUBANIMATIONMODEL_H
