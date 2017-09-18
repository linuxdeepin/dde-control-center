/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#ifndef QUICKCONTROLPANEL_H
#define QUICKCONTROLPANEL_H

#include <QWidget>
#include <QStackedLayout>
#include <QPushButton>

namespace dcc {

class QuickSwitchButton;

namespace network {

class NetworkModel;
class NetworkWorker;

}

namespace display {

class DisplayModel;
class DisplayWorker;

}

namespace bluetooth {

class BluetoothModel;
class BluetoothWorker;

}

}

class QuickControlPanel : public QWidget
{
    Q_OBJECT

public:
    explicit QuickControlPanel(QWidget *parent = 0);

    void setAllSettingsVisible(const bool visible);

signals:
    void requestDetailConfig() const;
    void requestPage(const QString &module, const QString &page) const;

protected:
    void leaveEvent(QEvent *e);

private slots:
    void onNetworkDeviceEnableChanged();
    void onNetworkDeviceListChanged();
    void onNetworkConnectionListChanged();
    void onWirelessButtonClicked();
    void onBluetoothDeviceEnableChanged();
    void onBluetoothButtonClicked(const bool checked);
    void onBluetoothDeviceListChanged();
    void onIndexChanged(const int index);

private:
    QStackedLayout *m_itemStack;
    dcc::network::NetworkModel *m_networkModel;
    dcc::network::NetworkWorker *m_networkWorker;
    dcc::display::DisplayModel *m_displayModel;
    dcc::display::DisplayWorker *m_displayWorker;
    dcc::bluetooth::BluetoothModel *m_bluetoothModel;
    dcc::bluetooth::BluetoothWorker *m_bluetoothWorker;

    dcc::QuickSwitchButton *m_wifiSwitch;
    dcc::QuickSwitchButton *m_btSwitch;
    dcc::QuickSwitchButton *m_vpnSwitch;
    dcc::QuickSwitchButton *m_detailSwitch;
    QList<dcc::QuickSwitchButton *> m_switchs;
};

#endif // QUICKCONTROLPANEL_H
