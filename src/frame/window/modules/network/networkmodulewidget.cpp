/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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
#include "window/utils.h"
#include "window/gsettingwatcher.h"
#include "widgets/nextpagewidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/multiselectlistview.h"
#include <DStyleOption>
#include <networkmodel.h>
#include <networkdevice.h>
#include <wirelessdevice.h>

#include <QDebug>
#include <QPointer>
#include <QVBoxLayout>
#include <QPushButton>
#include <QProcess>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QGSettings>

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

NetworkModuleWidget::NetworkModuleWidget()
    : QWidget()
    , m_lvnmpages(new dcc::widgets::MultiSelectListView(this))
    , m_modelpages(new QStandardItemModel(this))
    , m_nmConnectionEditorProcess(nullptr)
    , m_settings(new QGSettings("com.deepin.dde.control-center", QByteArray(), this))
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

#if !defined(DISABLE_NETWORK_PROXY) || !defined(DISABLE_NETWORK_VPN) || !defined(DISABLE_NETWORK_PPPOE)
#ifndef DISABLE_NETWORK_PPPOE
    DStandardItem *pppit = new DStandardItem(tr("DSL"));
    pppit->setData(QVariant::fromValue(DSLPage), SectionRole);
    pppit->setIcon(QIcon::fromTheme("dcc_dsl"));
    m_modelpages->appendRow(pppit);
    GSettingWatcher::instance()->bind("networkDsl", m_lvnmpages, pppit);
#endif

#ifndef DISABLE_NETWORK_VPN
    DStandardItem *vpnit = new DStandardItem(tr("VPN"));
    vpnit->setData(QVariant::fromValue(VPNPage), SectionRole);
    vpnit->setIcon(QIcon::fromTheme("dcc_vpn"));
    m_modelpages->appendRow(vpnit);
    GSettingWatcher::instance()->bind("networkVpn", m_lvnmpages, vpnit);
#endif

#ifndef DISABLE_NETWORK_PROXY
    DStandardItem *prxyit = new DStandardItem(tr("System Proxy"));
    prxyit->setData(QVariant::fromValue(SysProxyPage), SectionRole);
    prxyit->setIcon(QIcon::fromTheme("dcc_system_agent"));
    m_modelpages->appendRow(prxyit);
    GSettingWatcher::instance()->bind("systemProxy", m_lvnmpages, prxyit);

    DStandardItem *aprxit = new DStandardItem(tr("Application Proxy"));
    aprxit->setData(QVariant::fromValue(AppProxyPage), SectionRole);
    aprxit->setIcon(QIcon::fromTheme("dcc_app_proxy"));
    m_modelpages->appendRow(aprxit);
    GSettingWatcher::instance()->bind("applicationProxy", m_lvnmpages, aprxit);
#endif
#endif

    DStandardItem *infoit = new DStandardItem(tr("Network Details"));
    infoit->setData(QVariant::fromValue(NetworkInfoPage), SectionRole);
    infoit->setIcon(QIcon::fromTheme("dcc_network"));
    m_modelpages->appendRow(infoit);
    GSettingWatcher::instance()->bind("networkDetails", m_lvnmpages, infoit);

    m_centralLayout->addWidget(m_lvnmpages);
    if (IsServerSystem) {
        handleNMEditor();
    }

    initProxyStatus();

    connect(m_lvnmpages, &DListView::activated, this, &NetworkModuleWidget::onClickCurrentListIndex);
    connect(m_lvnmpages, &DListView::clicked, m_lvnmpages, &DListView::activated);

    connect(GSettingWatcher::instance(), &GSettingWatcher::requestUpdateSecondMenu, this, &NetworkModuleWidget::updateSecondMenu);

    connect(m_settings, &QGSettings::changed, this, [ = ](const QString &key) {
        if (key == "networkWired" || key == "networkWireless") {
            for (int i = 0; i < m_modelpages->rowCount(); i++) {
                if (m_modelpages->index(i, 0).data(SectionRole).value<PageType>() == WiredPage) {
                    bool status = m_settings->get("networkWired").toBool();
                    m_lvnmpages->setRowHidden(i, !status);
                    if (!status) updateSecondMenu(i);
                } else if (m_modelpages->index(i, 0).data(SectionRole).value<PageType>() == WirelessPage) {
                    bool status = m_settings->get("networkWireless").toBool();
                    m_lvnmpages->setRowHidden(i, !status);
                    if (!status) updateSecondMenu(i);
                }
            }
        }
    });
}

NetworkModuleWidget::~NetworkModuleWidget()
{
    if (m_nmConnectionEditorProcess) {
        m_nmConnectionEditorProcess->close();
        m_nmConnectionEditorProcess->deleteLater();
        m_nmConnectionEditorProcess = nullptr;
    }
}

void NetworkModuleWidget::onClickCurrentListIndex(const QModelIndex &idx)
{
    //查询一次之后去掉SearchPath,避免下次进来的时候还会search一遍
    const QString searchPath = idx.data(SearchPath).toString();
    m_modelpages->itemFromIndex(idx)->setData("", SearchPath);
    if (m_lastIndex == idx && searchPath.isEmpty()) return;

    PageType type = idx.data(SectionRole).value<PageType>();
    m_lastIndex = idx;
    m_lvnmpages->setCurrentIndex(idx);
    switch (type) {
    case DSLPage:
        Q_EMIT requestShowPppPage(searchPath);
        break;
    case VPNPage:
        Q_EMIT requestShowVpnPage(searchPath);
        break;
    case SysProxyPage:
        Q_EMIT requestShowProxyPage();
        break;
    case AppProxyPage:
        Q_EMIT requestShowChainsPage();
        break;
    case HotspotPage:
        Q_EMIT requestHotspotPage();
        break;
    case NetworkInfoPage:
        Q_EMIT requestShowInfomation();
        break;
    case WiredPage:
    case WirelessPage:
        Q_EMIT requestShowDeviceDetail(idx.data(DeviceRole).value<NetworkDevice *>(), searchPath);
        break;
    default:
        break;
    }
    m_lvnmpages->resetStatus(idx);
}

void NetworkModuleWidget::onProxyMethodChanged(const QString &proxyMethod)
{
    if (proxyMethod.isEmpty()) return;
    DStandardItem *item = nullptr;
    QPointer<DViewItemAction> action(new DViewItemAction(Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter));
    if (action.isNull()) return;
    //遍历获取系统代理项,设置状态
    for (int i = 0; i < m_modelpages->rowCount(); i++) {
        item = dynamic_cast<DStandardItem *>(m_modelpages->item(i));
        if (nullptr == item) continue;
        if (item->data(SectionRole).value<PageType>() == SysProxyPage) {
            item->setActionList(Qt::Edge::RightEdge, {action});
            if ("none" == proxyMethod) {
                action->setText(tr("Disabled"));
            } else if ("manual" == proxyMethod) {
                action->setText(tr("Manual"));
            } else if ("auto" == proxyMethod) {
                action->setText(tr("Auto"));
            } else {
                action->setText(tr("Disabled"));
            }
            return;
        }
    }
}

bool NetworkModuleWidget::handleNMEditor()
{
    m_strNetworkManageOutput = "";
    QProcess *process = new QProcess(this);
    QPushButton *nmConnEditBtn = new QPushButton(tr("Configure by Network Manager"));
    m_centralLayout->addWidget(nmConnEditBtn);
    nmConnEditBtn->hide();
    process->start("which nm-connection-editor");

    connect(process, static_cast<void (QProcess::*)(int)>(&QProcess::finished), this, [ = ] {
        m_strNetworkManageOutput = process->readAll();
        if (!m_strNetworkManageOutput.isEmpty()) {
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

void NetworkModuleWidget::updateSecondMenu(int row)
{
    bool isAllHidden = true;
    for (int i = 0; i < m_modelpages->rowCount(); i++) {
        if (!m_lvnmpages->isRowHidden(i))
            isAllHidden = false;
    }

    if (m_lvnmpages->selectionModel()->selectedRows().size() > 0) {
        int index = m_lvnmpages->selectionModel()->selectedRows()[0].row();
        Q_EMIT requestUpdateSecondMenu(index == row);
    } else {
        Q_EMIT requestUpdateSecondMenu(false);
    }

    if (isAllHidden) {
        m_lastIndex = QModelIndex();
        m_lvnmpages->clearSelection();
    }
}

void NetworkModuleWidget::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::deviceListChanged, this, &NetworkModuleWidget::onDeviceListChanged);
    connect(model, &NetworkModel::proxyMethodChanged, this, &NetworkModuleWidget::onProxyMethodChanged);

#ifndef DISABLE_NETWORK_PROXY
    //connect(model, &NetworkModel::appProxyExistChanged, m_appProxy, &NextPageWidget::setVisible);
    //m_appProxy->setVisible(model->appProxyExist());
#endif

    qDebug() << "network devices size:" << model->devices().size();
    onDeviceListChanged(model->devices());
}

void NetworkModuleWidget::initSetting(const int settingIndex, const QString &searchPath)
{
    if (!searchPath.isEmpty()) {
        m_modelpages->itemFromIndex(m_modelpages->index(settingIndex, 0))->setData(searchPath, SearchPath);
    }
    m_lvnmpages->setCurrentIndex(m_modelpages->index(settingIndex, 0));
    m_lvnmpages->clicked(m_modelpages->index(settingIndex, 0));
}

void NetworkModuleWidget::showDefaultWidget()
{
    for (int i = 0; i < m_modelpages->rowCount(); i++) {
        if (!m_lvnmpages->isRowHidden(i)) {
            m_lvnmpages->activated(m_modelpages->index(i, 0));
            break;
        }
    }
}

void NetworkModuleWidget::setCurrentIndex(const int settingIndex)
{
    // 设置网络列表当前索引
    m_lvnmpages->setCurrentIndex(m_modelpages->index(settingIndex, 0));
}

void NetworkModuleWidget::setIndexFromPath(const QString &path)
{
    for (int i = 0; i < m_modelpages->rowCount(); ++i) {
        QString indexPath = m_modelpages->item(i)->data(DeviceRole).value<NetworkDevice *>()->path();
        if (indexPath == path) {
            m_lvnmpages->setCurrentIndex(m_modelpages->index(i, 0));
            return;
        }
    }
}

void NetworkModuleWidget::initProxyStatus()
{
    QDBusInterface interface("com.deepin.daemon.Network", "/com/deepin/daemon/Network"
                                 , "com.deepin.daemon.Network", QDBusConnection::sessionBus(), this);
    QDBusMessage msg = interface.call("GetProxyMethod");
    QString method = msg.arguments().first().toString();
    //初始化系统代理状态
    onProxyMethodChanged(method);
}

int NetworkModuleWidget::gotoSetting(const QString &path)
{
    PageType type = NonePage;
    if (path == QStringLiteral("Network Details")) {
        type = NetworkInfoPage;
    } else if (path == QStringLiteral("Application Proxy")) {
        type = AppProxyPage;
    } else if (path == QStringLiteral("System Proxy")) {
        type = SysProxyPage;
    } else if (path == QStringLiteral("VPN") || path == QStringLiteral("Import VPN") || path == QStringLiteral("Create VPN")) {
        type = VPNPage;
    } else if (path == QStringLiteral("DSL") || path == QStringLiteral("Create PPPoE Connection")) {
        type = DSLPage;
    } else if (path.contains("Wireless Network")) {
        type = WirelessPage;
    } else if (path.contains("Wired Network") || path == QStringLiteral("Add Network Connection")) {
        type = WiredPage;
    } else if (path == QStringLiteral("Personal Hotspot")) {
        type = HotspotPage;
    }
    int index = -1;
    for (int i = 0; i < m_modelpages->rowCount(); ++i) {
        if (m_modelpages->item(i)->data(SectionRole).value<PageType>() == type) {
            index = i;
            break;
        }
    }

    qDebug() << "get network list index : " << index;
    return index;
}

void NetworkModuleWidget::onDeviceListChanged(const QList<NetworkDevice *> &devices)
{
    const QModelIndex currentIndex = m_lvnmpages->currentIndex();
    const PageType currentType = currentIndex.data(SectionRole).value<PageType>();
    const NetworkDevice *currentDevice = currentIndex.data(DeviceRole).value<NetworkDevice *>();
    const QString currentDevicePath = currentDevice != nullptr ? currentDevice->path() : "";

    while ((m_modelpages->item(0)->data(SectionRole).value<PageType>() == WiredPage)
            || (m_modelpages->item(0)->data(SectionRole).value<PageType>() == WirelessPage)) {
        m_modelpages->removeRow(0);
    }

    for (int i = 0; i < m_modelpages->rowCount(); ++i) {
        if (m_modelpages->item(i)->data(SectionRole).value<PageType>() == HotspotPage) {
            m_modelpages->removeRow(i);
            break;
        }
    }

    QList<QStandardItem *> devits;

    int wiredDevice = 0;
    int wirelessDevice = 0;
    bool haveCurrentDevice = false;
    for (auto const dev : devices) {
        switch (dev->type()) {
        case NetworkDevice::Wired:
            ++wiredDevice;
            break;
        case NetworkDevice::Wireless:
            ++wirelessDevice;
            break;
        default:
            break;
        }

        if (currentDevicePath == dev->path())
            haveCurrentDevice = true;
    }

    //如果当前item的设备路径不为空且在变更后的devices中没有找到该设备,则认为当前设备被移除了.
    bool bRemoveCurrentDevice = !currentDevicePath.isEmpty() && !haveCurrentDevice;

    // add wired device list
    int count = 0;
    for (auto const dev : devices) {
        if (dev->type() != NetworkDevice::Wired)
            continue;

        qDebug() << "add Wired item!";
        devits.push_back(createDeviceGroup(dev, ++count, wiredDevice > 1));
    }

    // add wireless device list
    count = 0;
    bool have_ap = false;
    for (auto const dev : devices) {
        if (dev->type() != NetworkDevice::Wireless)
            continue;

        qDebug() << "add Wireless item!";
        if (qobject_cast<WirelessDevice *>(dev)->supportHotspot()) {
            have_ap = true;
        }

        devits.push_back(createDeviceGroup(dev, ++count, wirelessDevice > 1));
    }

    bool detachCurrentDevice = false;
    int newRowIndex = currentDevicePath.isEmpty() ? 0 : devits.size();
    for (auto it = devits.rbegin(); it != devits.rend(); ++it) {
        m_modelpages->insertRow(0, *it);
        if ((*it)->data(SectionRole).value<PageType>() == WiredPage) {
            m_lvnmpages->setRowHidden((*it)->row(), !m_settings->get("networkWired").toBool());
        }
        if ((*it)->data(SectionRole).value<PageType>() == WirelessPage) {
            m_lvnmpages->setRowHidden((*it)->row(), !m_settings->get("networkWireless").toBool());
        }

        if (!currentDevicePath.isEmpty()){
            if (!detachCurrentDevice)
                newRowIndex--;

            NetworkDevice *device = (*it)->data(DeviceRole).value<NetworkDevice *>();
            if (nullptr != device && device->path() == currentDevicePath)
                detachCurrentDevice = true;
        }
    }

    if (have_ap) {
        //~ contents_path /network/Personal Hotspot
        //~ child_page Personal Hotspot
        DStandardItem *hotspotit = new DStandardItem(tr("Personal Hotspot"));
        hotspotit->setData(QVariant::fromValue(HotspotPage), SectionRole);
        hotspotit->setIcon(QIcon::fromTheme("dcc_hotspot"));
        m_modelpages->insertRow(m_modelpages->rowCount() - 1, hotspotit);
        GSettingWatcher::instance()->bind("personalHotspot", m_lvnmpages, hotspotit);
        if (currentType == HotspotPage)
            newRowIndex = m_modelpages->rowCount() - 2;
    }

    // 跳转第一个item的情况:
    // 1.当前item是网络设备且该设备被移除
    // 2.当前item是热点且设备变更后无热点
    if (bRemoveCurrentDevice || (currentType == HotspotPage && !have_ap)) {
        setCurrentIndex(0);
    } else if (currentType == HotspotPage || currentType == WiredPage || currentType == WirelessPage){
        setCurrentIndex(newRowIndex);
    }
}

QStandardItem *NetworkModuleWidget::createDeviceGroup(NetworkDevice *dev, const int number, const bool multiple)
{
    QString text;

    Q_EMIT dev->statusChanged(dev->statusString());

    if (dev->type() == NetworkDevice::Wired) {
        if (multiple) {
            text = tr("Wired Network %1").arg(number);
        } else {
            text = tr("Wired Network");
        }
    } else if (dev->type() == NetworkDevice::Wireless) {
        if (multiple) {
            text = tr("Wireless Network %1").arg(number);
        } else {
            text = tr("Wireless Network");
        }
    }

    DStandardItem *ret = new DStandardItem(text);
    ret->setData(dev->type() == NetworkDevice::Wireless ? WirelessPage : WiredPage, SectionRole);
    ret->setIcon(QIcon::fromTheme(dev->type() == NetworkDevice::Wireless ? "dcc_wifi" : "dcc_ethernet"));
    ret->setData(QVariant::fromValue(dev), DeviceRole);

    QPointer<DViewItemAction> dummystatus(new DViewItemAction(Qt::AlignmentFlag::AlignRight | Qt::AlignmentFlag::AlignVCenter));
    ret->setActionList(Qt::Edge::RightEdge, {dummystatus});

    if (!dummystatus.isNull()) {
        if (dev->enabled()) {
            dummystatus->setText(dev->statusString());
        } else {
            dummystatus->setText(tr("Disabled"));
        }
        m_lvnmpages->update();
    }

    connect(dev, &NetworkDevice::enableChanged, this, [this, dev, dummystatus](const bool enabled) {
        if (!dummystatus.isNull()) {
            QString txt = enabled ? dev->statusString() : tr("Disabled");
            dummystatus->setText(txt);
        }
        this->m_lvnmpages->update();
    });
    connect(dev, static_cast<void (NetworkDevice::*)(const QString &) const>(&NetworkDevice::statusChanged), this, [this, dev, dummystatus] {
        if (!dummystatus.isNull()) {
            if (dev->enabled()) {
                dummystatus->setText(dev->statusString());
            } else {
                dummystatus->setText(tr("Disabled"));
            }
        }
        this->m_lvnmpages->update();
    }, Qt::QueuedConnection);

    WirelessDevice *wirelssDev = qobject_cast<WirelessDevice *>(dev);
    if (wirelssDev) {
        if (wirelssDev->hotspotEnabled()) {
            if (!dummystatus.isNull()) {
                dummystatus->setText(tr("Disconnected"));
                m_lvnmpages->update();
            }
        }
        connect(wirelssDev, &WirelessDevice::hotspotEnabledChanged, this, [this, dummystatus](const bool enabled) {
            if (enabled && !dummystatus.isNull()) {
                dummystatus->setText(tr("Disconnected"));
                m_lvnmpages->update();
            }
        });
    }
    return ret;
}
