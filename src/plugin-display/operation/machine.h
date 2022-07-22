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

#ifndef MACHINE_H
#define MACHINE_H

#include <QObject>
#include "interface/namespace.h"

namespace DCC_NAMESPACE {

class Machine : public QObject
{
    Q_OBJECT
public:
    explicit Machine(QObject *parent = 0);
    ~Machine();

    void setPath(const QString& path);
    inline QString Path() { return m_path; }

    inline const QString Name() { return m_name; }
    inline const QString IP() { return m_IP; }
    inline const bool Paired() { return m_paired; }
    inline const bool Cooperating() { return m_cooperating; }

public Q_SLOTS:
    void setName(const QString& name);
    void setIP(const QString& ip);
    void setPaired(const bool paired);
    void setCooperating(const bool cooperating);

Q_SIGNALS:
    void pairedChanged(bool paired);
    void nameChanged(const QString& name);
    void IPChanged(const QString& Ip);
    void cooperatingChanged(bool cooperating);

private:
    QString m_path;
    QString m_name;
    QString m_IP;
    bool m_paired;
    bool m_cooperating;
};

}
#endif // MACHINE_H
