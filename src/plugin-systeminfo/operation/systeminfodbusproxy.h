/*
 * Copyright (C) 2022 ~ 2022 Deepin Technology Co., Ltd.
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
#ifndef SYSTEMINFODBUSPROXY_H
#define SYSTEMINFODBUSPROXY_H

#include <QObject>
class QDBusInterface;
class QDBusMessage;

class SystemInfoDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfoDBusProxy(QObject *parent = nullptr);

    Q_PROPERTY(QString StaticHostname READ staticHostname WRITE setStaticHostname NOTIFY StaticHostnameChanged)
    QString staticHostname();
    void setStaticHostname(const QString &value);

    Q_PROPERTY(int AuthorizationState READ authorizationState WRITE setAuthorizationState NOTIFY AuthorizationStateChanged)
    int authorizationState();
    void setAuthorizationState(const int value);

signals:
    void StaticHostnameChanged(const QString &value) const;
    void AuthorizationStateChanged(const int value) const;

public slots:
    void Show();

private slots:
    void onPropertiesChanged(const QDBusMessage &message);

private:
    QDBusInterface *m_hostname1Inter;
    QDBusInterface *m_licenseInfoInter;
    QDBusInterface *m_licenseActivatorInter;
};

#endif // SYSTEMINFODBUSPROXY_H
