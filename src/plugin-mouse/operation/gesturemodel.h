//SPDX-FileCopyrightText: 2025 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef GESTUREMODEL_H
#define GESTUREMODEL_H

#include <QAbstractListModel>

#include "gesturedata.h"

class GestureModel : public QAbstractListModel
{
    Q_OBJECT

public:
    enum gestureRoles {
        IconRole = Qt::UserRole + 1,
        DescriptionRole,
        ActionsIndexRole,
        ActionListRole,
    };

    explicit GestureModel(QObject *parent = nullptr);
    ~GestureModel() override;

    bool containsGestures(QString direction, int fingersNum);
    void updateGestureData(const GestureData &data);

    // Basic functionality:
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

    QString getGesturesDec(GestureData *data) const;
    QString getGesturesIconPath(GestureData *data) const;
    QVariantList getGestureActionNames(GestureData *data) const;
    int getGestureActionIndex(GestureData *data) const;

    // Add data:
    bool insertRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex &parent = QModelIndex()) override;

    void addGestureData(GestureData *data);
    void removeGestureData(GestureData *data);
    void updateGestureData(GestureData *data);

    GestureData *getGestureData(int index) const;

    QHash<int, QByteArray> roleNames() const override
    {
        QHash<int, QByteArray> roles;
        roles[IconRole] = "iconRole";
        roles[DescriptionRole] = "descriptionRole";
        roles[ActionsIndexRole] = "actionsIndexRole";
        roles[ActionListRole] = "actionListRole";
        return roles;
    }

private:
    QList< GestureData *> m_gestures;
};
#endif // GESTUREMODEL_H
