/*
* Copyright (C) 2021 ~ 2021 Deepin Technology Co., Ltd.
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
#pragma once

#include <QObject>
#include <tuple>
#include "interface/namespace.h"

namespace DCC_NAMESPACE {
class DCCDBusInterface;
}

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
    DCC_NAMESPACE::DCCDBusInterface *m_dBusInter;
    QString m_lastError;
};
