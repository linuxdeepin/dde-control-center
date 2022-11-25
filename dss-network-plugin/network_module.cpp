// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "network_module.h"
#include "networkpluginhelper.h"
#include "networkdialog.h"
#include "secretagent.h"
#include "trayicon.h"
#include "notificationmanager.h"
#include "dockpopupwindow.h"
#include "networkdialog/thememanager.h"

#include <QWidget>
#include <QTime>

#include <DArrowRectangle>

#include <networkcontroller.h>
#include <networkdevicebase.h>

#include <com_deepin_daemon_accounts_user.h>

#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WiredDevice>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/Connection>
#include <NetworkManagerQt/WiredSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>

#define NETWORK_KEY "network-item-key"

using namespace NetworkManager;
NETWORKPLUGIN_USE_NAMESPACE

namespace dss {
namespace module {
// 弹窗管理类,多屏会有多个弹窗
class PopupAppletManager : public QObject
{
public:
    explicit PopupAppletManager(NetworkDialog *networkDialog, QObject *parent = nullptr)
        : QObject(parent)
        , m_networkDialog(networkDialog)
        , m_popupShown(false)
    {
        m_networkDialog->setCloseOnClear(false);
    }
    ~PopupAppletManager() {}
    inline bool popupShown()
    {
        return m_popupShown;
    }
    void showPopupApplet()
    {
        if (!m_popupShown) {
            qApp->installEventFilter(this);
            m_popupShown = true;
        }
        updatePopup();
    }
    void hidePopup()
    {
        m_popupShown = false;
        qApp->removeEventFilter(this);
        for (auto &&it : m_applets) {
            if (it.second && it.second->isVisible()) {
                m_networkDialog->clear();
                it.second->setVisible(false);
            }
        }
    }
    void updatePopup()
    {
        if (!m_popupShown)
            return;
        for (auto &&it : m_applets) {
            if (it.first->isVisible()) {
                QPoint point = it.first->mapToGlobal(QPoint(it.first->width() / 2, 0));
                QDesktopWidget *desktopWidget = qApp->desktop();
                if (desktopWidget->screenNumber(point) != desktopWidget->screenNumber(QCursor::pos()))
                    continue;

                if (it.second.isNull()) {
                    QWidget *pWidget = it.first->window();
                    it.second = new DockPopupWindow(pWidget);
                }

                point = it.second->parentWidget()->mapFromGlobal(point);
                QWidget * panel = m_networkDialog->panel();
                QPalette palette = panel->palette();
                palette.setColor(QPalette::Normal, QPalette::BrightText, QColor(255, 255, 255)); // 文本颜色
                palette.setColor(QPalette::Normal, QPalette::Window, QColor(235, 235, 235, 13)); // 背景颜色
                palette.setColor(DPalette::Normal, QPalette::ButtonText, QColor(0, 0, 0, 76));
                panel->setPalette(palette);

                it.second->setContent(panel);
                it.second->show(point);
            } else if (!it.second.isNull()) {
                it.second->setVisible(false);
            }
        }
    }

    void addTrayIcon(TrayIcon *trayIcon)
    {
        if (!trayIcon)
            return;
        for (auto &&it : m_applets) {
            if (it.first == trayIcon) {
                trayIcon = nullptr;
                break;
            }
        }
        if (trayIcon) {
            m_applets.append({ QPointer<TrayIcon>(trayIcon), QPointer<DockPopupWindow>(nullptr) });
        }
    }

protected:
    // qApp的事件较多，固单独一个类监听其事件
    bool eventFilter(QObject *watched, QEvent *e) override
    {
        switch (e->type()) {
        case QEvent::MouseButtonPress: {
            if (!m_popupShown)
                break;

            QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(e);
            for (auto &&it : m_applets) {
                if (!it.second || !it.second->isVisible())
                    continue;

                QRect rect = it.second->rect();
                rect.moveTo(it.second->mapToGlobal(rect.topLeft()));
                if (!rect.contains(mouseEvent->globalPos())) {
                    hidePopup();
                }
                break;
            }
        } break;
        default:
            break;
        }
        return QObject::eventFilter(watched, e);
    }

public:
    QList<QPair<QPointer<NETWORKPLUGIN_NAMESPACE::TrayIcon>, QPointer<DockPopupWindow>>> m_applets;
    NetworkDialog *m_networkDialog;
    bool m_popupShown;
};

NetworkModule::NetworkModule(QObject *parent)
    : QObject(parent)
    , m_lastState(NetworkManager::Device::State::UnknownState)
{
    QDBusConnection::sessionBus().connect("com.deepin.dde.lockFront", "/com/deepin/dde/lockFront", "com.deepin.dde.lockFront", "Visible", this, SLOT(updateLockScreenStatus(bool)));
    m_isLockModel = (-1 == qAppName().indexOf("greeter"));
    if (!m_isLockModel) {
        dde::network::NetworkController::setServiceType(dde::network::ServiceLoadType::LoadFromManager);
    }

    m_networkDialog = new NetworkDialog(this);
    m_popupAppletManager = new PopupAppletManager(m_networkDialog, this);
    m_networkDialog->setServerName("dde-network-dialog" + QString::number(getuid()) + "lock");
    m_networkHelper = new NetworkPluginHelper(m_networkDialog, this);
    connect(m_networkDialog, &NetworkDialog::requestShow, m_popupAppletManager, &PopupAppletManager::showPopupApplet);

    installTranslator(QLocale::system().name());
    ThemeManager::instance()->setThemeType(m_isLockModel ? ThemeManager::LockType : ThemeManager::GreeterType);
    if (!m_isLockModel) {
        QDBusMessage lock = QDBusMessage::createMethodCall("com.deepin.dde.LockService", "/com/deepin/dde/LockService", "com.deepin.dde.LockService", "CurrentUser");
        QDBusConnection::systemBus().callWithCallback(lock, this, SLOT(onUserChanged(QString)));
        QDBusConnection::systemBus().connect("com.deepin.dde.LockService", "/com/deepin/dde/LockService", "com.deepin.dde.LockService", "UserChanged", this, SLOT(onUserChanged(QString)));

        connect(m_networkHelper, &NetworkPluginHelper::addDevice, this, &NetworkModule::onAddDevice);
        for (dde::network::NetworkDeviceBase *device : dde::network::NetworkController::instance()->devices()) {
            onAddDevice(device->path());
        }
        m_secretAgent = new NETWORKPLUGIN_NAMESPACE::SecretAgent(true, this);
        connect(m_networkDialog, &NetworkDialog::inputPassword, m_secretAgent, &NETWORKPLUGIN_NAMESPACE::SecretAgent::onInputPassword);
        connect(m_secretAgent, &NETWORKPLUGIN_NAMESPACE::SecretAgent::requestPassword, m_networkDialog, &NetworkDialog::setConnectWireless);
    }
}

QWidget *NetworkModule::content()
{
    int msec = QTime::currentTime().msecsSinceStartOfDay();
    if (!m_popupAppletManager->popupShown() && abs(msec - m_clickTime) > 200) {
        m_clickTime = msec;
        m_popupAppletManager->showPopupApplet();
    }
    return nullptr;
}

QWidget *NetworkModule::itemTipsWidget() const
{
    if (m_popupAppletManager->popupShown())
        return nullptr;
    QWidget *itemTips = m_networkHelper->itemTips();
    if (itemTips) {
        QPalette palette = itemTips->palette();
        palette.setColor(QPalette::BrightText, Qt::black);
        itemTips->setPalette(palette);
    }
    return itemTips;
}

const QString NetworkModule::itemContextMenu() const
{
    return m_networkHelper->contextMenu(false);
}

void NetworkModule::invokedMenuItem(const QString &menuId, const bool checked) const
{
    Q_UNUSED(checked);
    m_networkHelper->invokeMenuItem(menuId);
}

bool NetworkModule::eventFilter(QObject *watched, QEvent *e)
{
    switch (e->type()) {
    case QEvent::ParentChange: {
        TrayIcon *trayIcon = qobject_cast<TrayIcon *>(watched);
        // ParentChange可能会进来多次，只需要处理父对象是FloatingButton的情况
        // FIXME 这种写法增加了与dde-session-shell的耦合性
        if (!trayIcon || !trayIcon->parent() || (trayIcon->parent()->metaObject()->className() != QString("FloatingButton")))
            break;
        if (!m_isLockModel)
            NotificationManager::InstallEventFilter(trayIcon);
        trayIcon->parent()->installEventFilter(this);
        m_popupAppletManager->addTrayIcon(trayIcon);
    } break;
    case QEvent::Move:
    case QEvent::Show:
    case QEvent::Hide:
        m_popupAppletManager->updatePopup();
        break;
    default:
        break;
    }
    return QObject::eventFilter(watched, e);
}

void NetworkModule::updateLockScreenStatus(bool visible)
{
    m_isLockModel = true;
    m_isLockScreen = visible;
    m_popupAppletManager->hidePopup();
}

void NetworkModule::onAddDevice(const QString &devicePath)
{
    if (m_isLockModel) {
        return;
    }
    // 登录界面才监听该信号，用于横幅、密码错误处理
    if (!m_devicePaths.contains(devicePath)) {
        Device::Ptr device(new Device(devicePath));
        Device *nmDevice = nullptr;
        if (device->type() == Device::Wifi) {
            NetworkManager::WirelessDevice *wDevice = new NetworkManager::WirelessDevice(devicePath, this);
            nmDevice = wDevice;
            connect(wDevice, &NetworkManager::WirelessDevice::activeAccessPointChanged, this, [this](const QString &ap) {
                m_lastActiveWirelessDevicePath = static_cast<NetworkManager::WirelessDevice *>(sender())->uni() + NetworkManager::AccessPoint(ap).ssid();
            });
        } else if (device->type() == Device::Ethernet) {
            NetworkManager::WiredDevice *wDevice = new NetworkManager::WiredDevice(devicePath, this);
            nmDevice = wDevice;
            addFirstConnection(wDevice);
            connect(wDevice, &NetworkManager::WiredDevice::availableConnectionAppeared, this, [ this ] (const QString &) {
                NetworkManager::WiredDevice *device = qobject_cast<NetworkManager::WiredDevice *>(sender());
                addFirstConnection(device);
            });
        }
        if (nmDevice) {
            connect(nmDevice, &NetworkManager::Device::stateChanged, this, &NetworkModule::onDeviceStatusChanged);
            m_devicePaths.insert(devicePath);
        }
    }
}

void NetworkModule::onUserChanged(QString json)
{
    QJsonDocument doc = QJsonDocument::fromJson(json.toUtf8());
    if (!doc.isObject())
        return;
    int uid = doc.object().value("Uid").toInt();
    com::deepin::daemon::accounts::User user("com.deepin.daemon.Accounts", QString("/com/deepin/daemon/Accounts/User%1").arg(uid), QDBusConnection::systemBus());
    installTranslator(user.locale().split(".").first());
}

void NetworkModule::installTranslator(QString locale)
{
    static QTranslator translator;
    static QString localTmp;
    if (localTmp == locale) {
        return;
    }
    localTmp = locale;
    QApplication::removeTranslator(&translator);
    translator.load(QString("/usr/share/dss-network-plugin/translations/dss-network-plugin_%1.qm").arg(locale));
    QApplication::installTranslator(&translator);
    dde::network::NetworkController::instance()->retranslate();
    m_networkHelper->updateTooltips();
}

const QString NetworkModule::connectionMatchName() const
{
    NetworkManager::Connection::List connList = listConnections();
    QStringList connNameList;
    int connSuffixNum = 1;

    for (NetworkManager::Connection::Ptr conn : connList) {
        if (conn->settings()->connectionType() == ConnectionSettings::ConnectionType::Wired)
            connNameList.append(conn->name());
    }

    QString matchConnName = tr("Wired Connection");
    if (!connNameList.contains(matchConnName))
        return matchConnName;

    matchConnName = QString(tr("Wired Connection")) + QString(" %1");
    for (int i = 1; i <= connNameList.size(); ++i) {
        if (!connNameList.contains(matchConnName.arg(i))) {
            connSuffixNum = i;
            break;
        }
        if (i == connNameList.size())
            connSuffixNum = i + 1;
    }

    return matchConnName.arg(connSuffixNum);
}

bool NetworkModule::hasConnection(NetworkManager::WiredDevice *nmDevice, NetworkManager::Connection::List &unSaveDevices)
{
    bool connIsEmpty = false;
    // 获取所有的连接列表,遍历连接列表,如果当前连接中的MAC地址不为空且不等于当前网卡的MAC地址，则认为它不是当前网卡的连接
    // 在获取所有连接之前，需要手动调用一下当前设备的availableConnections接口，否则获取到的连接列表就不是最新的(具体原因待查)。
    nmDevice->availableConnections();
    NetworkManager::Connection::List connList = listConnections();
    for (NetworkManager::Connection::Ptr conn : connList) {
        WiredSetting::Ptr settings = conn->settings()->setting(Setting::Wired).staticCast<WiredSetting>();
        // 如果当前连接的MAC地址不为空且连接的MAC地址不等于当前设备的MAC地址，则认为不是当前的连接，跳过
        if (settings.isNull() || (!settings->macAddress().isEmpty() && nmDevice->hardwareAddress().compare(settings->macAddress().toHex(':'), Qt::CaseInsensitive) != 0))
            continue;

        // 将未保存的连接放入到列表中，供外面调用删除
        if (conn->isUnsaved()) {
            unSaveDevices << conn;
            continue;
        }

        connIsEmpty = true;
    }

    return connIsEmpty;
}

void NetworkModule::addFirstConnection(NetworkManager::WiredDevice *nmDevice)
{
    // 先查找当前的设备下是否存在有线连接，如果不存在，则直接新建一个，因为按照要求是至少要有一个有线连接
    NetworkManager::Connection::List unSaveConnections;
    bool findConnection = hasConnection(nmDevice, unSaveConnections);
    // 按照需求，需要将未保存的连接删除
    bool isRemoved = !unSaveConnections.isEmpty();
    for (NetworkManager::Connection::Ptr conn : unSaveConnections)
        conn->remove();

    static bool connectionCreated = false;
    // 只要有一个新增的连接,就不继续新增连接了,因为这个新增的连接是所有网卡共享的
    if (connectionCreated)
        return;

    connectionCreated = true;
    auto autoCreateConnection = [ & ]() {
        // 如果发现当前的连接的数量为空,则自动创建以当前语言为基础的连接
        ConnectionSettings::Ptr conn(new ConnectionSettings);
        conn->setId(connectionMatchName());
        conn->setUuid("");
        NetworkManager::addConnection(conn->toMap());
    };

    if (!findConnection) {
        if (isRemoved) {
            // 如果有删除的连接，则等待1秒后重新创建
            QTimer::singleShot(1000, this, [ = ] {
                autoCreateConnection();
            });
        } else {
            autoCreateConnection();
        }
    }
}

bool NetworkModule::needPopupNetworkDialog() const
{
    // 如果上一次没有保存密码信息则不弹窗
    if (m_lastConnectionUuid.isEmpty())
        return false;

    NetworkManager::Connection::Ptr connection = NetworkManager::findConnectionByUuid(m_lastConnectionUuid);
    if (connection.isNull())
        return false;

    // 如果当前连接的密码是按照用户保存的，就不弹出来
    WirelessSecuritySetting::Ptr securitySetting = connection->settings()->setting(Setting::SettingType::WirelessSecurity).staticCast<WirelessSecuritySetting>();
    if (securitySetting.isNull())
        return true;

    NetworkManager::Setting::SecretFlags passwordFlags = securitySetting->pskFlags();
    return (passwordFlags.testFlag(Setting::None));
}

void NetworkModule::onDeviceStatusChanged(NetworkManager::Device::State newstate, NetworkManager::Device::State oldstate, NetworkManager::Device::StateChangeReason reason)
{
    if (m_isLockModel) {
        return;
    }
    NetworkManager::Device *device = static_cast<NetworkManager::Device *>(sender());
    NetworkManager::ActiveConnection::Ptr conn = device->activeConnection();
    if (!conn.isNull()) {
        m_lastConnection = conn->id();
        m_lastConnectionUuid = conn->uuid();
        m_lastState = newstate;
    } else if (m_lastState != oldstate || m_lastConnection.isEmpty()) {
        m_lastConnection.clear();
        m_lastConnectionUuid.clear();
        return;
    }
    switch (newstate) {
    case Device::State::Preparing: { // 正在连接
        if (oldstate == Device::State::Disconnected) {
            switch (device->type()) {
            case Device::Type::Ethernet:
                NotificationManager::NetworkNotify(NotificationManager::WiredConnecting, m_lastConnection);
                break;
            case Device::Type::Wifi:
                NotificationManager::NetworkNotify(NotificationManager::WirelessConnecting, m_lastConnection);
                break;
            default:
                break;
            }
        }
    } break;
    case Device::State::Activated: { // 连接成功
        switch (device->type()) {
        case Device::Type::Ethernet:
            NotificationManager::NetworkNotify(NotificationManager::WiredConnected, m_lastConnection);
            break;
        case Device::Type::Wifi:
            NotificationManager::NetworkNotify(NotificationManager::WirelessConnected, m_lastConnection);
            break;
        default:
            break;
        }
    } break;
    case Device::State::Failed:
    case Device::State::Disconnected:
    case Device::State::NeedAuth:
    case Device::State::Unmanaged:
    case Device::State::Unavailable: {
        if (reason == Device::StateChangeReason::DeviceRemovedReason) {
            return;
        }

        // ignore if device's old state is not available
        if (oldstate <= Device::State::Unavailable) {
            qDebug("no notify, old state is not available");
            return;
        }

        // ignore invalid reasons
        if (reason == Device::StateChangeReason::UnknownReason) {
            qDebug("no notify, device state reason invalid");
            return;
        }

        switch (reason) {
        case Device::StateChangeReason::UserRequestedReason:
            if (newstate == Device::State::Disconnected) {
                switch (device->type()) {
                case Device::Type::Ethernet:
                    NotificationManager::NetworkNotify(NotificationManager::WiredDisconnected, m_lastConnection);
                    break;
                case Device::Type::Wifi:
                    NotificationManager::NetworkNotify(NotificationManager::WirelessDisconnected, m_lastConnection);
                    break;
                default:
                    break;
                }
            }
            break;
        case Device::StateChangeReason::ConfigUnavailableReason:
        case Device::StateChangeReason::AuthSupplicantTimeoutReason: // 超时
            switch (device->type()) {
            case Device::Type::Ethernet:
                NotificationManager::NetworkNotify(NotificationManager::WiredUnableConnect, m_lastConnection);
                break;
            case Device::Type::Wifi:
                NotificationManager::NetworkNotify(NotificationManager::WirelessUnableConnect, m_lastConnection);
                break;
            default:
                break;
            }
            break;
        case Device::StateChangeReason::AuthSupplicantDisconnectReason:
            if (oldstate == Device::State::ConfiguringHardware && newstate == Device::State::NeedAuth) {
                switch (device->type()) {
                case Device::Type::Ethernet:
                    NotificationManager::NetworkNotify(NotificationManager::WiredConnectionFailed, m_lastConnection);
                    break;
                case Device::Type::Wifi:
                    NotificationManager::NetworkNotify(NotificationManager::WirelessConnectionFailed, m_lastConnection);
                    if (needPopupNetworkDialog()) {
                        m_networkDialog->setConnectWireless(device->uni(), m_lastConnection);
                    }
                    break;
                default:
                    break;
                }
            }
            break;
        case Device::StateChangeReason::CarrierReason:
            if (device->type() == Device::Ethernet) {
                qDebug("unplugged device is ethernet");
                NotificationManager::NetworkNotify(NotificationManager::WiredDisconnected, m_lastConnection);
            }
            break;
        case Device::StateChangeReason::NoSecretsReason:
            NotificationManager::NetworkNotify(NotificationManager::NoSecrets, m_lastConnection);
            if (needPopupNetworkDialog()) {
                // 不是仅当前用户，就弹窗
                m_networkDialog->setConnectWireless(device->uni(), m_lastConnection);
            }
            break;
        case Device::StateChangeReason::SsidNotFound:
            NotificationManager::NetworkNotify(NotificationManager::SsidNotFound, m_lastConnection);
            break;
        default:
            break;
        }
    } break;
    default:
        break;
    }
}

NetworkPlugin::NetworkPlugin(QObject *parent)
    : QObject(parent)
    , m_network(nullptr)
{
    setObjectName(QStringLiteral(NETWORK_KEY));
}

void NetworkPlugin::init()
{
    initUI();
}

QWidget *NetworkPlugin::content()
{
    return m_network->content();
}

QWidget *NetworkPlugin::itemWidget() const
{
    TrayIcon *trayIcon = new TrayIcon(m_network->networkHelper());
    trayIcon->setGreeterStyle(true);
    trayIcon->installEventFilter(m_network);
    return trayIcon;
}

QWidget *NetworkPlugin::itemTipsWidget() const
{
    return m_network->itemTipsWidget();
}

const QString NetworkPlugin::itemContextMenu() const
{
    return m_network->itemContextMenu();
}

void NetworkPlugin::invokedMenuItem(const QString &menuId, const bool checked) const
{
    m_network->invokedMenuItem(menuId, checked);
}

void NetworkPlugin::initUI()
{
    if (m_network) {
        return;
    }

    m_network = new NetworkModule(this);
}

} // namespace module
} // namespace dss
