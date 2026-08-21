//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>
#include <tuple>
#include <DDBusInterface>

using Dtk::Core::DDBusInterface;

class QDBusMessage;

class SecurityDBusProxy : public QObject
{
    Q_OBJECT
public:
    explicit SecurityDBusProxy(QObject *parent = nullptr);

    QString Status();
    std::tuple<QString, QString> GetSEUserByName(const QString &user);

    inline QString lastError() { return m_lastError; }

private:
    void init();

private:
    DDBusInterface *m_dBusInter;
    QString m_lastError;
};
