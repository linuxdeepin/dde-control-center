//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#include "indexmodel.h"
#include <DStandardItem>

DWIDGET_USE_NAMESPACE
using namespace DCC_NAMESPACE;

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
