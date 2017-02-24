#ifndef NAVGATIONMODEL_H
#define NAVGATIONMODEL_H

#include <QAbstractListModel>

namespace dcc {

class NavgationModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit NavgationModel(QObject *parent = 0);

public slots:
    void insertItem(const QString &item);
    void removeItem(const QString &item);
    void appendAvailableItem(const QString &item);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    QStringList m_items;
    QStringList m_allItems;
};

}

#endif // NAVGATIONMODEL_H
