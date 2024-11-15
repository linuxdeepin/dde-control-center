//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef SYSTEMINFODBUSPROXY_H
#define SYSTEMINFODBUSPROXY_H

#include <DDBusInterface>

using Dtk::Core::DDBusInterface;

#include <QObject>

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

    Q_PROPERTY(QString Timezone READ timezone WRITE setTimezone NOTIFY TimezoneChanged)
    QString timezone();
    void setTimezone(const QString &value);

    Q_PROPERTY(int ShortDateFormat READ shortDateFormat NOTIFY ShortDateFormatChanged)
    int shortDateFormat();

    // userexperience
    void Enable(const bool value);
    bool IsEnabled();
    qulonglong memorySize();

Q_SIGNALS:
    void StaticHostnameChanged(const QString &value) const;
    void AuthorizationStateChanged(const int value) const;
    void TimezoneChanged(const QString &value) const;
    void ShortDateFormatChanged(const int value) const;

public Q_SLOTS:
    void Show();

private:
    DDBusInterface *m_hostname1Inter;
    DDBusInterface *m_licenseInfoInter;
    DDBusInterface *m_licenseActivatorInter;
    DDBusInterface *m_userexperienceInter;
    DDBusInterface *m_systemInfo;
    DDBusInterface *m_timedateInter;
    DDBusInterface *m_timeZoneInter;
};

#endif // SYSTEMINFODBUSPROXY_H
