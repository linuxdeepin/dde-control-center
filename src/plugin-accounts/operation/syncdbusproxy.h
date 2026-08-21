//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <QDBusPendingReply>

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
