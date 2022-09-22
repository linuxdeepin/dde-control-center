// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef FLIGHTPAGE_H
#define FLIGHTPAGE_H

#include "interface/namespace.h"

#include <DListView>

#include <QWidget>
#include <QStandardItemModel>

#include <com_deepin_daemon_airplanemode.h>

using AirplanInter = com::deepin::daemon::AirplaneMode;

namespace dde {
namespace network {
class NetworkModel;
}
}

namespace dcc {
class ContentWidget;
namespace widgets {
class SettingsGroup;
class SwitchWidget;
class LoadingNextPageWidget;
}
}

namespace DCC_NAMESPACE {
namespace network {
class ConnectionVpnEditPage;
class AirplaneModepage : public QWidget
{
    Q_OBJECT

public:
    explicit AirplaneModepage(QWidget *parent = nullptr);

public Q_SLOTS:
    void AirplaneEnable(const bool enable) const;

private:
    dcc::widgets::SwitchWidget *m_airplaneModeSwitch;  //飞行模式开关
    AirplanInter *m_airplaninter;           //飞行模式dbus接口(system dbus)  com.deepin.daemon.AirplaneMode

    DTK_WIDGET_NAMESPACE::DListView *m_lvprofiles;      //占位控件,为了和其他的保持一致
};

}
}
#endif // FLIGHTPAGE_H
