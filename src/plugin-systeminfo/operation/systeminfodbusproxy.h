//Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMINFODBUSPROXY_H
#define SYSTEMINFODBUSPROXY_H

#include "interface/namespace.h"
#include <QObject>

namespace DCC_NAMESPACE {
class DCCDBusInterface;
}

class SystemInfoDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit SystemInfoDBusProxy(QObject *parent = nullptr);

    Q_PROPERTY(QString StaticHostname READ staticHostname WRITE setStaticHostname NOTIFY StaticHostnameChanged)
    QString staticHostname();
    void setStaticHostname(const QString &value);
    void setStaticHostname(const QString &value, QObject *receiver, const char *member, const char *errorSlot);

    Q_PROPERTY(int AuthorizationState READ authorizationState WRITE setAuthorizationState NOTIFY AuthorizationStateChanged)
    int authorizationState();
    void setAuthorizationState(const int value);

signals:
    void StaticHostnameChanged(const QString &value) const;
    void AuthorizationStateChanged(const int value) const;

public slots:
    void Show();

private:
    DCC_NAMESPACE::DCCDBusInterface *m_hostname1Inter;
    DCC_NAMESPACE::DCCDBusInterface *m_licenseInfoInter;
    DCC_NAMESPACE::DCCDBusInterface *m_licenseActivatorInter;
};

#endif // SYSTEMINFODBUSPROXY_H
