/*
 * Copyright (C) 2011 ~ 2020 Uniontech Technology Co., Ltd.
 *
 * Author:     xiehui <xiehui@uniontech.com>
 *
 * Maintainer: xiehui <xiehui@uniontech.com>
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
