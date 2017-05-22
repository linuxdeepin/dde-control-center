#ifndef BASICLISTMODEL_H
#define BASICLISTMODEL_H

#include <QAbstractListModel>

namespace dcc {

namespace widgets {

class BasicListModel : public QAbstractListModel
{
    Q_OBJECT

public:

    enum ItemRole
    {
        ItemSizeRole = Qt::SizeHintRole,
        ItemTextRole = Qt::DisplayRole,
        ReservedRole = Qt::UserRole,
        ItemIsFirstRole,
        ItemIsLastRole,
        ItemSelectedRole,
        ItemHoverRole
    };

    explicit BasicListModel(QObject *parent = 0);

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

public slots:
    void clear();
    void appendOption(const QString &text, const QVariant &data = QVariant());
    void setSelectedIndex(const QModelIndex &index);
    void setHoveredIndex(const QModelIndex &index);

private:
    QList<QString> m_options;
    QList<QVariant> m_values;

    QModelIndex m_selectedIndex;
    QModelIndex m_hoveredIndex;
};

}

}

#endif // BASICLISTMODEL_H
