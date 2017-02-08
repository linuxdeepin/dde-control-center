#ifndef VPNLISTDELEGATE_H
#define VPNLISTDELEGATE_H

#include <QAbstractItemDelegate>

class VpnListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit VpnListDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // VPNLISTDELEGATE_H
