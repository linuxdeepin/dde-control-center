// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later


#ifndef DEFAPPWORKER_H
#define DEFAPPWORKER_H

#include <QObject>
#include <QMap>

#include <QDBusPendingCall>

#include "mimedbusproxy.h"
#include "category.h"


class QFileInfo;

class DefAppModel;
class Category;
class DefAppWorker : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorker(DefAppModel *m_defAppModel, QObject *parent = 0);

    ~DefAppWorker();
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
    void onSetDefaultTerminal(const App &item);
    void onGetListApps();
    void onDelUserApp(const QString &mine, const App &item);
    void onCreateFile(const QString &mime, const QFileInfo &info);

private Q_SLOTS:
    void getListAppFinished(const QString &mime, const ObjectMap &map);
    void getDefaultAppFinished(const QString &mime, const QString &w);
    void getManagerObjectFinished(QDBusPendingCallWatcher *call);

private:
    DefAppModel *m_defAppModel;
    MimeDBusProxy  *m_dbusManager;
    QMap<QString, DefaultAppsCategory> m_stringToCategory;
    QString m_userLocalPath;

private:
    const QString getTypeByCategory(const DefAppWorker::DefaultAppsCategory &category);
    const QStringList getTypeListByCategory(const DefAppWorker::DefaultAppsCategory &category);
    Category* getCategory(const QString &mime) const;
    // QGSettings *m_defaultTerminal;
    bool executeGsettingsCommand(const QStringList &args, const QString &errorMessage);
};

#endif // DEFAPPWORKER_H
