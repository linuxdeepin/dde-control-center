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

#pragma once

#include "interface/namespace.h"

#include <com_deepin_daemon_inputdevice_wacom.h>
#include <QObject>

using Wacom = com::deepin::daemon::inputdevice::Wacom;

namespace DCC_NAMESPACE {
namespace wacom {

class WacomModel;

class WacomWorker : public QObject
{
    Q_OBJECT
public:
    explicit WacomWorker(WacomModel *model, QObject *parent = nullptr);
    void active();
    void deactive();

public Q_SLOTS:
    void setPressureSensitive(const uint value);
    void setCursorMode(const bool value);
    void onPressureSensitiveChanged(const uint value);
    void onCursorModeChanged(const bool value);
    bool exist();

private:
    int converToPressureValue(int value);
    int converToModelPressureValue(int value);

private:
    Wacom *m_dbusWacom;
    WacomModel *m_model;
};
}
}
