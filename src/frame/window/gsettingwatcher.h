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
#include <QHash>
#include <QMap>

class QGSettings;
class QListView;
class QStandardItem;
class GSettingWatcher : public QObject
{
    Q_OBJECT
public:
    static GSettingWatcher *instance();

    void bind(const QString &gsettingsName, QWidget *binder);
    void bind(const QString &gsettingsName, QListView *viewer, QStandardItem *item);
    void erase(const QString &gsettingsName);
    void erase(const QString &gsettingsName, QWidget *binder);
    void insertState(const QString &);
    const QString getStatus(const QString &gsettingsName);
    QMap<QString, bool> getMenuState();
    QVariant get(const QString &key) const;

private:
    GSettingWatcher(QObject *parent = nullptr);

    void setStatus(const QString &gsettingsName, QWidget *binder);
    void setStatus(const QString &gsettingsName, QListView *viewer, QStandardItem *item);
    void onStatusModeChanged(const QString &key);

Q_SIGNALS:
    void requestUpdateSecondMenu(int);
    void requestUpdateSearchMenu(const QString &, bool);
    void requestShowSecondMenu(int); //显示第二级页面
    void notifyGSettingsChanged(const QString &, const QString &);

private:
    QMultiHash<QString, QWidget *> m_map;
    QGSettings *m_gsettings;
    QHash<QString, QPair<QListView *, QStandardItem *>> m_menuMap;
    QMap<QString, bool> m_menuState;
};

#endif // GSETTINGWATCHER_H
