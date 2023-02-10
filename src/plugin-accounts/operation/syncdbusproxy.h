/*
 * Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     guoyao <guoyao@uniontech.com>
 *
 * Maintainer: guoyao <guoyao@uniontech.com>
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
#pragma once

#include <QDBusPendingReply>
#include <QObject>

class QDBusInterface;
class QDBusMessage;

class SyncDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit SyncDBusProxy(QObject *parent = nullptr);

    QString UOSID();
    QString LocalBindCheck(const QString &uosid, const QString &uuid);

    inline QString lastError() { return m_lastError; }

private:
    void init();

private:
    QDBusInterface *m_dBusInter;
    QString m_lastError;
};
