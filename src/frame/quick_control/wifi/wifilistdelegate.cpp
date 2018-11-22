/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "wifilistdelegate.h"
#include "wifilistmodel.h"
#include "widgets/basiclistdelegate.h"

#include <QPainter>
#include <QDebug>
#include <QJsonObject>
#include <QDateTime>

WifiListDelegate::WifiListDelegate(QObject *parent)
    : QAbstractItemDelegate(parent),

      m_securityPixmap(loadPixmap(":/frame/quick_control/wifi/wireless/security.svg"))
{

}

void WifiListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    painter->setRenderHints(QPainter::TextAntialiasing | QPainter::SmoothPixmapTransform);
    painter->setPen(Qt::white);
    painter->setBrush(Qt::red);

    const bool isHeader = index.data(WifiListModel::ItemIsHeaderRole).toBool();
    const bool isHovered = index.data(WifiListModel::ItemHoveredRole).toBool();
    const bool isTips = index.data(WifiListModel::ItemIsHiddenTipsRole).toBool();
    const bool isNext = index.data(WifiListModel::ItemNextRole).toBool();
    const bool isPowerOff = index.data(WifiListModel::ItemIsPowerOffRole).toBool();

    if (isHovered && !isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.1 * 255));

    if (isHeader)
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.15 * 255));

    if (index.row())
    {
        if (!isNext && !isHovered) {
            painter->setPen(QColor(255, 255, 255, 255 * 0.05));
            painter->drawLine(QPoint(10, option.rect.top()), QPoint(option.rect.right() - 10, option.rect.top()));
        }
    }
    else
    {
        painter->setPen(QColor(255, 255, 255, 255 * 0.1));
        painter->drawLine(QPoint(0, option.rect.top()), QPoint(option.rect.right(), option.rect.top()));
    }

    QFont f(painter->font());
    f.setWeight(isHeader ? 500 : 300);
    painter->setFont(f);

    painter->setPen(Qt::white);

    if (isPowerOff)
        painter->drawText(option.rect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
    else if (isHeader)
        painter->drawText(option.rect.marginsRemoved(QMargins(24, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());
    else
        painter->drawText(option.rect.marginsRemoved(QMargins(70, 0, 0, 0)), Qt::AlignVCenter | Qt::AlignLeft, index.data(Qt::DisplayRole).toString());

    if (!isHeader && !isTips)
    {
        const QJsonObject info = index.data(WifiListModel::ItemInfoRole).value<QJsonObject>();
        const bool isSecured = info.value("Secured").toBool();
        const int strength = info.value("Strength").toInt();

        // draw signal icon
        const int iconIndex = (strength / 10) & ~0x1;
        const int x = 40;
        const int y = option.rect.top() + (option.rect.height() - 16) / 2;
        painter->drawPixmap(x, y, loadPixmap(QString(":/frame/quick_control/wifi/wireless/wireless-%1-symbolic.svg").arg(iconIndex)));

        // draw secured icon
        if (isSecured)
        {
            const int x = 20;

            painter->drawPixmap(x, y, m_securityPixmap);
        }

        const int icon_x = option.rect.right() - 16 - 10;
        const bool isActivating = index.data(WifiListModel::ItemIsActivatingRole).toBool();
        if (isActivating)
        {
            const quint64 index = QDateTime::currentMSecsSinceEpoch() / 20;
            const QString pix = QString(":/frame/themes/light/icons/white_loading/loading_0%1.png").arg((index % 90), 2, 10, QChar('0'));
            painter->drawPixmap(icon_x, y + 1, loadPixmap(pix));
        } else {
            // draw active icon
            const bool isActive = index.data(WifiListModel::ItemIsActiveRole).toBool();
            if (isActive)
            {
                if (isHovered)
                    painter->drawPixmap(icon_x, y, loadPixmap(":/frame/themes/dark/icons/disconnect.svg"));
                else
                    painter->drawPixmap(icon_x, y, loadPixmap(":/frame/themes/dark/icons/select.svg"));
            }
        }
        painter->fillRect(option.rect, QColor(255, 255, 255, 0.05 * 255));
    }
}

QSize WifiListDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option);

    return index.data(Qt::SizeHintRole).toSize();
}
