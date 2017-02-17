#include "bluetoothdelegate.h"
#include "bluetoothlistmodel.h"
#include "bluetooth/device.h"

#include <QPainter>
#include <QDebug>

BluetoothDelegate::BluetoothDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{

}

void BluetoothDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    const bool isHeader = index.data(BluetoothListModel::ItemIsHeaderRole).toBool();
    const bool isHovered = index.data(BluetoothListModel::ItemHoveredRole).toBool();

    if (isHovered && !isHeader)
        painter->fillRect(option.rect, QColor(0, 0, 0, .6 * 255));

    if (isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, .3 * 255));

    if (isHeader)
        painter->setPen(Qt::red);
    else
        painter->setPen(Qt::white);

    if (isHeader)
        painter->drawText(option.rect.marginsRemoved(QMargins(10, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
    else
        painter->drawText(option.rect.marginsRemoved(QMargins(70, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

    if (!isHeader)
    {
        const QJsonObject info = index.data(BluetoothListModel::ItemInfoRole).value<QJsonObject>();
        // connecting , connected, no connect
        if (info["State"] == Device::StateConnected) {
            // draw connected icon
            const int x = option.rect.right() - 24;
            const int y = option.rect.top() + (option.rect.height() - 16) / 2;
            painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/select.png"));

        }
    }
}

QSize BluetoothDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
