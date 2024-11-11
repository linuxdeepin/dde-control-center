//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "keyboarddbusproxy.h"

#include <QMetaObject>
#include <QDBusConnection>
#include <QDBusInterface>
#include <QDBusPendingReply>
#include <QDBusMetaType>
#include <QDebug>

const static QString LangSelectorService = "org.deepin.dde.LangSelector1";
const static QString LangSelectorPath = "/org/deepin/dde/LangSelector1";
const static QString LangSelectorInterface = "org.deepin.dde.LangSelector1";

KeyboardDBusProxy::KeyboardDBusProxy(QObject *parent)
    : QObject(parent)
{
    qRegisterMetaType<KeyboardLayoutList>("KeyboardLayoutList");
    qDBusRegisterMetaType<KeyboardLayoutList>();

    qRegisterMetaType<LocaleInfo>("LocaleInfo");
    qDBusRegisterMetaType<LocaleInfo>();

    qRegisterMetaType<LocaleList>("LocaleList");
    qDBusRegisterMetaType<LocaleList>();

    init();
}

void KeyboardDBusProxy::init()
{
    m_dBusLangSelectorInter = new DDBusInterface(LangSelectorService, LangSelectorPath, LangSelectorInterface, QDBusConnection::sessionBus(), this);
}

void KeyboardDBusProxy::langSelectorStartServiceProcess()
{
    if (m_dBusLangSelectorInter->isValid())
    {
        qWarning() << "Service" << LangSelectorService << "is already started.";
        return;
    }

    QDBusInterface freedesktopInter = QDBusInterface("org.freedesktop.DBus", "/", "org.freedesktop.DBus", QDBusConnection::systemBus(), this);
    QDBusMessage msg = QDBusMessage::createMethodCall("org.freedesktop.DBus", "/", "org.freedesktop.DBus", QStringLiteral("StartServiceByName"));
    msg << LangSelectorService << quint32(0);
    QDBusPendingReply<quint32> async = freedesktopInter.connection().asyncCall(msg);
    QDBusPendingCallWatcher *watcher = new QDBusPendingCallWatcher(async, this);

    connect(watcher, &QDBusPendingCallWatcher::finished, this, &KeyboardDBusProxy::onLangSelectorStartServiceProcessFinished);
}

void KeyboardDBusProxy::onLangSelectorStartServiceProcessFinished(QDBusPendingCallWatcher *w)
{

    QDBusPendingReply<quint32> reply = *w;
    Q_EMIT langSelectorServiceStartFinished(reply.value());
    w->deleteLater();
}

//LangSelector
QString KeyboardDBusProxy::currentLocale()
{
    return qvariant_cast<QString>(m_dBusLangSelectorInter->property("CurrentLocale"));
}

int KeyboardDBusProxy::localeState()
{
    return qvariant_cast<int>(m_dBusLangSelectorInter->property("LocaleState"));
}

QStringList KeyboardDBusProxy::locales()
{
    return qvariant_cast<QStringList>(m_dBusLangSelectorInter->property("Locales"));
}

bool KeyboardDBusProxy::langSelectorIsValid()
{
    return m_dBusLangSelectorInter->isValid();
}

QDBusPendingReply<> KeyboardDBusProxy::AddLocale(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("AddLocale"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::DeleteLocale(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("DeleteLocale"), argumentList);
}

QDBusPendingReply<> KeyboardDBusProxy::SetLocale(const QString &in0)
{
    QList<QVariant> argumentList;
    argumentList << QVariant::fromValue(in0);
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("SetLocale"), argumentList);
}

QDBusPendingReply<LocaleList> KeyboardDBusProxy::GetLocaleList()
{
    QList<QVariant> argumentList;
    return m_dBusLangSelectorInter->asyncCallWithArgumentList(QStringLiteral("GetLocaleList"), argumentList);
}
