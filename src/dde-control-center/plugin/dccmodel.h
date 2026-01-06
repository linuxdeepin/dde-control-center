// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCMODEL_H
#define DCCMODEL_H

#include <QAbstractItemModel>
#include <QQmlEngine>

namespace dccV25 {
class DccObject;

class DccModel : public QAbstractItemModel
{
    Q_OBJECT
    Q_PROPERTY(DccObject * root READ root WRITE setRoot NOTIFY rootChanged)
    QML_ELEMENT
public:
    explicit DccModel(QObject *parent = nullptr);
    ~DccModel() override;

    DccObject *root() const;
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex index(const DccObject *object);
    // Basic functionality:
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
public Q_SLOTS:
    void setRoot(DccObject *root);
    DccObject *getObject(int row);

private Q_SLOTS:
    void updateObject();
    void AboutToAddObject(const DccObject *parent, int pos);
    void addObject(const DccObject *child);
    void AboutToRemoveObject(const DccObject *parent, int pos);
    void removeObject(const DccObject *child);
    void AboutToMoveObject(const DccObject *parent, int pos, int oldPos);
    void moveObject(const DccObject *child);

Q_SIGNALS:
    void rootChanged(DccObject *root);

private:
    void connectObject(const DccObject *obj);
    void disconnectObject(const DccObject *obj);

private:
    DccObject *m_root;
};
} // namespace dccV25
#endif // DCCMODEL_H
