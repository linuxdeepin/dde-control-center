// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WACOMWORKER_H
#define WACOMWORKER_H

#include <com_deepin_daemon_inputdevice_wacom.h>
#include <QObject>

using com::deepin::daemon::inputdevice::Wacom;
namespace dcc
{
namespace wacom
{
class WacomModel;
class WacomWorker : public QObject
{
    Q_OBJECT
public:
    explicit WacomWorker(WacomModel *model, QObject *parent = 0);
    void active();
    void deactive();

public Q_SLOTS:
    void setPressureSensitive(const int value);
    void onPressureSensitiveChanged(const int value);
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


#endif // WACOMWORKER_H
