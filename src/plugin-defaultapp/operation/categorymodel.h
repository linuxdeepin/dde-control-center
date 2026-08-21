// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef CATEGORYMODEL_H
#define CATEGORYMODEL_H

#include "category.h"

#include <QAbstractItemModel>
#include <QFileInfo>

class CategoryModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit CategoryModel(Category *parent = nullptr);
    ~CategoryModel() override;

    inline Category *category() const { return m_category; }

    // Basic functionality:
    QHash<int, QByteArray> roleNames() const override;
    QModelIndex index(int row, int column, const QModelIndex &parentIndex = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;

    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;

public Q_SLOTS:
    void addApp(const QString &path);
    void removeApp(const QString &id);
    void setDefaultApp(const QString &id);

Q_SIGNALS:
    void requestCreateFile(const QString &category, const QFileInfo &info);
    void requestDelUserApp(const QString &name, const App &item);
    void requestSetDefaultApp(const QString &category, const App &item);

private Q_SLOTS:
    void onAddApp(const App &app);
    void onRemoveApp(const App &app);
    void onDefaultChanged(const App &app);
    void resetApp();

private:
    const App *getAppById(const QString &appId);

    inline bool isValid(const App &app) const { return (!app.Id.isNull() && !app.Id.isEmpty()); }

private:
    QList<App> m_applist;
    Category *m_category;
};

#endif // CATEGORYMODEL_H
