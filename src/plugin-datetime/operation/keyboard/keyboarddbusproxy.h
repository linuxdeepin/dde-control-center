//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef KEYBOARDDBUSPROXY_H
#define KEYBOARDDBUSPROXY_H

#include "../datetimedbusproxy.h"
// class LocaleInfo;
// typedef QList<LocaleInfo> LocaleList;

#include <DDBusInterface>

#include <QObject>
#include <QDBusPendingReply>

class QDBusInterface;
class QDBusMessage;

using Dtk::Core::DDBusInterface;

typedef QMap<QString, QString> KeyboardLayoutList;

namespace dccV25 {
class DCCDBusInterface;
}

class KeyboardDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit KeyboardDBusProxy(QObject *parent = nullptr);

    //LangSelector
    Q_PROPERTY(QString CurrentLocale READ currentLocale NOTIFY CurrentLocaleChanged)
    QString currentLocale();

    Q_PROPERTY(int LocaleState READ localeState NOTIFY LocaleStateChanged)
    int localeState();

    Q_PROPERTY(QStringList Locales READ locales NOTIFY LocalesChanged)
    QStringList locales();

    bool langSelectorIsValid();
    void langSelectorStartServiceProcess();

signals:
    // LangSelector property
    void CurrentLocaleChanged(const QString & value) const;
    void LocaleStateChanged(int value) const;
    void LocalesChanged(const QStringList & value) const;
    void langSelectorServiceStartFinished(const quint32 ret) const;

public slots:
    QDBusPendingReply<> AddLocale(const QString &in0);
    QDBusPendingReply<> DeleteLocale(const QString &in0);
    QDBusPendingReply<> SetLocale(const QString &in0);
    QDBusPendingReply<LocaleList> GetLocaleList();

private slots:
    void onLangSelectorStartServiceProcessFinished(QDBusPendingCallWatcher *w);
private:
    void init();

private:
    DDBusInterface *m_dBusLangSelectorInter;
};

Q_DECLARE_METATYPE(KeyboardLayoutList)

#endif // KeyboardDBusProxy_H
