/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
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

#ifndef MODULEINITTHREAD_H
#define MODULEINITTHREAD_H

#include "interfaces/moduleinterface.h"

#include <QThread>

class ModuleInitThread : public QThread
{
    Q_OBJECT

public:
    explicit ModuleInitThread(dcc::ModuleInterface *inter, QObject *parent = 0);

Q_SIGNALS:
    void moduleInitFinished(dcc::ModuleInterface *inter) const;

protected:
    void run();

private:
    dcc::ModuleInterface *m_module;
};

#endif // MODULEINITTHREAD_H
