/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     justforlxz <justforlxz@outlook.com>
 *
 * Maintainer: justforlxz <justforlxz@outlook.com>
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

#ifndef STANDARDITEMHELPER_H
#define STANDARDITEMHELPER_H

#include <QObject>
#include <QModelIndex>

class QListView;
class QStandardItemModel;

class StandardItemHelper : public QObject
{
    Q_OBJECT
public:
    explicit StandardItemHelper(QListView const * const listView, QStandardItemModel const * const model, QObject *parent = nullptr);

Q_SIGNALS:

public Q_SLOTS:

private:
    void onListViewClicked(const QModelIndex& index);

private:
    QListView const * const m_listView;
    QStandardItemModel const * const m_model;
    QModelIndex m_previousIndex;
};

#endif // STANDARDITEMHELPER_H
