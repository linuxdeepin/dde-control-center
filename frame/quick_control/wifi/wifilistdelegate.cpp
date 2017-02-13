#include "wifilistdelegate.h"
#include "wifilistmodel.h"

#include <QPainter>
#include <QDebug>
#include <QJsonObject>

WifiListDelegate::WifiListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent),

      m_securityPixmap(QPixmap(":/frame/quick_control/wifi/wireless/security.svg"))
{

}

void WifiListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    const bool isHeader = index.data(WifiListModel::ItemIsHeaderRole).toBool();
    const bool isHovered = index.data(WifiListModel::ItemHoveredRole).toBool();

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
        const QJsonObject info = index.data(WifiListModel::ItemInfoRole).value<QJsonObject>();
        const bool isSecured = info.value("Secured").toBool();
        const int strength = info.value("Strength").toInt();

        // draw signal icon
        const int iconIndex = (strength / 10) & ~0x1;
        const int x = 40;
        const int y = option.rect.top() + (option.rect.height() - 16) / 2;
        painter->drawPixmap(x, y, QPixmap(QString(":/frame/quick_control/wifi/wireless/wireless-%1-symbolic.svg").arg(iconIndex)));

        // draw secured icon
        if (isSecured)
        {
            const int x = 20;

            painter->drawPixmap(x, y, m_securityPixmap);
        }
    }
}

QSize WifiListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
