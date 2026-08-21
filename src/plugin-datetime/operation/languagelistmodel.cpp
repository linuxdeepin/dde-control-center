//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "languagelistmodel.h"
#include "keyboard/metadata.h"

namespace dccV25 {
LanguageListModel::LanguageListModel(QObject *parent)
    : QAbstractListModel(parent)
{

}

LanguageListModel::~LanguageListModel()
{

}

int LanguageListModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_datas.count();
}

QVariant LanguageListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_datas.size())
        return QVariant();

    const auto &data = m_datas.value(index.row());

    switch (role) {
    case Qt::DisplayRole:
        return data.text();
    case SearchTextRole:
        return data.text() + data.pinyin() + data.key();
    case LangKeyIdRole:
        return data.key();
    default:
        break;
    }

    return QVariant();
}

QHash<int, QByteArray> LanguageListModel::roleNames() const
{
    QHash<int, QByteArray> names = QAbstractListModel::roleNames();
    names[SearchTextRole] = "searchText";
    names[LangKeyIdRole] = "key";
    return names;
}

void LanguageListModel::removeLocalLangs()
{
    for (QList<dccV25::MetaData>::iterator iter = m_datas.begin(); iter != m_datas.end();) {
        if (m_localLangs.contains(iter->text())) {
            iter = m_datas.erase(iter);
            continue;
        }
        ++iter;
    }
}

void LanguageListModel::setMetaData(const QList<dccV25::MetaData> &data)
{
    if (m_datas != data) {
        beginResetModel();
        m_originalDatas = data;
        m_datas = data;
        removeLocalLangs();
        endResetModel();
    }
}

void LanguageListModel::setLocalLang(const QStringList &langs)
{
    if (m_localLangs != langs) {
        beginResetModel();
        m_localLangs = langs;
        m_datas = m_originalDatas;
        removeLocalLangs();
        endResetModel();
    }
}
}
