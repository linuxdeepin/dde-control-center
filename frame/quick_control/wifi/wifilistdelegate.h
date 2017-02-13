#ifndef WIFILISTDELEGATE_H
#define WIFILISTDELEGATE_H

#include <QAbstractItemDelegate>

class WifiListDelegate : public QAbstractItemDelegate
{
    Q_OBJECT

public:
    explicit WifiListDelegate(QObject *parent = 0);

    void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
    QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const;

private:
    QPixmap m_securityPixmap;
};

#endif // WIFILISTDELEGATE_H
