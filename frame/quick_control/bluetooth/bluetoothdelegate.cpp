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
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.1 * 255));

    if (isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.1 * 255));

    QFont f(painter->font());
    f.setBold(isHeader);
    painter->setFont(f);

    if (isHeader)
        painter->drawText(option.rect.marginsRemoved(QMargins(10, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
    else
        painter->drawText(option.rect.marginsRemoved(QMargins(70, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

    if (!isHeader)
    {
        const bool state = index.data(BluetoothListModel::ItemConnectedRole).toBool();

        // connecting , connected, no connect
        if (state) {
            // draw connected icon
            const int x = option.rect.right() - 24;
            const int y = option.rect.top() + (option.rect.height() - 16) / 2;

            if (isHovered)
                painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/remove.png"));
            else
                painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/select.png"));

        }

        painter->fillRect(option.rect, QColor(255, 255, 255, 0.05 * 255));
    }
}

QSize BluetoothDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
