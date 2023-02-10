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

#ifndef DEFAPPWORKER_H
#define DEFAPPWORKER_H

#include "category.h"
#include "mimedbusproxy.h"

#include <QMap>
#include <QObject>

class QFileInfo;

class DefAppModel;
class Category;

class DefAppWorker : public QObject
{
    Q_OBJECT
public:
    explicit DefAppWorker(DefAppModel *m_defAppModel, QObject *parent = 0);

    enum DefaultAppsCategory { Browser, Mail, Text, Music, Video, Picture, Terminal };

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
    MimeDBusProxy *m_dbusManager;
    QMap<QString, DefaultAppsCategory> m_stringToCategory;
    QString m_userLocalPath;

private:
    const QString getTypeByCategory(const DefAppWorker::DefaultAppsCategory &category);
    const QStringList getTypeListByCategory(const DefAppWorker::DefaultAppsCategory &category);
    Category *getCategory(const QString &mime) const;
};

#endif // DEFAPPWORKER_H
