// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef DEFAPPWORKER_H
#define DEFAPPWORKER_H

#include "model/category.h"

#include <com_deepin_daemon_mime.h>
#include <QObject>
using com::deepin::daemon::Mime;

namespace dcc
{
namespace defapp
{
class DefAppModel;
class Category;
class DefAppWorker : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorker(DefAppModel *m_defAppModel, QObject *parent = 0);

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
    Mime *getDbusObject() { return m_dbusManager; }

public Q_SLOTS:
    void onSetDefaultApp(const QString &category, const App &item);
    void onGetListApps();
    void onDelUserApp(const QString &mine, const App &item);
    void onCreateFile(const QString &mime, const QFileInfo &info);

private Q_SLOTS:
    void getListAppFinished(QDBusPendingCallWatcher *w);
    void getDefaultAppFinished(QDBusPendingCallWatcher *w);
    void saveListApp(const QString &mime, const QJsonArray &json, const bool isUser);
    void saveDefaultApp(const QString &mime, const QJsonObject &json);

private:
    DefAppModel *m_defAppModel;
    Mime     *m_dbusManager;
    QMap<QString, DefaultAppsCategory> m_stringToCategory;
    QString m_userLocalPath;

private:
    const QString getTypeByCategory(const DefAppWorker::DefaultAppsCategory &category);
    const QStringList getTypeListByCategory(const DefAppWorker::DefaultAppsCategory &category);
    Category* getCategory(const QString &mime) const;
};
}
}

#endif // DEFAPPWORKER_H
