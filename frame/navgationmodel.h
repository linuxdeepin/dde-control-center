#ifndef NAVGATIONMODEL_H
#define NAVGATIONMODEL_H

#include <QAbstractListModel>
#include <QFlags>

namespace dcc {

class NavgationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit NavgationModel(QObject *parent = 0);

    enum NavgationItemRole
    {
        ItemReservedRole = Qt::UserRole,
        ItemHoveredRole,
        ItemEdgeRole,
    };

    enum EdgeFlag
    {
        Top     =   1 << 0,
        Left    =   1 << 1,
        Bottom  =   1 << 2,
        Right   =   1 << 3,
    };
    Q_DECLARE_FLAGS(EdgeFlags, EdgeFlag)

public slots:
    void insertItem(const QString &item);
    void removeItem(const QString &item);
    void appendAvailableItem(const QString &item);

    void setCurrentItem(const QModelIndex &index);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    NavgationModel::EdgeFlags indexEdgeFlag(const QModelIndex &index) const;

private:
    QStringList m_items;
    QStringList m_allItems;

    QModelIndex m_currentIndex;
};

}

Q_DECLARE_OPERATORS_FOR_FLAGS(dcc::NavgationModel::EdgeFlags)
Q_DECLARE_METATYPE(dcc::NavgationModel::EdgeFlags)

#endif // NAVGATIONMODEL_H
