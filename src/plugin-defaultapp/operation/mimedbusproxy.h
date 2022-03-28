/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
*
* Author:     duanhongyu <duanhongyu@uniontech.com>
*
* Maintainer: duanhongyu <duanhongyu@uniontech.com>
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
#ifndef MIMEDBUSPROXY_H
#define MIMEDBUSPROXY_H

#include <QObject>

class QDBusInterface;
class QDBusMessage;

class MimeDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit MimeDBusProxy(QObject *parent = nullptr);

    void SetDefaultApp(const QStringList &mimeTypes, const QString &desktopId);
    void DeleteApp(const QStringList &mimeTypes, const QString &desktopId);
    void DeleteUserApp(const QString &desktopId);
    void AddUserApp(const QStringList &mimeTypes, const QString &desktopId);

    QString GetDefaultApp(const QString &mimeType);
    QString ListApps(const QString &mimeType);

    QString ListUserApps(const QString &mimeType);

Q_SIGNALS: // SIGNALS
    void Change();
    // begin property changed signals

private:
    QDBusInterface *m_mimeInter;
};

#endif // MIMEDBUSPROXY_H
