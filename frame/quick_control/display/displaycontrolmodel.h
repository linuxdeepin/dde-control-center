#ifndef DISPLAYCONTROLMODEL_H
#define DISPLAYCONTROLMODEL_H

#include "display/displaymodel.h"

#include <QAbstractListModel>

class DisplayControlModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit DisplayControlModel(dcc::display::DisplayModel *model, QObject *parent = 0);

    enum DisplayRole
    {
        TypeNameRole = Qt::DisplayRole,
        UnusedRole = Qt::UserRole,
        TypeDescriptionRole,
    };

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

private:
    const QString optionName(const int index) const;

private:
    dcc::display::DisplayModel *m_displayModel;
};

#endif // DISPLAYCONTROLMODEL_H
