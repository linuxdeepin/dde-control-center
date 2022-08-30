// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
