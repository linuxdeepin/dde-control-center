#include "networkhandler.h"
#include <iostream>

#include "networkcontroller.h"
#include "networkdevicebase.h"
#include "wirelessdevice.h"
#include "wireddevice.h"
#include "vpncontroller.h"
#include "proxycontroller.h"
#include "hotspotcontroller.h"
#include "dslcontroller.h"
#include "networkdetails.h"

#include <QTextStream>
#include <QKeyEvent>

using namespace std;

using namespace dde::network;

static void outputMessage(const QString &s)
{
    qInfo().noquote() << s;
}

static int getInput(const QString &inputMessage, QString &inputStr)
{
    QTextStream qin(stdin);
    QTextStream qout(stdout);

    qout << inputMessage << endl;
    qin >> inputStr;
    int selection = inputStr.toInt();
    return selection;
}

NetworkHandler::NetworkHandler(QObject *parent)
    : QObject(parent)
{
    initConnection();
    initProxy();
    initHotsPot();
    initDSL();
}

NetworkHandler::~NetworkHandler()
{
}

void NetworkHandler::initConnection()
{
    // 网络模块
    NetworkController *controller = NetworkController::instance();
    connect(controller, &NetworkController::deviceAdded, this, &NetworkHandler::onDeviceAdded);
    connect(controller, &NetworkController::deviceRemoved, this, &NetworkHandler::onDeviceRemoved);
    connect(controller, &NetworkController::connectionChanged, this, &NetworkHandler::onStatusChanged);
    connect(controller, &NetworkController::connectivity, this, &NetworkHandler::onConnectivityChanged);

    // VPN模块
    VPNController *vpnController = controller->vpnController();
    connect(vpnController, &VPNController::enableChanged, this, &NetworkHandler::onVpnEnabled);
    connect(vpnController, &VPNController::itemAdded, this, &NetworkHandler::onItemAdded);
    connect(vpnController, &VPNController::itemRemoved, this, &NetworkHandler::onItemRemoved);
    connect(vpnController, &VPNController::activeConnectionChanged, this, &NetworkHandler::onVPNActiveChanged);

    // Proxy模块
    ProxyController *proxyController = controller->proxyController();
    connect(proxyController, &ProxyController::autoProxyChanged, this, &NetworkHandler::onAutoProxyChanged);
    connect(proxyController, &ProxyController::proxyChanged, this, &NetworkHandler::onProxyChanged);
    connect(proxyController, &ProxyController::proxyMethodChanged, this, &NetworkHandler::onProxyMethodChange);
    connect(proxyController, &ProxyController::proxyIgnoreHostsChanged, this, &NetworkHandler::onProxyIgnoreHostsChanged);
    connect(proxyController, &ProxyController::appUsernameChanged, this, &NetworkHandler::onAppProxyChanged);
    connect(proxyController, &ProxyController::appPasswordChanged, this, &NetworkHandler::onAppProxyChanged);
    connect(proxyController, &ProxyController::appTypeChanged, this, &NetworkHandler::onAppProxyChanged);
    connect(proxyController, &ProxyController::appIPChanged, this, &NetworkHandler::onAppProxyChanged);
    connect(proxyController, &ProxyController::appPortChanged, this, &NetworkHandler::onAppProxyChanged);

    // 无线热点模块
    HotspotController *hotspotController = NetworkController::instance()->hotspotController();
    connect(hotspotController, &HotspotController::itemAdded, this, &NetworkHandler::onHotspotChanged);

    // DSL
    DSLController *dslController = NetworkController::instance()->dslController();
    connect(dslController, &DSLController::activeConnectionChanged, this, &NetworkHandler::onDSLActiveChanged);
}

void NetworkHandler::initProxy()
{
    ProxyController *proxyController = NetworkController::instance()->proxyController();
    proxyController->querySysProxyData();
}

void NetworkHandler::initHotsPot()
{
    HotspotController *ctrl = NetworkController::instance()->hotspotController();
    if (ctrl->supportHotspot()) {
        QList<WirelessDevice *> devices = ctrl->devices();
        for (WirelessDevice *device : devices) {
            QList<HotspotItem *> items = ctrl->items(device);
            for (HotspotItem *item : items)
                outputMessage("this item is in" + item->name());
        }
    }
}

void NetworkHandler::initDSL()
{
    DSLController *controll = NetworkController::instance()->dslController();
    QList<DSLItem *> items = controll->items();
    for (DSLItem *item : items)
        outputMessage(QString(tr("dslItems:%1")).arg(item->connection()->id()));
}

void NetworkHandler::onDeviceInput() const
{
    QString inputStr;
    int ope = getInput("please select the option: 0: disable device 1:enable device "
                       "2:querySystemProxy 3:queryAppProxy 4:DSL 5:hotspot 6:vpn 7 appProxy "
                       "8:sysProxy 9:testMulit"
                       "other:continue", inputStr);
    switch (ope) {
    case 0: {
        // 禁用设备
        getInput("select the device index, split with,", inputStr);
        QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
        QStringList allInputStr = inputStr.split(",");
        for (int i = 0; i < devices.size(); i++) {
            NetworkDeviceBase *device = devices[i];
            if (allInputStr.contains(QString::number(i))) {
                if (device->isEnabled())
                    device->setEnabled(false);
                else
                    outputMessage("the device is disabled,can't disabled it");
            }
        }
        break;
    }
    case 1: {
        // 启用设备
        getInput("select the device index", inputStr);
        QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
        QStringList allInputStr = inputStr.split(",");
        for (int i = 0; i < devices.size(); i++) {
            NetworkDeviceBase *device = devices[i];
            if (allInputStr.contains(QString::number(i))) {
                if (!device->isEnabled())
                    device->setEnabled(true);
                else
                    outputMessage("the device is enabled,can't disabled it");
            }
        }
        break;
    }
    case 2: {
        ProxyController *pProxyController = NetworkController::instance()->proxyController();
        pProxyController->querySysProxyData();
        // 查询系统Proxy
        break;
    }
    case 3: {
        AppProxyConfig proxyConfig = NetworkController::instance()->proxyController()->appProxy();
        outputMessage("appProxy:Ip " + proxyConfig.ip + ",port " + QString::number(proxyConfig.port)
                      + ",username " + proxyConfig.username + ",password " + proxyConfig.password);
        // 查询应用Proxy
        break;
    }
    case 4: {
        DSLController *ctrl = NetworkController::instance()->dslController();
        QList<DSLItem *> items = ctrl->items();
        for (int i = 0; i < items.size(); i++) {
            DSLItem *item = items[i];
            outputMessage(QString("DSL item %1 : %2").arg(i).arg(item->connection()->id()));
        }

        int n = getInput("select operation, 0:connectItem 1:disconnectItem other:continue", inputStr);
        if (n >= 0 && n <= 1) {
            if (n == 0) {
                n = getInput("select index", inputStr);
                if (n >= 0 && n < items.size()) {
                    DSLItem *item = items[n];
                    ctrl->connectItem(item);
                }
            } else {
                ctrl->disconnectItem();
            }
        }
        break;
    }
    case 5: {
        HotspotController *ctrl = NetworkController::instance()->hotspotController();
        QList<WirelessDevice *> wirelessDevices = ctrl->devices();
        outputMessage("suport hotspot devices");
        for (int i = 0; i < wirelessDevices.size(); i++) {
            WirelessDevice *device = wirelessDevices[i];
            outputMessage(QString("%1:%2:%3:%4").arg(i).arg(device->deviceName())
                          .arg(device->isEnabled()).arg(device->path()));
        }

        int n = getInput("select device index", inputStr);
        if (n >= 0 && n < wirelessDevices.size()) {
            WirelessDevice *device = wirelessDevices[n];
            QList<HotspotItem *> items = ctrl->items(device);
            for (int i = 0; i < items.size(); i++) {
                HotspotItem *item = items[i];
                outputMessage(QString("DSL item %1 : %2").arg(i).arg(item->connection()->ssid()));
            }
            n = getInput("0:connectItem 1:disconnectItem other:continue", inputStr);
            if (n >= 0 && n <= 1) {
                if (n == 0) {
                    n = getInput("select index", inputStr);
                    if (n >= 0 && n < items.size()) {
                        HotspotItem *item = items[n];
                        ctrl->connectItem(item);
                    }
                } else {
                    ctrl->disconnectItem();
                }
            }
        }
        break;
    }
    case 6: {
        VPNController *ctrl = NetworkController::instance()->vpnController();
        QList<VPNItem *> items = ctrl->items();
        for (int i = 0; i < items.size(); i++) {
            VPNItem *item = items[i];
            outputMessage(QString("vpnItem %1: %2").arg(i).arg(item->connection()->id()));
        }
        int n = getInput("select operation, 0:connectItem 1:disconnectItem other:continue", inputStr);
        if (n >= 0 && n <= 1) {
            if (n == 0) {
                n = getInput("select index", inputStr);
                if (n >= 0 && n < items.size()) {
                    VPNItem *item = items[n];
                    ctrl->connectItem(item);
                }
            } else {
                ctrl->disconnectItem();
            }
        }
        break;
    }
    case 7: {
        ProxyController *ctrl = NetworkController::instance()->proxyController();
        if (ctrl->appProxyExist()) {
            AppProxyConfig proxy = ctrl->appProxy();
        }
        break;
    }
    case 8: {
        ProxyController *ctrl = NetworkController::instance()->proxyController();
        AppProxyConfig conf;
        conf.ip = "127.0.0.1";
        conf.port = 2531;
        conf.type = AppProxyType::Http;
        ctrl->setAppProxy(conf);
        break;
    }
    default: break;
    }
}

void NetworkHandler::onDeviceAdded(QList<NetworkDeviceBase *> devices)
{
    outputMessage("received DeviceAdded signal:");
    for (int i = 0; i < devices.size(); i++) {
        NetworkDeviceBase *device = devices[i];
        connect(device, &NetworkDeviceBase::deviceStatusChanged,
                this, &NetworkHandler::onDeviceStatusChanged);

        connect(device, &NetworkDeviceBase::enableChanged,
                this, &NetworkHandler::onEnableChanged);
        connect(device, &NetworkDeviceBase::connectionChanged,
                this, &NetworkHandler::onConnectionChanged);

        if (device->deviceType() == DeviceType::Wireless) {
            WirelessDevice *wirelessDevice = static_cast<WirelessDevice *>(device);
            connect(wirelessDevice, &WirelessDevice::hotspotEnableChanged,
                    this, &NetworkHandler::onHotspotEnabledChanged);
            connect(wirelessDevice, &WirelessDevice::networkAdded,
                    this, &NetworkHandler::onNetworkAdded);
            connect(wirelessDevice, &WirelessDevice::networkAdded,
                    this, &NetworkHandler::onNetworkRemoved);
        }

        QString deviceMessage = QString("find new Device %1 %2 %3 enabled:%4:  %5").arg(i)
                                .arg(device->deviceType() == DeviceType::Wired ? "wired" : "wireless")
                                .arg(device->deviceName()).arg(device->isEnabled() ? "true" : "false")
                                .arg(device->path());

        outputMessage(deviceMessage);
    }
    onDeviceInput();
}

void NetworkHandler::onDeviceRemoved(QList<NetworkDeviceBase *> devices)
{
    outputMessage("received DeviceRemoved signal:");
    for (NetworkDeviceBase *device : devices)
        cout << "remove Device:" << static_cast<const char *>(device->path().toLocal8Bit());
}

void NetworkHandler::onDeviceStatusChanged(const DeviceStatus &s) const
{
    /*UNetworkDeviceBase *device = static_cast<UNetworkDeviceBase *>(sender());
    outputMessage("received DeviceStatusChanged : devcice path:"
                  + device->path() + ", status:"
                  + QString("%1").arg(static_cast<int>(s)));
    */
}

void NetworkHandler::onEnableChanged(const bool enabled) const
{
    NetworkDeviceBase *device = static_cast<NetworkDeviceBase *>(sender());
    outputMessage("device enabled Changed:" + device->path()
                  + ", device Name:" + device->deviceName()
                  + ", enable status:" + QString::number(enabled));

    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (int i = 0; i < devices.size(); i++) {
        NetworkDeviceBase *device = devices[i];
        QString deviceMessage = QString("find new Device %1 %2 %3 enabled:%4:  %5").arg(i)
                                .arg(device->deviceType() == DeviceType::Wired ? "wired" : "wireless")
                                .arg(device->deviceName()).arg(device->isEnabled() ? "true" : "false")
                                .arg(device->path());
        outputMessage(deviceMessage);
    }
    onDeviceInput();
}

void NetworkHandler::onConnectionChanged()
{
    NetworkDeviceBase *device = static_cast<NetworkDeviceBase *>(sender());
    if (device->deviceType() == DeviceType::Wired) {
        WiredDevice *dev = static_cast<WiredDevice *>(device);
        QList<WiredConnection *> items = dev->items();
        for (WiredConnection *conn : items) {
            if (conn->connected()) {
                outputMessage("wired Connection id:" + conn->connection()->id());
                break;
            }
        }
    } else if (device->deviceType() == DeviceType::Wireless) {
        WirelessDevice *dev = static_cast<WirelessDevice *>(device);
        QList<AccessPoints *> aps = dev->accessPointItems();
        for (AccessPoints *ap : aps) {
            switch (ap->status()) {
            case ConnectionStatus::Activated: {
                outputMessage("ssid:" + ap->ssid() + " is Connected");
                break;
            }
            case ConnectionStatus::Activating: {
                outputMessage("ssid:" + ap->ssid() + " is Connecting");
                break;
            }
            case ConnectionStatus::Deactivating: {
                outputMessage("ssid:" + ap->ssid() + " is DisConnecting");
                break;
            }
            case ConnectionStatus::Deactivated: {
                outputMessage("ssid:" + ap->ssid() + " is DisConnected");
                break;
            }
            default: break;
            }
        }
    }

    QList<NetworkDetails *> networkDetails = NetworkController::instance()->networkDetails();
    for (NetworkDetails *detail : networkDetails) {
        outputMessage("networkDetails:" + detail->name());
        QList<QPair<QString, QString>> items = detail->items();
        for (int j = 0; j < items.size(); j++) {
            const QPair<QString, QString> &rowData = items[j];
            outputMessage("name:" + rowData.first + " value:" + rowData.second);
        }
    }
    onDeviceInput();
}

void NetworkHandler::onConnectivityChanged()
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    for (NetworkDeviceBase *device : devices) {
        bool ipIsValid = device->IPValid();
        if (ipIsValid)
            outputMessage("Ip Is Valid");
    }
}

void NetworkHandler::onVpnEnabled(const bool enabled)
{
    outputMessage("received vpnEnabled signal:" + QString(enabled ? "true" : "false"));
}

void NetworkHandler::onItemAdded(const QList<VPNItem *> vpns)
{
    outputMessage("received vpnAdd signal:");
    for (int i = 0; i < vpns.size(); i++) {
        VPNItem *item = vpns[i];
        outputMessage(QString("new vpnItems %1").arg(i));
        QStringList itemMsg;
        itemMsg << "id:" + item->connection()->id()
                << "path:" + item->connection()->path()
                << "ssid:" + item->connection()->ssid()
                << "uuid:" + item->connection()->uuid()
                << "hwAddress:" + item->connection()->hwAddress()
                << "cloneAddress:" + item->connection()->clonedAddress();
        outputMessage(itemMsg.join(","));
    }
}

void NetworkHandler::onItemRemoved(const QList<VPNItem *> vpns)
{
    outputMessage("received vpnRemove signal:");
    for (int i = 0; i < vpns.size(); i++) {
        VPNItem *item = vpns[i];
        outputMessage(QString("remove vpnItems %1").arg(i));
        QStringList itemMsg;
        itemMsg << "id:" + item->connection()->id()
                << "path:" + item->connection()->path()
                << "ssid:" + item->connection()->ssid()
                << "uuid:" + item->connection()->uuid()
                << "hwAddress:" + item->connection()->hwAddress()
                << "cloneAddress:" + item->connection()->clonedAddress();
        outputMessage(itemMsg.join(","));
    }
}

void NetworkHandler::onAutoProxyChanged(const QString &proxy)
{
    outputMessage("received AutoProxyChanged signal:" + proxy);
}

void NetworkHandler::onProxyChanged(const SysProxyConfig &proxy)
{
    QString type;
    switch (proxy.type) {
    case SysProxyType::Http:
        type = "http";
        break;
    case SysProxyType::Ftp:
        type = "ftp";
        break;
    case SysProxyType::Https:
        type = "https";
        break;
    case SysProxyType::Socks:
        type = "socks";
        break;
    }
    outputMessage("reveived proxyChanged signal:url " + proxy.url
                  + ",port " + QString::number(proxy.port) + ",type " + type);
}

void NetworkHandler::onProxyMethodChange(const ProxyMethod &method)
{
    QString m;
    switch (method) {
    case ProxyMethod::None:
        m = "none";
        break;
    case ProxyMethod::Auto:
        m = "auto";
        break;
    case ProxyMethod::Manual:
        m = "manual";
        break;
    default:
        m = "unknow";
        break;
    }
    outputMessage("received proxyMethodChange signal:" + m);
}

void NetworkHandler::onProxyIgnoreHostsChanged(const QString &host)
{
    outputMessage("received proxyIgnoreHostsChanged signal:" + host);
}

void NetworkHandler::onAppProxyChanged()
{
    AppProxyConfig config = NetworkController::instance()->proxyController()->appProxy();
    outputMessage("received appProxyChanged signal:ip " + config.ip
                  + ",port " + QString::number(config.port)
                  + ",username" + config.username
                  + ",password" + config.password);
}

void NetworkHandler::onHotspotChanged()
{
    initHotsPot();
}

void NetworkHandler::onStatusChanged()
{
    initDSL();
}

void NetworkHandler::onHotspotEnabledChanged(const bool &enabled)
{
    qInfo() << "hotspot:" << enabled;
}

void NetworkHandler::onDSLActiveChanged()
{
    DSLController *ctrl = NetworkController::instance()->dslController();
    QList<DSLItem *> items = ctrl->items();
    if (items.size() > 0) {
        DSLItem *item = items[0];
        ctrl->connectItem(item);
    }
}

void NetworkHandler::onVPNActiveChanged()
{
    VPNController *ctrl = NetworkController::instance()->vpnController();
    QList<VPNItem *> items = ctrl->items();
    if (items.size() > 0) {
        VPNItem *item = items[0];
        ctrl->connectItem(item);
    }
}

void NetworkHandler::onNetworkAdded(QList<AccessPoints *> newAps)
{
    outputMessage("new accessPoints");
    for (AccessPoints *ap : newAps)
        qInfo() << "ssid:" << ap->ssid() << ",path:" << ap->path();
}

void NetworkHandler::onNetworkRemoved(QList<AccessPoints *> rmAps)
{
    outputMessage("removed accessPoints");
    for (AccessPoints *ap : rmAps)
        qInfo() << "ssid:" << ap->ssid() << ",path:" << ap->path();
}
