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

#ifndef POWERMODULE_H
#define POWERMODULE_H

#include "interfaces/moduleinterface.h"

#include <QObject>

namespace dcc {
namespace power {
class PowerModel;
class PowerWidget;
class PowerWorker;
class PowerModule : public QObject, public dcc::ModuleInterface
{
    Q_OBJECT

public:
    explicit PowerModule(dcc::FrameProxyInterface *frame, QObject *parent = 0);

private:
    ~PowerModule();

    void initialize();
    void moduleActive();
    void moduleDeactive();
    void reset();
    void contentPopped(dcc::ContentWidget * const w);
    dcc::ModuleWidget *moduleWidget();
    const QString name() const;


private:
    PowerModel *m_powerModel;
    PowerWidget *m_powerWidget;
    PowerWorker *m_powerWorker;
};
}
}

#endif // POWERMODULE_H
