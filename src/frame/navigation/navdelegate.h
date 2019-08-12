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

#ifndef NAVDELEGATE_H
#define NAVDELEGATE_H

#include <QStyledItemDelegate>
#include <QListView>

class NavDelegate : public QStyledItemDelegate
{
public:
    NavDelegate(QListView::ViewMode mode = QListView::IconMode, QObject *parent = nullptr);

    inline QListView::ViewMode viewMode() const { return m_viewMode; }
    void setViewMode(QListView::ViewMode mode);

private:
    void initStyleOption(QStyleOptionViewItem *option, const QModelIndex &index) const override;

    QListView::ViewMode m_viewMode;
    QSize m_itemSize;
};

#endif // NAVDELEGATE_H
