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
 *             listenerri <listenerri@gmail.com>
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

#include "wirelesspage.h"
#include "accesspointwidget.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/translucentframe.h"
#include "widgets/tipsitem.h"
#include "connectionwirelesseditpage.h"

#include <networkmodel.h>
#include <wirelessdevice.h>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <DDBusSender>
#include <QJsonDocument>

using namespace dcc::widgets;
using namespace dcc::network;
using namespace dde::network;

WirelessPage::WirelessPage(WirelessDevice *dev, QWidget *parent)
    : ContentWidget(parent),
      m_device(dev),
      m_listGroup(new SettingsGroup),
      m_tipsGroup(new SettingsGroup),
      m_connectHideSSID(new AccessPointWidget(this)),
      m_closeHotspotBtn(new QPushButton),
      m_currentClickApw(nullptr),
      m_sortDelayTimer(new QTimer(this)),
      m_indicatorDelayTimer(new QTimer(this))
{
    m_sortDelayTimer->setInterval(100);
    m_sortDelayTimer->setSingleShot(true);

    m_indicatorDelayTimer->setInterval(300);
    m_indicatorDelayTimer->setSingleShot(true);

    m_connectHideSSID->setAPName(tr("Connect to hidden network"));
    m_connectHideSSID->setStrength(-1);

    m_closeHotspotBtn->setText(tr("Close Hotspot"));

    TipsItem *tips = new TipsItem;
    tips->setText(tr("Please disable hotspot sharing first if you want to connect to a wireless network"));

    m_tipsGroup->appendItem(tips);
    m_listGroup->appendItem(m_connectHideSSID);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_listGroup);
    mainLayout->addWidget(m_tipsGroup);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_closeHotspotBtn);
    mainLayout->addSpacing(10);
    mainLayout->addStretch();

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
#ifdef QT_DEBUG
    setTitle(m_device->path());
#else
    setTitle(tr("WLAN"));
#endif

    connect(m_sortDelayTimer, &QTimer::timeout, this, &WirelessPage::sortAPList);
    connect(m_indicatorDelayTimer, &QTimer::timeout, this, &WirelessPage::refreshLoadingIndicator);
    connect(m_connectHideSSID, &AccessPointWidget::requestEdit, this, &WirelessPage::showConnectHidePage);
    connect(m_closeHotspotBtn, &QPushButton::clicked, this, &WirelessPage::onCloseHotspotClicked);
    connect(m_device, &WirelessDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(m_device, &WirelessDevice::apInfoChanged, this, &WirelessPage::onAPChanged);
    connect(m_device, &WirelessDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    connect(m_device, &WirelessDevice::activeWirelessConnectionInfoChanged, this, &WirelessPage::updateActiveAp);
    connect(m_device, &WirelessDevice::hotspotEnabledChanged, this, &WirelessPage::onHotspotEnableChanged);
    connect(m_device, &WirelessDevice::removed, this, &WirelessPage::onDeviceRemoved);
    connect(m_device, &WirelessDevice::activateAccessPointFailed, this, &WirelessPage::onActivateApFailed);
    connect(m_device, &WirelessDevice::activeConnectionsChanged, m_indicatorDelayTimer, static_cast<void (QTimer::*) ()>(&QTimer::start));

    // init data
    const QJsonArray mApList = m_device->apList();
    if (!mApList.isEmpty()) {
        for (auto ap : mApList) {
            onAPAdded(ap.toObject());
        }
    }
    QTimer::singleShot(100, this, [=] {
        Q_EMIT requestDeviceAPList(m_device->path());
        Q_EMIT requestWirelessScan();
    });
}

WirelessPage::~WirelessPage()
{
}

void WirelessPage::setModel(NetworkModel *model)
{
    m_model = model;

//    connect(m_model, &NetworkModel::activeConnInfoChanged, this, &WirelessPage::onActiveConnInfoChanged);

//    onActiveConnInfoChanged(m_model->activeConnInfos());

    onHotspotEnableChanged(m_device->hotspotEnabled());
}

void WirelessPage::onAPAdded(const QJsonObject &apInfo)
{
    const QString &ssid = apInfo.value("Ssid").toString();

    if (!m_apItems.contains(ssid)) {
        AccessPointWidget *w = new AccessPointWidget;

        w->setConnected(ssid == m_device->activeApSsid());
        w->setAPName(ssid);

        connect(w, &AccessPointWidget::requestEdit, this, &WirelessPage::onApWidgetEditRequested);
        connect(w, &AccessPointWidget::requestConnect, this, &WirelessPage::onApWidgetConnectRequested);

        m_apItems.insert(ssid, w);
        m_listGroup->appendItem(w);
    }

    onAPChanged(apInfo);
}

void WirelessPage::onAPChanged(const QJsonObject &apInfo)
{
    const QString &ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid))
        return;

    const QString &path = apInfo.value("Path").toString();
    const int strength = apInfo.value("Strength").toInt();

    AccessPointWidget *w = m_apItems[ssid];

    if (w->path() == path)
    {
        w->setStrength(strength);
    }
    else if (strength > w->strength())
    {
        w->setStrength(strength);
        w->setPath(path);
    }

    w->setEncrypt(apInfo.value("Secured").toBool());

    m_sortDelayTimer->start();
}

void WirelessPage::onAPRemoved(const QJsonObject &apInfo)
{
    const QString &ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid))
        return;

    const QString &path = apInfo.value("Path").toString();

    AccessPointWidget *w = m_apItems[ssid];

    if (w->path() ==  path) {
        m_apItems.remove(ssid);
        m_listGroup->removeItem(w);
        w->deleteLater();
    }
}

void WirelessPage::onHotspotEnableChanged(const bool enabled)
{
    m_closeHotspotBtn->setVisible(enabled);
    m_tipsGroup->setVisible(enabled);
    m_listGroup->setVisible(!enabled);
}

void WirelessPage::onCloseHotspotClicked()
{
    Q_EMIT requestDisconnectConnection(m_device->activeHotspotUuid());
    Q_EMIT requestDeviceRemanage(m_device->path());
}

void WirelessPage::onDeviceRemoved()
{
    // back if ap edit page exist
    if (!m_apEditPage.isNull()) {
        m_apEditPage->onDeviceRemoved();
    }

    // destroy self page
    Q_EMIT back();
}

void WirelessPage::onActivateApFailed(const QString &apPath, const QString &uuid)
{
    Q_UNUSED(uuid);

    if (m_currentClickApw && m_currentClickApw->path() == apPath) {
        qDebug() << "wireless connect failed and may require more configuration,"
            << "path:" << m_currentClickApw->path() << "ssid" << m_currentClickApw->ssid()
            << "secret:" << m_currentClickApw->encrypt() << "strength" << m_currentClickApw->strength();

        onApWidgetEditRequested(m_currentClickApw->path(), m_currentClickApw->ssid());
    }
}

void WirelessPage::refreshLoadingIndicator()
{
    QString activeSsid;
    for (auto activeConnObj : m_device->activeConnections()) {
        if (activeConnObj.value("Vpn").toBool(false)) {
            continue;
        }
        // the State of Active Connection
        // 0:Unknow, 1:Activating, 2:Activated, 3:Deactivating, 4:Deactivated
        if (activeConnObj.value("State").toInt(0) != 1) {
            break;
        }
        activeSsid = activeConnObj.value("Id").toString();
        break;
    }

    for (auto it = m_apItems.constBegin(); it != m_apItems.constEnd(); ++it) {
        it.value()->setLoadingIndicatorVisible(it.key() == activeSsid);
    }
}

void WirelessPage::sortAPList()
{
    auto cmpFunc = [=](const AccessPointWidget *a, const AccessPointWidget *b) {
        if (a->connected() != b->connected())
            return a->connected();
        return *a > *b;
    };

    QList<AccessPointWidget *> sortedList;
    for (auto it(m_apItems.cbegin()); it != m_apItems.cend(); ++it)
    {
        const auto index = std::upper_bound(sortedList.begin(), sortedList.end(), it.value(), cmpFunc);

        sortedList.insert(index, it.value());
    }

    // sort list
    for (int i(0); i != sortedList.size(); ++i)
        m_listGroup->moveItem(sortedList[i], i);

    m_indicatorDelayTimer->start();
}

void WirelessPage::onApWidgetEditRequested(const QString &apPath, const QString &ssid)
{
    const QString uuid = connectionUuid(ssid);

    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), uuid);

    if (!uuid.isEmpty())
    {
        m_editingUuid = uuid;

        m_apEditPage->initSettingsWidget();
    } else {
        m_apEditPage->initSettingsWidgetFromAp(apPath);
    }

    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    connect(m_apEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);

    Q_EMIT requestNextPage(m_apEditPage);
}

void WirelessPage::onApWidgetConnectRequested(const QString &path, const QString &ssid)
{
    m_currentClickApw = dynamic_cast<AccessPointWidget *>(sender());
    if (!m_currentClickApw) {
        qDebug() << "warning: init current clicked ap widget failed!";
    }

    const QString uuid = connectionUuid(ssid);

    // uuid could be empty
//    Q_ASSERT(!uuid.isEmpty());

    Q_EMIT requestConnectAp(m_device->path(), path, uuid);
}

void WirelessPage::showConnectHidePage()
{
    m_apEditPage = new ConnectionWirelessEditPage(m_device->path());
    m_apEditPage->initSettingsWidget();
    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    connect(m_apEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_apEditPage);
}

void WirelessPage::updateActiveAp()
{
    for (auto it(m_apItems.cbegin()); it != m_apItems.cend(); ++it)
        it.value()->setConnected(it.key() == m_device->activeApSsid());

    m_sortDelayTimer->start();
}

QString WirelessPage::connectionUuid(const QString &ssid)
{
    QString uuid;

    QList<QJsonObject> connections = m_device->connections();
    for (auto item : connections) {
        if (item.value("Ssid").toString() != ssid)
            continue;

        uuid = item.value("Uuid").toString();
        if (!uuid.isEmpty())
            break;
    }

    return uuid;
}
