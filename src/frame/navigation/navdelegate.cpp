/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "navdelegate.h"
#include "navmodel.h"

#include <QPainter>
#include <QImageReader>
#include <QDebug>

NavDelegate::NavDelegate(QListView::ViewMode mode, QObject *parent)
    : QStyledItemDelegate(parent)
    , m_viewMode(mode)
{
}

void NavDelegate::setViewMode(QListView::ViewMode mode)
{
    m_viewMode = mode;
}

void NavDelegate::setItemSize(const QSize &size)
{
    m_itemSize = size;
}

QSize NavDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    if (m_itemSize.isValid())
        return m_itemSize;

    return QStyledItemDelegate::sizeHint(option, index);
}

void NavDelegate::initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const
{
    QStyledItemDelegate::initStyleOption(option, index);

    if (m_viewMode == QListView::IconMode) {
        option->decorationPosition = QStyleOptionViewItem::Top;
        option->decorationAlignment = Qt::AlignCenter;
        option->displayAlignment = Qt::AlignHCenter;

        if (m_itemSize.isValid()) {
            option->decorationSize = QSize(m_itemSize.height() / 2, m_itemSize.height() / 2);
        }
    }
}
