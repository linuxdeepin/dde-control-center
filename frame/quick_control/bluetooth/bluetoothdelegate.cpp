#include "bluetoothdelegate.h"
#include "bluetoothlistmodel.h"
#include "bluetooth/device.h"

#include <QPainter>
#include <QDateTime>
#include <QDebug>

BluetoothDelegate::BluetoothDelegate(QObject *parent) : QAbstractItemDelegate(parent)
{

}

void BluetoothDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    const bool isSetting = index.data(BluetoothListModel::ItemIsSettingRole).toBool();
    const bool isHeader = index.data(BluetoothListModel::ItemIsHeaderRole).toBool();
    const bool isHovered = index.data(BluetoothListModel::ItemHoveredRole).toBool();

    if (isHovered && !isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.1 * 255));

    if (isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.15 * 255));

    QFont f(painter->font());
    f.setBold(isHeader);
    painter->setFont(f);

    if (isSetting) {
        painter->drawText(option.rect.marginsRemoved(QMargins(10, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
    } else {
        if (isHeader)
            painter->drawText(option.rect.marginsRemoved(QMargins(10, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
        else
            painter->drawText(option.rect.marginsRemoved(QMargins(70, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());


        if (!isHeader)
        {
            const bool connecting = index.data(BluetoothListModel::ItemConnectingRole).toBool();

            const int x = option.rect.right() - 24;
            const int y = option.rect.top() + (option.rect.height() - 16) / 2;

            // connecting , connected, no connect
            if (connecting) {
                const quint64 index = QDateTime::currentMSecsSinceEpoch() / 20;
                const QString pix = QString(":/frame/themes/dark/icons/spinner14/Spinner%1.png").arg((index % 91) + 1, 2, 10, QChar('0'));
                painter->drawPixmap(x, y, QPixmap(pix));
            } else {
                const bool state = index.data(BluetoothListModel::ItemConnectedRole).toBool();
                if (state) {
                    // draw connected icon
                    if (isHovered)
                        painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/remove.png"));
                    else
                        painter->drawPixmap(x, y, QPixmap(":/frame/themes/dark/icons/select.png"));
                }
            }
            painter->fillRect(option.rect, QColor(255, 255, 255, 0.05 * 255));

            if (!isHovered) {
                QPen pen(QColor(255, 255, 255, 0.1 * 255));
                QLineF line;
                line.setP1(QPoint(option.rect.bottomLeft().x() + 10, option.rect.bottomLeft().y()));
                line.setP2(QPoint(option.rect.bottomRight().x() - 10, option.rect.bottomRight().y()));
                painter->setPen(pen);
                painter->drawLine(line);
            }
        }
    }
}

QSize BluetoothDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
