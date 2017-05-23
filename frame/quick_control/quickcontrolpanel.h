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
