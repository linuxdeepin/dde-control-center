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

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

NetworkModuleWidget::NetworkModuleWidget()
    : QWidget()
    , m_lvnmpages(new dcc::widgets::MultiSelectListView(this))
    , m_modelpages(new QStandardItemModel(this))
{
    QProcess process;
    process.start("gdbus introspect -y -d com.deepin.system.SystemInfo -o /com/deepin/system/SystemInfo -p");
    process.waitForFinished();
    QByteArray pcType = process.readAllStandardOutput();
    process.close();

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

#ifndef DISABLE_NETWORK_AIRPLANE
    //判断当前的机器是否为盘古v，如果为盘古v则不需要飞行模式功能
    while (!pcType.contains("panguV")) {
        qDebug() << "This machine is not PanguV";
        //~ contents_path /network/Airplane
        DStandardItem *airplanemode = new DStandardItem(tr("Airplane Mode"));
        airplanemode->setData(QVariant::fromValue(AirplaneModepage), SectionRole);
        airplanemode->setIcon(QIcon::fromTheme("dcc_airplane_mode"));
        m_modelpages->appendRow(airplanemode);
        break;
    }
#endif

#ifndef DISABLE_NETWORK_PPPOE
    //~ contents_path /network/DSL
    DStandardItem *pppit = new DStandardItem(tr("DSL"));
    pppit->setData(QVariant::fromValue(DSLPage), SectionRole);
    pppit->setIcon(QIcon::fromTheme("dcc_dsl"));
    m_modelpages->appendRow(pppit);
#endif

#ifndef DISABLE_NETWORK_VPN
    //~ contents_path /network/VPN
    DStandardItem *vpnit = new DStandardItem(tr("VPN"));
    vpnit->setData(QVariant::fromValue(VPNPage), SectionRole);
    vpnit->setIcon(QIcon::fromTheme("dcc_vpn"));
    m_modelpages->appendRow(vpnit);
#endif

#ifndef DISABLE_NETWORK_PROXY
    //~ contents_path /network/System Proxy
    DStandardItem *prxyit = new DStandardItem(tr("System Proxy"));
    prxyit->setData(QVariant::fromValue(SysProxyPage), SectionRole);
    prxyit->setIcon(QIcon::fromTheme("dcc_system_agent"));
    m_modelpages->appendRow(prxyit);

    //~ contents_path /network/Application Proxy
    DStandardItem *aprxit = new DStandardItem(tr("Application Proxy"));
    aprxit->setData(QVariant::fromValue(AppProxyPage), SectionRole);
    aprxit->setIcon(QIcon::fromTheme("dcc_app_proxy"));
    m_modelpages->appendRow(aprxit);
#endif
#endif

    m_lastIndex = m_lvnmpages->rootIndex();

    //~ contents_path /network/Network Details
    DStandardItem *infoit = new DStandardItem(tr("Network Details"));
    infoit->setData(QVariant::fromValue(NetworkInfoPage), SectionRole);
    infoit->setIcon(QIcon::fromTheme("dcc_network"));
    m_modelpages->appendRow(infoit);
    m_centralLayout->addWidget(m_lvnmpages);
    if (IsServerSystem) {
        handleNMEditor();
    }
    connect(m_lvnmpages, &DListView::activated, this, &NetworkModuleWidget::onClickCurrentListIndex);
    connect(m_lvnmpages, &DListView::clicked, m_lvnmpages, &DListView::activated);
}

void NetworkModuleWidget::onClickCurrentListIndex(const QModelIndex &idx)
{
    PageType type = idx.data(SectionRole).value<PageType>();
    if (m_lastIndex == idx && idx.row() != 0) return;

    m_lastIndex = idx;
    m_lvnmpages->setCurrentIndex(idx);
    switch (type) {
    case DSLPage:
        Q_EMIT requestShowPppPage(idx.data(SearchPath).toString());
        break;
    case VPNPage:
        Q_EMIT requestShowVpnPage(idx.data(SearchPath).toString());
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
        Q_EMIT requestShowDeviceDetail(idx.data(DeviceRole).value<NetworkDevice *>(), idx.data(SearchPath).toString());
        break;
    case AirplaneModepage:
        Q_EMIT requestShowAirplanePage();
        break;
    default:
        break;
    }
    m_lvnmpages->resetStatus(idx);
}

bool NetworkModuleWidget::handleNMEditor()
{
    m_strNetworkManageOutput = "";
    QProcess *process = new QProcess(this);
    QPushButton *nmConnEditBtn = new QPushButton(tr("Configure by Network Manager"));
    m_centralLayout->addWidget(nmConnEditBtn);
    nmConnEditBtn->hide();
    process->start("which nm-connection-editor");

    connect(process, static_cast<void (QProcess:: *)(int)>(&QProcess::finished), this, [=]{
        m_strNetworkManageOutput = process->readAll();
        if (!m_strNetworkManageOutput.isEmpty()) {
            nmConnEditBtn->show();
            connect(nmConnEditBtn, &QPushButton::clicked, this, [] {
                QProcess::startDetached("nm-connection-editor");
            });
        }
    });
    process->deleteLater();
    return true;
}

void NetworkModuleWidget::setModel(NetworkModel *model)
{
    connect(model, &NetworkModel::deviceListChanged, this, &NetworkModuleWidget::onDeviceListChanged);
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

void NetworkModuleWidget::setCurrentIndex(const int settingIndex)
{
    m_lastIndex = m_lvnmpages->rootIndex();
    m_lvnmpages->setCurrentIndex(m_modelpages->index(settingIndex, 0));
    m_lvnmpages->clicked(m_lvnmpages->currentIndex());
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

int NetworkModuleWidget::gotoSetting(const QString &path)
{
    PageType type = NonePage;
    if (path == QStringLiteral("Network Details")) {
        type = NetworkInfoPage;
    } else if (path == QStringLiteral("Application Proxy")) {
        type = AppProxyPage;
    } else if (path == QStringLiteral("System Proxy")) {
        type = SysProxyPage;
    } else if (path == QStringLiteral("VPN")) {
        type = VPNPage;
    } else if (path == QStringLiteral("DSL")) {
        type = DSLPage;
    } else if (path.contains("Wireless Network")) {
        type = WirelessPage;
    } else if (path.contains("Wired Network")) {
        type = WiredPage;
    } else if (path == QStringLiteral("Personal Hotspot")) {
        type = HotspotPage;
    } else if (path == QStringLiteral("Airplane Mode")) {
        type = AirplaneModepage;
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
    int tempRowCounts = m_modelpages->rowCount();
    bool bRemoveCurrentDevice = false;
    QModelIndex currentIndex = m_lvnmpages->currentIndex();
    PageType currentType = currentIndex.data(SectionRole).value<PageType>();

    while ((m_modelpages->item(0)->data(SectionRole).value<PageType>() == WiredPage)
            || (m_modelpages->item(0)->data(SectionRole).value<PageType>() == WirelessPage)) {
        m_modelpages->removeRow(0);
        if ((currentType == WiredPage) || (currentType == WirelessPage)) {
            bRemoveCurrentDevice = true;
        }
    }

    for (int i = 0; i < m_modelpages->rowCount(); ++i) {
        if (m_modelpages->item(i)->data(SectionRole).value<PageType>() == HotspotPage) {
            m_modelpages->removeRow(i);
            if (currentType == HotspotPage) {
                bRemoveCurrentDevice = true;
            }
            break;
        }
    }

    QList<QStandardItem *> devits;

    int wiredDevice = 0;
    int wirelessDevice = 0;
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
    }

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
    for (auto it = devits.rbegin(); it != devits.rend(); ++it) {
        m_modelpages->insertRow(0, *it);
    }

    if (have_ap) {
        //~ contents_path /network/Personal Hotspot
        DStandardItem *hotspotit = new DStandardItem(tr("Personal Hotspot"));
        hotspotit->setData(QVariant::fromValue(HotspotPage), SectionRole);
        hotspotit->setIcon(QIcon::fromTheme("dcc_hotspot"));
        m_modelpages->insertRow(m_modelpages->rowCount() - 1, hotspotit);
    }

    if (bRemoveCurrentDevice) {
        if (tempRowCounts > m_modelpages->rowCount()) {
            setCurrentIndex(m_lastIndex.row() == -1 ? 0 : (m_lastIndex.row() >= 1 ? (m_lastIndex.row() - 1) : m_lastIndex.row()));
        } else {
            setCurrentIndex(m_lastIndex.row() == -1 ? 0 : (m_lastIndex.row() >= 1 ? (m_lastIndex.row() + 1) : m_lastIndex.row()));
        }
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
            //~ contents_path /network/Wired Network
            text = tr("Wired Network");
        }
    } else if (dev->type() == NetworkDevice::Wireless) {
        if (multiple) {
            text = tr("Wireless Network %1").arg(number);
        } else {
            //~ contents_path /network/WirelessPage
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
        if (dev->enabled() && dev->status() != NetworkDevice::Unavailable) {
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
            if (dev->enabled() && dev->status() != NetworkDevice::Unavailable) {
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
        connect(wirelssDev, &WirelessDevice::hotspotEnabledChanged, this, [this, dummystatus] (const bool enabled) {
            if (enabled && !dummystatus.isNull()) {
                dummystatus->setText(tr("Disconnected"));
                m_lvnmpages->update();
            }
        });
    }
    return ret;
}
