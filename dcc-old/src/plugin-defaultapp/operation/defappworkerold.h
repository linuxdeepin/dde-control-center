// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QMap>

#include "mimedbusproxyold.h"
#include "category.h"

class QFileInfo;

class DefAppModel;
class Category;
class DefAppWorkerOld : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorkerOld(DefAppModel *m_defAppModel, QObject *parent = 0);

    enum DefaultAppsCategory {
        Browser,
        Mail,
        Text,
        Music,
        Video,
        Picture,
        Terminal
    };

    void active();
    void deactive();


public Q_SLOTS:
    void onSetDefaultApp(const QString &category, const App &item);
    void onGetListApps();
    void onDelUserApp(const QString &mine, const App &item);
    void onCreateFile(const QString &mime, const QFileInfo &info);

private Q_SLOTS:
    void getListAppFinished(const QString &mime, const QString &defaultApp, bool isUser);
    void getDefaultAppFinished(const QString &mime, const QString &w);
    void saveListApp(const QString &mime, const QJsonArray &json, const bool isUser);
    void saveDefaultApp(const QString &mime, const QJsonObject &json);

private:
    DefAppModel *m_defAppModel;
    MimeDBusProxyOld  *m_dbusManager;
    QMap<QString, DefaultAppsCategory> m_stringToCategory;
    QString m_userLocalPath;

private:
    const QString getTypeByCategory(const DefAppWorkerOld::DefaultAppsCategory &category);
    const QStringList getTypeListByCategory(const DefAppWorkerOld::DefaultAppsCategory &category);
    Category* getCategory(const QString &mime) const;
};
