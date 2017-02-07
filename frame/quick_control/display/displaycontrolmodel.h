#ifndef DISPLAYCONTROLMODEL_H
#define DISPLAYCONTROLMODEL_H

#include <QAbstractListModel>

class DisplayControlModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DisplayControlModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
};

#endif // DISPLAYCONTROLMODEL_H
