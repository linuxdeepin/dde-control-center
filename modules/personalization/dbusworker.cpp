/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dbusworker.h"
#include "dbus/appearancedaemon_interface.h"
#include <QStringList>
#include <QDBusConnection>
#include <QDBusPendingReply>
#include <QByteArray>
#include <QJsonDocument>
#include <QDebug>

void listSort(QStringList &list)
{
    qSort(list.begin(), list.end());

    int index = 0;

    for (int i = 0; i < list.count(); ++i) {
        if (list[i].toLower().contains("deepin")) {
            list.move(i, index++);
        }
    }
}

DBusWorker::DBusWorker(QObject *parent) : QObject(parent)
{
    m_appearanceDaemonInterface = new AppearanceDaemonInterface(staticServiceName(),
            staticObjectPathName(),
            QDBusConnection::sessionBus(),
            this);

    connect(m_appearanceDaemonInterface, &AppearanceDaemonInterface::Changed,
    m_appearanceDaemonInterface, [this](const QString & str1, const QString & val) {
        qDebug() << "AppearanceDaemonInterface::Changed" << str1 << val;

        if (str1 == staticTypeKeys.value("TypeBackground")) {
            getDetails(staticTypeKeys.value("TypeBackground"), m_backgroundKeys, m_backgroundObjs, m_backgroundDetails);
            emit backgroundDetailsChanged(m_backgroundDetails);
            emit backgroundKeysChanged(m_backgroundKeys);
            emit backgroundChanged(val);
            return;
        }

        if (str1 == staticTypeKeys.value("TypeGtkTheme")) {
            getDetails(staticTypeKeys.value("TypeGtkTheme"), m_windowKeys, m_windowObjs, m_windowDetails);
            qDebug() << m_windowKeys.size() << m_windowDetails.size();
            emit windowChanged(val);
        }

        if (str1 == staticTypeKeys.value("TypeIconTheme")) {
            getDetails(staticTypeKeys.value("TypeIconTheme"), m_iconKeys, m_iconObjs, m_iconDetails);
            emit iconChanged(val);
        }

        if (str1 == staticTypeKeys.value("TypeCursorTheme")) {
            getDetails(staticTypeKeys.value("TypeCursorTheme"), m_cursorKeys, m_cursorObjs, m_cursorDetails);
            emit cursorChanged(val);
        }

        if (str1 == staticTypeKeys.value("TypeStandardFont")) {
            getDetails(staticTypeKeys.value("TypeStandardFont"), m_standardFontKeys, m_standardFontObjs);
            emit standardFontChanged(val);
        }

        if (str1 == staticTypeKeys.value("TypeMonospaceFont")) {
            getDetails(staticTypeKeys.value("TypeMonospaceFont"), m_monospaceFontKeys, m_monospaceFontObjs);
            emit monospaceFontChanged(val);
        }

        if (str1 == staticTypeKeys.value("TypeFontSize")) {
            getFontSize();
            emit fontSizeChanged(val.toInt());
        }
    });
}

void DBusWorker::doWork()
{
    QTimer *delayTimer = new QTimer;
    delayTimer->setSingleShot(true);
    delayTimer->setInterval(200);
    connect(delayTimer, SIGNAL(timeout()), this, SLOT(delayGetData()));
    connect(delayTimer, SIGNAL(timeout()), delayTimer, SLOT(deleteLater()));
    delayTimer->start();
}

void DBusWorker::delayGetData()
{
    getDetails(staticTypeKeys.value("TypeGtkTheme"), m_windowKeys, m_windowObjs, m_windowDetails);
    getDetails(staticTypeKeys.value("TypeIconTheme"), m_iconKeys, m_iconObjs, m_iconDetails);
    getDetails(staticTypeKeys.value("TypeCursorTheme"), m_cursorKeys, m_cursorObjs, m_cursorDetails);
    getDetails(staticTypeKeys.value("TypeBackground"), m_backgroundKeys, m_backgroundObjs, m_backgroundDetails);
    getDetails(staticTypeKeys.value("TypeStandardFont"), m_standardFontKeys, m_standardFontObjs);
    getDetails(staticTypeKeys.value("TypeMonospaceFont"), m_monospaceFontKeys, m_monospaceFontObjs);

    getFontSize();

    emit windowKeysChanged(m_windowKeys);
    emit iconKeysChanged(m_iconKeys);
    emit cursorKeysChanged(m_cursorKeys);
    emit backgroundKeysChanged(m_backgroundKeys);

    emit windowDetailsChanged(m_windowDetails);
    emit iconDetailsChanged(m_iconDetails);
    emit cursorDetailsChanged(m_cursorDetails);
    emit backgroundDetailsChanged(m_backgroundDetails);
    emit standardFontDetailsChanged(m_standardFontKeys);
    emit monospaceFontDetailsChanged(m_monospaceFontKeys);

    emit windowChanged(m_appearanceDaemonInterface->gtkTheme());
    emit iconChanged(m_appearanceDaemonInterface->iconTheme());
    emit cursorChanged(m_appearanceDaemonInterface->cursorTheme());
    emit backgroundChanged(m_appearanceDaemonInterface->background());
    emit standardFontChanged(m_appearanceDaemonInterface->standardFont());
    emit monospaceFontChanged(m_appearanceDaemonInterface->monospaceFont());

    emit fontSizeChanged(m_fontSize);

    emit dataFinished();
}

QString DBusWorker::getThumbnail(QString Type, QString key)
{
    QDBusPendingReply<QString> reply = m_appearanceDaemonInterface->Thumbnail(Type, key);
    reply.waitForFinished();
    QString thumbnail;
    if (!reply.isError()) {
        thumbnail = reply.argumentAt(0).toString();
    } else {
        qDebug() << reply.error().message();
    }
    return thumbnail;
}

void DBusWorker::getDetails(QString Type, QStringList &keys, JosnMapObjs &objs)
{
    keys.clear();
    objs.clear();
    QDBusPendingReply<QString> reply = m_appearanceDaemonInterface->List(Type);
    reply.waitForFinished();
    if (!reply.isError()) {
        QJsonDocument doc = QJsonDocument::fromJson(reply.argumentAt(0).toString().toLocal8Bit());
        for (int i = 0; i < doc.array().count() ; i++) {
            QJsonObject obj = doc.array().at(i).toObject();
            QString id = obj.value("Id").toString();
            objs.insert(id, obj);
            keys.append(id);
        }
    } else {
        qDebug() << reply.error().message();
    }

    listSort(keys);
}

ImageInfoList &DBusWorker::getDetails(QString Type, QStringList &keys, JosnMapObjs &objs, ImageInfoList &details)
{
    getDetails(Type, keys, objs);
    details.clear();
    foreach(QString key, keys) {
        QString thumbnail = getThumbnail(Type, key);
        if (thumbnail.length() > 0) {
            QString name = key;
            QString deletable("false");
            if (objs.value(key).contains("Deletable")) {
                if (objs.value(key).value("Deletable").toBool()) {
                    deletable = "true";
                }
            }
            QMap<QString, QString> detail {
                {"key", key},
                {"name", name},
                {"url", thumbnail},
                {"deletable", deletable}
            };
            details.append(detail);
        }
    }
    return details;
}


int DBusWorker::getFontSize()
{
    m_fontSize = m_appearanceDaemonInterface->fontSize();
    return m_fontSize;
}

void DBusWorker::setTheme(QString Type, QString Key)
{
    if (!m_setThemeTimer) {
        m_setThemeTimer = new QTimer(this);

        connect(m_setThemeTimer, &QTimer::timeout, this, [this] {
            m_setThemeTimer->deleteLater();
            m_setThemeTimer = nullptr;
            m_appearanceDaemonInterface->Set(m_setThemeType, m_setThemeKey);
            qDebug() << "DBusWorker::setTheme" << m_setThemeType << m_setThemeKey;
        });
    }

    m_setThemeType = Type;
    m_setThemeKey = Key;
    m_setThemeTimer->start(500);
}

void DBusWorker::deleteItem(QString Type, QString Key)
{
    QDBusPendingReply<> reply = m_appearanceDaemonInterface->Delete(Type, Key);
    reply.waitForFinished();
    if (!reply.isError()) {
        qDebug() << "delete" << Type << Key << "successfully";
        if (Type == staticTypeKeys.value("TypeBackground")) {
            getDetails(staticTypeKeys.value("TypeBackground"), m_backgroundKeys, m_backgroundObjs, m_backgroundDetails);
            emit backgroundKeysChanged(m_backgroundKeys);
            emit backgroundDetailsChanged(m_backgroundDetails);
            emit backgroundChanged(m_appearanceDaemonInterface->background());
        }
    } else {
        qDebug() << reply.error().message();
    }
}

DBusWorker::~DBusWorker()
{

}
