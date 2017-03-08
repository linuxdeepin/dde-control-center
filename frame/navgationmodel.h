#ifndef NAVGATIONMODEL_H
#define NAVGATIONMODEL_H

#include <QAbstractListModel>

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
    };

public slots:
    void insertItem(const QString &item);
    void removeItem(const QString &item);
    void appendAvailableItem(const QString &item);

    void setCurrentItem(const QModelIndex &index);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QStringList m_items;
    QStringList m_allItems;

    QModelIndex m_currentIndex;
};

}

#endif // NAVGATIONMODEL_H
