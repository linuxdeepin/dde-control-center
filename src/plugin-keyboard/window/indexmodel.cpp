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

#include "indexmodel.h"
#include <DStandardItem>

DWIDGET_USE_NAMESPACE
DCC_USE_NAMESPACE

IndexModel::IndexModel(QObject *parent)
    : QStandardItemModel(parent)
{
}

void IndexModel::setMetaData(const QList<MetaData> &datas)
{
    beginResetModel();
    m_datas = datas;
    for (int i = 0; i < m_datas.size(); ++i) {
        DStandardItem *item = new DStandardItem(m_datas[i].text());
        item->setData(QVariant::fromValue(m_datas[i]), KBLayoutRole);
        appendRow(item);
    }
    endResetModel();
}

QList<MetaData> IndexModel::metaData() const
{
    return m_datas;
}

int IndexModel::indexOf(const MetaData &md)
{
    int index = 0;
    QList<MetaData>::iterator it = m_datas.begin();
    for (; it != m_datas.end(); ++it) {
        if ((*it) == md && (*it).section()) {
            return index;
        }
        index++;
    }

    return -1;
}

void IndexModel::setLetters(QList<QString> &letters)
{
    m_letters = letters;
}

QList<QString> IndexModel::letters() const
{
    return m_letters;
}

int IndexModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);

    return m_datas.count();
}

int IndexModel::getModelCount()
{
    return m_datas.count();
}
