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

class QGSettings;
class GSettingWatcher : public QObject
{
    Q_OBJECT
public:
    static GSettingWatcher *instance();

    void bind(const QString &gsettingsName, QWidget *binder);
    void erase(const QString &gsettingsName);
    void erase(const QString &gsettingsName, QWidget *binder);
    const QString getStatus(const QString &gsettingsName);

private:
    GSettingWatcher(QObject *parent = nullptr);

    void setStatus(const QString &gsettingsName, QWidget *binder);
    void onStatusModeChanged(const QString &key);

private:
    QMultiHash<QString, QWidget *> m_map;
    QGSettings *m_gsettings;
};

#endif // GSETTINGWATCHER_H
