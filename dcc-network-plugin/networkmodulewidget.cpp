/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
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

#include "networkmodulewidget.h"
#include "pppoepage.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/multiselectlistview.h"

#include <DStyleOption>

#include <QDebug>
#include <QPointer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QGSettings>

#include <interface/frameproxyinterface.h>

#include <wireddevice.h>
#include <wirelessdevice.h>
#include <networkdevicebase.h>
#include <networkcontroller.h>
#include <proxycontroller.h>
#include <networkconst.h>
#include <hotspotcontroller.h>

#include <widgets/contentwidget.h>

#include <org_freedesktop_notifications.h>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE;
using namespace dde::network;
using Notifications = org::freedesktop::Notifications;

static const int SectionRole = Dtk::UserRole + 1;
static const int DeviceRole = Dtk::UserRole + 2;
static const int SearchPath = Dtk::UserRole + 3;
static const int DumyStatusRole = Dtk::UserRole + 4;

NetworkModuleWidget::NetworkModuleWidget(QWidget *parent)
    : QWidget(parent)
    , m_lvnmpages(new dcc::widgets::MultiSelectListView(this))
    , m_modelpages(new QStandardItemModel(this))
    , m_nmConnectionEditorProcess(nullptr)
    , m_settings(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
    , m_isFirstEnter(true)
    , m_switchIndex(true)
{
    setObjectName("Network");
    m_lvnmpages->setAccessibleName("List_networkmenulist");
    m_lvnmpages->setFrameShape(QFrame::NoFrame);
    m_lvnmpages->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvnmpages->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lvnmpages->setModel(m_modelpages);
    m_lvnmpages->setViewportMargins(ScrollAreaMargins);
    m_lvnmpages->setIconSize(ListViweIconSize);

    m_centralLayout = new QVBoxLayout();
    setMinimumWidth(250);
    m_centralLayout->setMargin(0);
    setLayout(m_centralLayout);

    //~ contents_path /network/DSL
    DStandardItem *pppIt = new DStandardItem(tr("DSL"));
    pppIt->setData(QVariant::fromValue(PageType::DSLPage), SectionRole);
    pppIt->setIcon(QIcon::fromTheme("dcc_dsl"));
    m_modelpages->appendRow(pppIt);
    GSettingWatcher::instance()->bind("networkDsl", m_lvnmpages, pppIt);

    //~ contents_path /network/VPN
    DStandardItem *vpnit = new DStandardItem(tr("VPN"));
    vpnit->setData(QVariant::fromValue(PageType::VPNPage), SectionRole);
    vpnit->setIcon(QIcon::fromTheme("dcc_vpn"));
    m_modelpages->appendRow(vpnit);
    GSettingWatcher::instance()->bind("networkVpn", m_lvnmpages, vpnit);

    //~ contents_path /network/System Proxy
    DStandardItem *prxyit = new DStandardItem(tr("System Proxy"));
    prxyit->setData(QVariant::fromValue(PageType::SysProxyPage), SectionRole);
    prxyit->setIcon(QIcon::fromTheme("dcc_system_agent"));
    m_modelpages->appendRow(prxyit);
    GSettingWatcher::instance()->bind("systemProxy", m_lvnmpages, prxyit);

    //~ contents_path /network/Application Proxy
    DStandardItem *aprxit = new DStandardItem(tr("Application Proxy"));
    aprxit->setData(QVariant::fromValue(PageType::AppProxyPage), SectionRole);
    aprxit->setIcon(QIcon::fromTheme("dcc_app_proxy"));
    m_modelpages->appendRow(aprxit);
    GSettingWatcher::instance()->bind("applicationProxy", m_lvnmpages, aprxit);

    //~ contents_path /network/Network Details
    DStandardItem *infoit = new DStandardItem(tr("Network Details"));
    infoit->setData(QVariant::fromValue(PageType::NetworkInfoPage), SectionRole);
    infoit->setIcon(QIcon::fromTheme("dcc_network"));
    m_modelpages->appendRow(infoit);
    GSettingWatcher::instance()->bind("networkDetails", m_lvnmpages, infoit);

    m_centralLayout->addWidget(m_lvnmpages);
    if (IsServerSystem)
        handleNMEditor();

    connect(m_lvnmpages->selectionModel(), &QItemSelectionModel::currentChanged, this, [ this ] (const QModelIndex &index) {
        selectListIndex(index);
        if (!m_lastDevicePath.isEmpty())
            m_lastDevicePath.clear();
    });

    NetworkController *pNetworkController = NetworkController::instance();
    connect(pNetworkController, &NetworkController::activeConnectionChange, this, &NetworkModuleWidget::onDeviceStatusChanged);
    connect(pNetworkController, &NetworkController::deviceRemoved, this, &NetworkModuleWidget::onDeviceChanged);
    connect(pNetworkController, &NetworkController::deviceAdded, this, [ = ] (QList<NetworkDeviceBase *> devices) {
        initIpConflictInfo(devices);
        onDeviceChanged();
        if (m_isFirstEnter) {
            showDefaultWidget();
            m_isFirstEnter = false;
        }
    });

    ProxyController *proxyController = pNetworkController->proxyController();
    connect(proxyController, &ProxyController::proxyMethodChanged, this, &NetworkModuleWidget::onProxyMethodChanged);

    initIpConflictInfo(pNetworkController->devices());
    onDeviceChanged();
    if (NetworkController::instance()->devices().size() > 0)
        m_isFirstEnter = false;
}

NetworkModuleWidget::~NetworkModuleWidget()
{
    if (m_nmConnectionEditorProcess) {
        m_nmConnectionEditorProcess->close();
        m_nmConnectionEditorProcess->deleteLater();
        m_nmConnectionEditorProcess = nullptr;
    }
}

void NetworkModuleWidget::initIpConflictInfo(const QList<NetworkDeviceBase *> &devices)
{
    for (NetworkDeviceBase *device : devices) {
        connect(device, &NetworkDeviceBase::deviceStatusChanged, this, [](const DeviceStatus &deviceStatus) {
            if (deviceStatus == DeviceStatus::IpConfilct) {
                Notifications notifications("org.freedesktop.Notifications", "/org/freedesktop/Notifications", QDBusConnection::sessionBus());
                notifications.Notify("dde-control-center", static_cast<uint>(QDateTime::currentMSecsSinceEpoch()), "preferences-system", tr("Network"), tr("IP conflict"), QStringList(), QVariantMap(), 3000);
            }
        });
    }
}

void NetworkModuleWidget::selectListIndex(const QModelIndex &idx)
{
    if (!m_switchIndex)
        return;

    const QString searchPath = idx.data(SearchPath).toString();
    m_modelpages->itemFromIndex(idx)->setData("", SearchPath);

    PageType type = idx.data(SectionRole).value<PageType>();
    m_lvnmpages->setCurrentIndex(idx);
    switch (type) {
    case PageType::DSLPage:
        Q_EMIT requestShowPppPage(searchPath);
        break;
    case PageType::VPNPage:
        Q_EMIT requestShowVpnPage(searchPath);
        break;
    case PageType::SysProxyPage:
        Q_EMIT requestShowProxyPage();
        break;
    case PageType::AppProxyPage:
        Q_EMIT requestShowChainsPage();
        break;
    case PageType::HotspotPage:
        Q_EMIT requestHotspotPage();
        break;
    case PageType::NetworkInfoPage:
        Q_EMIT requestShowInfomation();
        break;
    case PageType::WiredPage:
    case PageType::WirelessPage:
        Q_EMIT requestShowDeviceDetail(idx.data(DeviceRole).value<NetworkDeviceBase *>(), searchPath);
        break;
    default:
        break;
    }

    m_lvnmpages->resetStatus(idx);
}

void NetworkModuleWidget::onProxyMethodChanged(const ProxyMethod &method)
{
    if (method == ProxyMethod::Init) return;
    QPointer<DViewItemAction> action(new DViewItemAction(Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter));
    if (action.isNull()) return;
    //遍历获取系统代理项,设置状态
    for (int i = 0; i < m_modelpages->rowCount(); i++) {
        DStandardItem *item = dynamic_cast<DStandardItem *>(m_modelpages->item(i));
        if (!item)
            continue;

        if (item->data(SectionRole).value<PageType>() == PageType::SysProxyPage) {
            item->setActionList(Qt::Edge::RightEdge, {action});
            if (method == ProxyMethod::None)
                action->setText(tr("Disabled"));
            else if (method == ProxyMethod::Manual)
                action->setText(tr("Manual"));
            else if (method == ProxyMethod::Auto)
                action->setText(tr("Auto"));
            else
                action->setText(tr("Disabled"));

            break;
        }
    }
}

bool NetworkModuleWidget::handleNMEditor()
{
    QProcess *process = new QProcess(this);
    QPushButton *nmConnEditBtn = new QPushButton(tr("Configure by Network Manager"));
    m_centralLayout->addWidget(nmConnEditBtn);
    nmConnEditBtn->hide();
    process->start("which nm-connection-editor");

    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [ = ] {
        QString networkManageOutput = process->readAll();
        if (!networkManageOutput.isEmpty()) {
            nmConnEditBtn->show();
            connect(nmConnEditBtn, &QPushButton::clicked, this, [ = ] {
                if (!m_nmConnectionEditorProcess) {
                    m_nmConnectionEditorProcess = new QProcess(this);
                }
                m_nmConnectionEditorProcess->start("nm-connection-editor");
            });
        }
        process->deleteLater();
    });

    return true;
}

void NetworkModuleWidget::showDefaultWidget()
{
    for (int i = 0; i < m_modelpages->rowCount(); i++) {
        if (!m_lvnmpages->isRowHidden(i)) {
            setCurrentIndex(i);
            break;
        }
    }
}

void NetworkModuleWidget::setCurrentIndex(const int settingIndex)
{
    // 设置网络列表当前索引
    QModelIndex index = m_modelpages->index(settingIndex, 0);
    m_lvnmpages->setCurrentIndex(index);
    selectListIndex(index);
}

void NetworkModuleWidget::setIndexFromPath(const QString &path)
{
    for (int i = 0; i < m_modelpages->rowCount(); ++i) {
        QString indexPath = m_modelpages->item(i)->data(DeviceRole).value<NetworkDeviceBase *>()->path();
        if (indexPath == path) {
            m_lvnmpages->setCurrentIndex(m_modelpages->index(i, 0));
            return;
        }
    }
}

void NetworkModuleWidget::setLastDevicePath(const QString &path)
{
    m_lastDevicePath = path;
}

void NetworkModuleWidget::initSetting(const int settingIndex, const QString &searchPath)
{
    if (!searchPath.isEmpty())
        m_modelpages->itemFromIndex(m_modelpages->index(settingIndex, 0))->setData(searchPath, SearchPath);

    m_lvnmpages->setCurrentIndex(m_modelpages->index(settingIndex, 0));
    m_lvnmpages->clicked(m_modelpages->index(settingIndex, 0));
}

static PageType getPageTypeFromModelName(const QString &modelName)
{
    if (modelName == "networkWired")
        return PageType::WiredPage;

    if (modelName == "networkWireless")
        return PageType::WirelessPage;

    if (modelName == "personalHotspot")
        return PageType::HotspotPage;

    if (modelName =="applicationProxy")
        return PageType::AppProxyPage;

    if (modelName == "networkDetails")
        return PageType::NetworkInfoPage;

    if (modelName == "networkDsl")
        return PageType::DSLPage;

    if (modelName == "systemProxy")
        return PageType::SysProxyPage;

    if (modelName == "networkVpn")
        return PageType::VPNPage;

    return PageType::NonePage;
}

void NetworkModuleWidget::setModelVisible(const QString &modelName, const bool &visible)
{
    PageType type = getPageTypeFromModelName(modelName);
    for (int i = 0; i < m_modelpages->rowCount(); i++) {
        if (m_modelpages->item(i)->data(SectionRole).value<PageType>() == type)
            m_lvnmpages->setRowHidden(i, !visible);
    }
}

int NetworkModuleWidget::gotoSetting(const QString &path)
{
    PageType type = PageType::NonePage;
    if (path == QStringLiteral("Network Details")) {
        type = PageType::NetworkInfoPage;
    } else if (path == QStringLiteral("Application Proxy")) {
        type = PageType::AppProxyPage;
    } else if (path == QStringLiteral("System Proxy")) {
        type = PageType::SysProxyPage;
    } else if (path == QStringLiteral("VPN")) {
        type = PageType::VPNPage;
    } else if (path == QStringLiteral("DSL")) {
        type = PageType::DSLPage;
    } else if (path.contains("WirelessPage") || path.contains("Wireless Network")) {
        // 历史原因，WirelessPage可能也在使用中
        type = PageType::WirelessPage;
    } else if (path.contains("Wired Network")) {
        type = PageType::WiredPage;
    } else if (path == QStringLiteral("Personal Hotspot")) {
        type = PageType::HotspotPage;
    }
    int index = -1;
    for (int i = 0; i < m_modelpages->rowCount(); ++i) {
        if (m_modelpages->item(i)->data(SectionRole).value<PageType>() == type) {
            index = i;
            break;
        }
    }

    return index;
}

void NetworkModuleWidget::onDeviceStatusChanged()
{
    for (int i = 0; i < m_modelpages->rowCount(); i++) {
        DStandardItem *item = static_cast<DStandardItem *>(m_modelpages->item(i));
        PageType pageType = item->data(SectionRole).value<PageType>();
        if (pageType != PageType::WiredPage && pageType != PageType::WirelessPage)
            continue;

        NetworkDeviceBase *device = item->data(DeviceRole).value<NetworkDeviceBase *>();
        if (!device)
            continue;

        QPointer<DViewItemAction> dummyStatus = item->data(DumyStatusRole).value<QPointer<DViewItemAction>>();
        QString txt = device->isEnabled() ? device->property("statusName").toString() : tr("Disabled");
        dummyStatus->setText(txt);
    }
}

void NetworkModuleWidget::onDeviceChanged()
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    QModelIndex currentIndex = m_lvnmpages->currentIndex();
    PageType pageType = currentIndex.data(SectionRole).value<PageType>();
    NetworkDeviceBase *currentDevice = currentIndex.data(DeviceRole).value<NetworkDeviceBase *>();

    for (int i = m_modelpages->rowCount() - 1; i >= 0; i--) {
        QStandardItem *item = m_modelpages->item(i);
        PageType itemPageType = item->data(SectionRole).value<PageType>();
        if (itemPageType != PageType::WiredPage && itemPageType != PageType::WirelessPage
                && itemPageType != PageType::HotspotPage)
            continue;

        m_modelpages->removeRow(i);
    }

    bool supportHotspot = false;
    for (NetworkDeviceBase *device : devices) {
        if (device->supportHotspot()) {
            supportHotspot = true;
            break;
        }
    }

    m_switchIndex = true;
    int newRowIndex = -1;
    for (int i = 0; i < devices.size(); i++) {
        NetworkDeviceBase *device = devices[i];
        DStandardItem *deviceItem = new DStandardItem(device->deviceName());
        if (device->deviceType() == DeviceType::Wireless) {
             //~ contents_path /network/WirelessPage
            deviceItem->setData(QVariant::fromValue(PageType::WirelessPage), SectionRole);
            deviceItem->setIcon(QIcon::fromTheme("dcc_wifi"));
        } else {
             //~ contents_path /network/Wired Network
            deviceItem->setData(QVariant::fromValue(PageType::WiredPage), SectionRole);
            deviceItem->setIcon(QIcon::fromTheme("dcc_ethernet"));
        }
        deviceItem->setData(QVariant::fromValue(device), DeviceRole);

        QPointer<DViewItemAction> dummyStatus(new DViewItemAction(Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter));
        deviceItem->setActionList(Qt::Edge::RightEdge, { dummyStatus });
        deviceItem->setData(QVariant::fromValue(dummyStatus), DumyStatusRole);

        if (!dummyStatus.isNull()) {
            if (device->isEnabled())
                dummyStatus->setText(device->property("statusName").toString());
            else
                dummyStatus->setText(tr("Disabled"));

            m_lvnmpages->update();
        }

        auto setDeviceStatus = [ this, device, dummyStatus ] {
            if (!dummyStatus.isNull()) {
                QString txt = device->isEnabled() ? device->property("statusName").toString() : tr("Disabled");
                dummyStatus->setText(txt);
            }
            this->m_lvnmpages->update();
        };

        connect(device, &NetworkDeviceBase::enableChanged, this, setDeviceStatus);
        connect(device, &NetworkDeviceBase::deviceStatusChanged, this, setDeviceStatus);
        if (device->deviceType() == DeviceType::Wireless) {
            WirelessDevice *wirelssDev = qobject_cast<WirelessDevice *>(device);
            if (wirelssDev)
                connect(wirelssDev, &WirelessDevice::hotspotEnableChanged, this, setDeviceStatus);
        }

        m_modelpages->insertRow(i, deviceItem);

        if (pageType == PageType::WiredPage || pageType == PageType::WirelessPage) {
            // 如果是有线网络或者无线网络，则根据之前的设备和当前的设备是否相同来获取索引
            // 此时调用setCurrentIndex无需再次创建三级菜单窗口，因此，将m_switchIndex赋值为false
            if (currentDevice == device) {
                newRowIndex = i;
                m_switchIndex = false;
            }
        }
    }
    // 如果之前是通过调用关闭热点后，因为关闭热点会触发一次deviceRemove，然后再触发一次deviceAdd，导致这个函数会被调用两次，因此，此处需要判断
    if (!m_lastDevicePath.isEmpty()) {
        for (int i = 0; i < devices.size(); i++) {
            NetworkDeviceBase *device = devices[i];
            if (device->path() == m_lastDevicePath) {
                newRowIndex = i;
                m_lastDevicePath.clear();
                m_switchIndex = true;
                break;
            }
        }
    }

    if (supportHotspot) {
        //~ contents_path /network/Personal Hotspot
        DStandardItem *hotspotit = new DStandardItem(tr("Personal Hotspot"));
        hotspotit->setData(QVariant::fromValue(PageType::HotspotPage), SectionRole);
        hotspotit->setIcon(QIcon::fromTheme("dcc_hotspot"));
        int hotspotRow = m_modelpages->rowCount() - 1;
        m_modelpages->insertRow(hotspotRow, hotspotit);
        GSettingWatcher::instance()->bind("personalHotspot", m_lvnmpages, hotspotit);
    }
    // 获取之前的索引就和当前的索引对比
    if (newRowIndex < 0) {
        for (int i = 0; i < m_modelpages->rowCount(); i++) {
            QStandardItem *currentRowItem = m_modelpages->item(i);
            PageType currentPageType = currentRowItem->data(SectionRole).value<PageType>();
            if (currentPageType == PageType::WiredPage || currentPageType == PageType::WirelessPage)
                continue;

            if (pageType == currentPageType) {
                newRowIndex = i;
                m_switchIndex = false;
            }
        }
    }

    if (newRowIndex < 0)
        newRowIndex = 0;

    setCurrentIndex(newRowIndex);
    m_switchIndex = true;
}
