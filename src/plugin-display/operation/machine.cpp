/*
 * Copyright (C) 2011 ~ 2022 Deepin Technology Co., Ltd.
 *
 * Author:     duanhongyu <duanhongyu@uniontech.com>

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

#include "machine.h"

DCC_USE_NAMESPACE

Machine::Machine(QObject *parent)
    : QObject(parent)
    , m_paired(false)
    , m_cooperating(false)
{

}

Machine::~Machine()
{

}

void Machine::setPath(const QString &path)
{
    m_path = path;
}

void Machine::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        Q_EMIT nameChanged(name);
    }
}

void Machine::setIP(const QString &ip)
{
    if (m_IP != ip) {
        m_IP = ip;
        Q_EMIT IPChanged(ip);
    }
}

void Machine::setPaired(const bool paired)
{
    if (m_paired != paired) {
        m_paired = paired;
        Q_EMIT pairedChanged(paired);
    }
}

void Machine::setCooperating(const bool cooperating)
{
    if (m_cooperating != cooperating) {
        m_cooperating = cooperating;
        Q_EMIT  cooperatingChanged(cooperating);
    }
}

