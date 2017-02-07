#include "displaycontrolmodel.h"

#include <QSize>

DisplayControlModel::DisplayControlModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

int DisplayControlModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return 3;
}

QVariant DisplayControlModel::data(const QModelIndex &index, int role) const
{
    switch (role)
    {
    case Qt::DisplayRole:
        return "config " + QString::number(index.row());
    case Qt::SizeHintRole:
        return QSize(0, 50);
    default:;
    }

    return QVariant();
}
