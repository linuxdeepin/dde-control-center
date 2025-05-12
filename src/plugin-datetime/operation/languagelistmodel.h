//SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LANGUAGELISTMODEL_H
#define LANGUAGELISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
namespace dccV25 {
class MetaData;
class LanguageListModel : public QAbstractListModel
{
public:
    explicit LanguageListModel(QObject *parent = nullptr);
    virtual ~LanguageListModel();

    enum LangItemRole {
        SearchTextRole = Qt::UserRole + 1,
        LangKeyIdRole
    };

public:
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;

    void setMetaData(const QList<dccV25::MetaData> &data);
    void setLocalLang(const QStringList &langs);

protected:
    void removeLocalLangs();

private:
    QList<dccV25::MetaData> m_datas;
    QList<dccV25::MetaData> m_originalDatas;
    QStringList m_localLangs;
};
}

#endif // LANGUAGELISTMODEL_H
