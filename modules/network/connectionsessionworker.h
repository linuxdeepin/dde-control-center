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

#ifndef CONNECTIONSESSIONWORKER_H
#define CONNECTIONSESSIONWORKER_H

#include <QObject>

#include <com_deepin_daemon_network_connectionsession.h>

using ConnectionSessionInter = com::deepin::daemon::ConnectionSession;

namespace dcc {

namespace network {

class ConnectionSessionModel;
class ConnectionSessionWorker : public QObject
{
    Q_OBJECT

public:
    explicit ConnectionSessionWorker(const QString &sessionPath, ConnectionSessionModel *model, QObject *parent = 0);

public slots:
    void closeSession();
    void queryAllKeys();
    void saveSettings(const bool active);
    void changeSettings(const QString &section, const QString &vKey, const QString &data, const bool encrypt);
    void changeSettingsEncrypted(const QString &section, const QString &vKey, const QString &data);

private:
    void queryAvailableKeys();

private slots:
    void queryAllKeysCB(QDBusPendingCallWatcher *w);
    void queryAvailableKeysCB(QDBusPendingCallWatcher *w);
    void saveSettingsCB(QDBusPendingCallWatcher *w);
    void changeSettingsEncryptedCB(QDBusPendingCallWatcher *w);

private:
    ConnectionSessionModel *m_connModel;

    ConnectionSessionInter m_sessionInter;
};

}

}

#endif // CONNECTIONSESSIONWORKER_H
