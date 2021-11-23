#include "ipconfilctchecker.h"
#include "networkdevicebase.h"
#include "realize/netinterface.h"
#include "utils.h"

#include <QtMath>

#include <networkmanagerqt/manager.h>
#include <networkmanagerqt/device.h>

const static QString networkService = "com.deepin.daemon.Network";
const static QString networkPath = "/com/deepin/daemon/Network";

using namespace dde::network;

IPConfilctChecker::IPConfilctChecker(NetworkProcesser *networkProcesser, const bool ipChecked, NetworkInter *netInter, QObject *parent)
    : QObject(parent)
    , m_networkInter(netInter)
    , m_networkProcesser(networkProcesser)
    , m_ipChecked(ipChecked)
{
    if (!m_networkInter)
        m_networkInter = new NetworkInter(networkService, networkPath, QDBusConnection::sessionBus(), this);

    Q_ASSERT(m_networkProcesser);
    connect(m_networkInter, &NetworkInter::IPConflict, this, &IPConfilctChecker::onIPConfilct);                      // IP冲突发出的信号
    // 构造所有的设备冲突检测对象
    QList<NetworkDeviceBase *> devices = m_networkProcesser->devices();
    connect(m_networkProcesser, &NetworkProcesser::deviceAdded, this, [ this ] (QList<NetworkDeviceBase *> devices) {
        for (NetworkDeviceBase *device : devices) {
            DeviceIPChecker *ipChecker = new DeviceIPChecker(device, m_networkInter, m_ipChecked, this);
            connect(ipChecker, &DeviceIPChecker::conflictStatusChanged, this, &IPConfilctChecker::conflictStatusChanged);
            m_deviceCheckers << ipChecker;
        }
    });
}

IPConfilctChecker::~IPConfilctChecker()
{
}

bool IPConfilctChecker::ipConfilct(NetworkDeviceBase *device) const
{
    for (DeviceIPChecker *ipChecker : m_deviceCheckers) {
        if (ipChecker->device() == device)
            return ipChecker->ipConfilct();
    }

    return false;
}

void IPConfilctChecker::clearUnExistDevice()
{
    QList<NetworkDeviceBase *> devices = m_networkProcesser->devices();
    for (DeviceIPChecker *ipChecker : m_deviceCheckers) {
        if (!devices.contains(ipChecker->device())) {
            m_deviceCheckers.removeOne(ipChecker);
            delete ipChecker;
        }
    }
}

void IPConfilctChecker::onIPConfilct(const QString &ip, const QString &macAddress)
{
    PRINTMESSAGE(QString("received:ip %1, macaddress %2").arg(ip).arg(macAddress));
    // 此处通过调用后台获取IP地址，如果直接使用当前网络库中设备的IP地址，就有如下问题
    // 如果是在控制中心修改手动IP的话，从当前库中获取到的IP地址就不是最新的地址，因此此处需要从后台获取实时IP
    QDBusPendingCallWatcher *w = new QDBusPendingCallWatcher(m_networkInter->GetActiveConnectionInfo(), this);
    connect(w, &QDBusPendingCallWatcher::finished, w, &QDBusPendingCallWatcher::deleteLater);
    connect(w, &QDBusPendingCallWatcher::finished, [ = ](QDBusPendingCallWatcher * w) {
        QDBusPendingReply<QString> reply = *w;
        QString activeConnectionInfo = reply.value();
        handlerIpConflict(ip, macAddress, activeConnectionInfo);
    });
}

void IPConfilctChecker::handlerIpConflict(const QString &ip, const QString &macAddress, const QString &activeConnectionInfo)
{
    PRINTMESSAGE(QString("ip:%1, macAddress: %2").arg(ip).arg(macAddress));
    QMap<QString, NetworkDeviceBase *> deviceIps = parseDeviceIp(activeConnectionInfo);
    NetworkDeviceBase *conflictDevice = Q_NULLPTR;
    if (deviceIps.contains(ip))
        conflictDevice = deviceIps[ip];

    // 如果不是本机IP，不做任何处理
    if (!conflictDevice) {
        PRINTMESSAGE("can't find conflict Device");
        return;
    }

    // 先从列表中查找对应的设备检测对象
    DeviceIPChecker *ipChecker = Q_NULLPTR;
    for (DeviceIPChecker *checker : m_deviceCheckers) {
        if (checker->device() == conflictDevice) {
            ipChecker = checker;
            break;
        }
    }

    if (!ipChecker) {
        ipChecker = new DeviceIPChecker(conflictDevice, m_networkInter, m_ipChecked, this);
        connect(ipChecker, &DeviceIPChecker::conflictStatusChanged, this, &IPConfilctChecker::conflictStatusChanged);
        m_deviceCheckers << ipChecker;
    }

    ipChecker->setDeviceInfo(ip, macAddress);
    ipChecker->handlerIpConflict();
}

QMap<QString, NetworkDeviceBase *> IPConfilctChecker::parseDeviceIp(const QString &activeConnectionInfo)
{
    QMap<QString, NetworkDeviceBase *> tmpDevicePath;
    QList<NetworkDeviceBase *> devices = m_networkProcesser->devices();
    for (NetworkDeviceBase *device : devices)
        tmpDevicePath[device->path()] = device;

    QJsonParseError error;
    QJsonDocument json = QJsonDocument::fromJson(activeConnectionInfo.toUtf8(), &error);
    if (error.error != QJsonParseError::NoError)
        return QMap<QString, NetworkDeviceBase *>();

    QMap<QString, NetworkDeviceBase *> deviceIp;
    const QJsonArray &infoArray = json.array();
    for (const QJsonValue &infoValue : infoArray) {
        QJsonObject info = infoValue.toObject();
        if (!info.contains("Ip4"))
            continue;

        QJsonObject ipv4Object = info.value("Ip4").toObject();
        const QString ipv4 = ipv4Object.value("Address").toString();
        if (ipv4.isEmpty())
            continue;

        const QString devicePath = info.value("Device").toString();
        if (!tmpDevicePath.contains(devicePath))
            continue;

        NetworkDeviceBase *device = tmpDevicePath[devicePath];
        deviceIp[ipv4] = device;
    }

    return deviceIp;
}

/**
 * @brief 构造函数
 * @param device 对应的设备
 * @param netInter
 * @param parent
 */
DeviceIPChecker::DeviceIPChecker(NetworkDeviceBase *device, NetworkInter *netInter, const bool &ipCheck, QObject *parent)
    : QObject(parent)
    , m_device(device)
    , m_networkInter(netInter)
    , m_conflictCount(0)
    , m_clearCount(0)
    , m_ipConflicted(false)
    , m_ipCheck(ipCheck)
{
    auto requestConflictCheck = [ this ] () {
        // 当设备的IP地址发生变化的时候，请求IP冲突， 只有在上一次请求和该次请求发生的时间大于2秒，才发出信号
        m_ipV4 = m_device->ipv4();
        if (!m_ipV4.isEmpty()) {
            PRINTMESSAGE(QString("request Device:%1, IP: %2").arg(m_device->deviceName()).arg(m_ipV4));
            m_changeIpv4s << m_ipV4;
            QTimer::singleShot(800, this, [ this ] {
                if (m_changeIpv4s.size() > 0) {
                    requestIPConflictCheck(m_changeIpv4s[m_changeIpv4s.size() - 1]);
                    m_changeIpv4s.clear();
                }
            });
        }
    };
    connect(device, &NetworkDeviceBase::ipV4Changed, this, requestConflictCheck);
    connect(device, &NetworkDeviceBase::enableChanged, this, requestConflictCheck);
    connect(device, &NetworkDeviceBase::activeConnectionChanged, this, requestConflictCheck);
}

DeviceIPChecker::~DeviceIPChecker()
{
}

NetworkDeviceBase *DeviceIPChecker::device()
{
    return m_device;
}

void DeviceIPChecker::setDeviceInfo(const QString &ipv4, const QString &macAddress)
{
    m_ipV4 = ipv4;
    m_macAddress = macAddress;
}

bool DeviceIPChecker::ipConfilct()
{
    return m_ipConflicted;
}

void DeviceIPChecker::handlerIpConflict()
{
    PRINTMESSAGE(QString("device: %1 ip: %2 mac address:%3").arg(m_device->deviceName()).arg(m_ipV4).arg(m_macAddress));
    if (m_macAddress.isEmpty()) {
        m_conflictCount = 0;
        // 如果MAC地址为空，则表示IP冲突已经解决，则让每个网卡恢复之前的状态
        if (m_clearCount < 3) {
            requestIPConflictCheck(m_ipV4, 800);
        } else {
            // 拿到最后一次设备冲突的状态
            bool lastConfilctStatus = m_ipConflicted;
            // 确认1次后解除冲突
            m_ipConflicted = false;

            // IP冲突状态发生变化的时候才会发送该信号
            if (lastConfilctStatus)
                Q_EMIT conflictStatusChanged(m_device, false);
            // 如果网络没有冲突，则30分钟定期检测一次
            if (m_clearCount < 3) {
#define SCANTIME (30 * 60 * 1000)
                requestIPConflictCheck(m_ipV4, SCANTIME);
            }
        }
        m_clearCount++;
    } else {
        m_clearCount = 0;
        PRINTMESSAGE(QString("find confilct device:%1, confilctCount:%2, conflictIP: %3").arg(m_device->deviceName()).arg(m_conflictCount)).arg(m_ipV4);

        // 如果少于两次，则继续确认
        if (m_conflictCount < 1) {
            PRINTMESSAGE(QString("start confirm %1 times").arg(m_conflictCount));
            requestIPConflictCheck(m_ipV4, 500);
        } else {
            // 如果大于3次，则认为当前IP冲突了
            PRINTMESSAGE(QString("ip Conflicted: %1, device: %2").arg(m_ipV4).arg(m_device->deviceName()));
            // 拿到最后一次设备冲突的状态
            bool lastConflictStatus = m_ipConflicted;
            m_ipConflicted = true;

            // 最后一次IP不冲突，本次IP冲突的时候才发送设备状态变化的信号
            if (!lastConflictStatus)
                Q_EMIT conflictStatusChanged(m_device, true);

            requestIPConflictCheck(m_ipV4, 5000);
        }
        m_conflictCount++;
    }
}

QString DeviceIPChecker::ipV4()
{
    return m_ipV4;
}

void DeviceIPChecker::requestIPConflictCheck(const QString &ipv4, const int interval)
{
    // 只有主动检测IP，才会定期发送IP冲突检测的功能，因为当前的库是放到了每个独立的应用里面，只需要发送一次检测就行，如果同时都在检测，会一直不停的检测引起问题
    if (!m_ipCheck)
        return;

    if (interval < 0) {
        m_networkInter->RequestIPConflictCheck(ipv4, "");
    } else {
        QTimer::singleShot(interval, this, [ this, ipv4 ] {
            m_networkInter->RequestIPConflictCheck(ipv4, "");
        });
    }
}
