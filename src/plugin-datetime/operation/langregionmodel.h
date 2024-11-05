//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LANGREGIONMODEL_H
#define LANGREGIONMODEL_H

#include <QAbstractListModel>
#include <QObject>

namespace dccV25 {
class LangRegionModel : public QAbstractListModel
{
public:
    explicit LangRegionModel(QObject *parent = nullptr);

    enum LangItemRole {
        SearchTextRole = Qt::UserRole + 1,
        RegionKeyIdRole,
        LocaleKeyIdRole,
        FirstDayOfWeek,
        ShortDate,
        LongDate,
        ShortTime,
        LongTime,
        Currency,
        Digit,
        PaperSize
    };

    // QAbstractItemModel interface
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

struct FormatsInfo
{
    QString name;
    QStringList values;
    int index = -1;
    int indexBegin = -1;
    bool operator ==(const FormatsInfo &info) const {
        return info.name == name && info.values == values &&
                info.index == index && info.indexBegin == indexBegin;
    }
};

class FormatsModel : public QAbstractListModel {
public:
    enum FormatsRole {
        NameRole = Qt::UserRole + 1,
        ValuesRole,
        CurrentRole,
        IndexBegin,
    };

    explicit FormatsModel(QObject *parent = nullptr);
    void setDatas(const QList<FormatsInfo> &datas);
public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

private:
    QList<FormatsInfo> m_datas;
};

}
#endif // LANGREGIONMODEL_H
