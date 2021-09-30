/*
 * Copyright (C) 2020 ~ 2021 Uniontech Technology Co., Ltd.
 *
 * Author:     xuyanghe <xuyanghe@uniontech.com>
 *
 * Maintainer: xuyanghe <xuyanghe@uniontech.com>
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

#ifndef GSETTINGWATCHER_H
#define GSETTINGWATCHER_H

#include <QObject>

#include <com_deepin_dde_modulevisible.h>
#include <QHash>

using  ModuleVisbleDbus = com::deepin::dde::ModuleVisible;
class QGSettings;
class QListView;
class QStandardItem;
class VisibleManagement : public QObject
{
    Q_OBJECT
public:
    static VisibleManagement *instance();

    void bind(const QString &moduleName, QWidget *binder);
    void bind(const QString &moduleName, QListView *viewer, QStandardItem *item);
    void erase(const QString &gsettingsName);
    void erase(const QString &gsettingsName, QWidget *binder);
    void setAppName(const QString &appName);
    void clearMenuMap();
    bool getStatus(const QString &moduleName);
    QMap<QString, bool> getSeartchState() { return m_seartchMap; }

private:
    VisibleManagement(QObject *parent = nullptr);

    void setStatus(QWidget *binder, const bool visible);
    void initData();
    void setStatus(QListView *viewer, QStandardItem *item, const bool visible);

    void updateData(const QString &data);

    void initDataFromConfFile();
    void initDataFromDbus(const QString &visibleData);
    void initSeartchData();
    void initStatus();

private Q_SLOTS:
    void onPageVisibleChanged(const QString &appName, const QString &moduleName, const QString &pageName, const bool enable);
    void onAllDataChanged(const QString &data);

Q_SIGNALS:
    void requestUpdateSecondMenu(int);
    void requestSearchMenuAll();
    void requestUpdateSearchMenu(const QString &, bool);
    void requestDataChanged();
    void requestCurrentPageOff();

private:
    QHash<QString, QWidget *> m_map;
    QHash<QString, bool> m_Visibledata;
    QMap<QString, bool> m_seartchMap;
    ModuleVisbleDbus *m_moduleVisble;
    QString m_appName;

    QHash<QString, QPair<QListView *, QStandardItem *>> m_menuMap;
};

#endif // GSETTINGWATCHER_H
