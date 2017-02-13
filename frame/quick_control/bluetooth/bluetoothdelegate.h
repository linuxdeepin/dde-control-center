#ifndef BLUETOOTHDELEGATE_H
#define BLUETOOTHDELEGATE_H

#include <QAbstractItemDelegate>

class BluetoothDelegate : public QAbstractItemDelegate
{
    Q_OBJECT
public:
    explicit BluetoothDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

#endif // BLUETOOTHDELEGATE_H
