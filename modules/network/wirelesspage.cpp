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

#include "wirelesspage.h"
#include "accesspointwidget.h"
#include "settingsgroup.h"
#include "switchwidget.h"
#include "translucentframe.h"
#include "connectioneditpage.h"
#include "connectionwirelesseditpage.h"
#include "connectionsessionworker.h"
#include "connectionsessionmodel.h"
#include "tipsitem.h"

#include <networkmodel.h>
#include <wirelessdevice.h>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

using namespace dcc::widgets;
using namespace dcc::network;
using namespace dde::network;

WirelessPage::WirelessPage(WirelessDevice *dev, QWidget *parent)
    : ContentWidget(parent),

      m_device(dev),

      m_listGroup(new SettingsGroup),
      m_tipsGroup(new SettingsGroup),

      m_connectHideSSID(new AccessPointWidget(this)),

      m_closeHotspotBtn(new QPushButton)
{
    m_sortDelayTimer.setInterval(100);
    m_sortDelayTimer.setSingleShot(true);

    m_connectHideSSID->setAPName(tr("Connect to hidden network"));
    m_connectHideSSID->setStrength(-1);

    m_closeHotspotBtn->setText(tr("Close hotspot"));

    TipsItem *tips = new TipsItem;
    tips->setText(tr("Please firstly disabled hotspot sharing if you want to connect and use wireless network function"));

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
    setTitle(dev->path());
#else
    setTitle(tr("WLAN"));
#endif

    connect(&m_sortDelayTimer, &QTimer::timeout, this, &WirelessPage::sortAPList);
    connect(m_connectHideSSID, &AccessPointWidget::requestEdit, this, &WirelessPage::showConnectHidePage);
    connect(m_closeHotspotBtn, &QPushButton::clicked, this, &WirelessPage::onCloseHotspotClicked);
    connect(dev, &WirelessDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(dev, &WirelessDevice::apInfoChanged, this, &WirelessPage::onAPChanged);
    connect(dev, &WirelessDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    connect(dev, &WirelessDevice::removed, this, &WirelessPage::onDeviceRemoved);
    connect(dev, &WirelessDevice::sessionCreated, this, &WirelessPage::showAPEditPage);
    connect(dev, &WirelessDevice::activeConnectionChanged, this, &WirelessPage::updateActiveAp);
    connect(dev, &WirelessDevice::hotspotEnabledChanged, this, &WirelessPage::onHotspotEnableChanged);

    // init data
    const QJsonArray mApList = m_device->apList();
    if (!mApList.isEmpty()) {
        for (auto ap : mApList) {
            onAPAdded(ap.toObject());
        }
    }
    QTimer::singleShot(100, this, [=] {
        emit requestDeviceAPList(m_device->path());
        emit requestWirelessScan();
    });
}

WirelessPage::~WirelessPage()
{
    if (!m_apEditPage.isNull())
        m_apEditPage->deleteLater();
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

        w->setConnected(ssid == m_device->activeConnName());
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

    w->setEncyrpt(apInfo.value("Secured").toBool());

    m_sortDelayTimer.start();
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
    emit requestDisconnectConnection(m_device->hotspotUuid());
    emit requestDeviceRemanage(m_device->path());
}

void WirelessPage::onDeviceRemoved()
{
    /* TODO: back all edit page's subpage */
    // back if ap edit page exist
    if (!m_apEditPage.isNull())
        m_apEditPage->back();

    // destroy self page
    emit back();
}

void WirelessPage::sortAPList()
{
    auto cmpFunc = [=](const AccessPointWidget *a, const AccessPointWidget *b) {
        if (a->connected() != b->connected())
            return a->connected();
        return a->strength() > b->strength();
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
}

void WirelessPage::onApWidgetEditRequested(const QString &apPath, const QString &ssid)
{
    const QString uuid = connectionUuid(ssid);

    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), uuid);

    if (!uuid.isEmpty())
    {
        m_editingUuid = uuid;

        m_apEditPage->initSettingsWidget();

        //emit requestEditAP(m_device->path(), uuid);
        //return;
    } else {
        m_apEditPage->initSettingsWidgetFromAp(apPath);
    }

    connect(m_apEditPage, &ConnectionEditPageNew::requestNextPage, this, &WirelessPage::requestNextPage);

    emit requestNextPage(m_apEditPage);

    //emit requestCreateApConfig(m_device->path(), path);
}

void WirelessPage::onApWidgetConnectRequested(const QString &path, const QString &ssid)
{
    const QString uuid = connectionUuid(ssid);

    Q_ASSERT(!uuid.isEmpty());

    emit requestConnectAp(m_device->path(), path, uuid);
}

void WirelessPage::showConnectHidePage()
{
//    if (m_connectHidePage.isNull())
//    {
//        m_connectHidePage = new ConnectHiddenPage(this);
//        connect(m_connectHidePage, &ConnectHiddenPage::requestNextPage, this, &WirelessPage::requestNextPage);
//    }

//    emit requestNextPage(m_connectHidePage);

    m_apEditPage = new ConnectionWirelessEditPage(m_device->path());
    m_apEditPage->initSettingsWidget();
    connect(m_apEditPage, &ConnectionEditPageNew::requestNextPage, this, &WirelessPage::requestNextPage);
    emit requestNextPage(m_apEditPage);

    //emit requestCreateAp("wireless", m_device->path());
}

void WirelessPage::showAPEditPage(const QString &session)
{
    Q_UNUSED(session)
    // ensure edit page is empty
    //Q_ASSERT(m_apEditPage.isNull());

    //m_apEditPage = new ConnectionEditPage;

    //ConnectionSessionModel *sessionModel = new ConnectionSessionModel(m_apEditPage);
    //ConnectionSessionWorker *sessionWorker = new ConnectionSessionWorker(session, sessionModel, m_apEditPage);

    //m_apEditPage->setModel(m_model, sessionModel);
    //connect(m_apEditPage, &ConnectionEditPage::requestCancelSession, sessionWorker, &ConnectionSessionWorker::closeSession);
    //connect(m_apEditPage, &ConnectionEditPage::requestChangeSettings, sessionWorker, &ConnectionSessionWorker::changeSettings);
    //connect(m_apEditPage, &ConnectionEditPage::requestSave, sessionWorker, &ConnectionSessionWorker::saveSettings);
    //connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    //connect(m_apEditPage, &ConnectionEditPage::requestRemove, [this] { emit requestDeleteConnection(m_editingUuid); });
    //connect(m_apEditPage, &ConnectionEditPage::requestDisconnect, [this] { emit requestDisconnectConnection(m_editingUuid); });
    //connect(m_apEditPage, &ConnectionEditPage::requestFrameKeepAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);
    //emit requestNextPage(m_apEditPage);
}

void WirelessPage::updateActiveAp()
{
    for (auto it(m_apItems.cbegin()); it != m_apItems.cend(); ++it)
        it.value()->setConnected(it.key() == m_device->activeConnName());

    sortAPList();
}

QString WirelessPage::connectionUuid(const QString &ssid)
{
    QString uuid;

    QList<QJsonObject> connections = m_device->connections();
    for (auto item : connections) {
        if (item.value("Ssid").toString() != ssid)
            continue;
        if (item.value("HwAddress").toString() != m_device->usingHwAdr())
            continue;

        uuid = item.value("Uuid").toString();
        if (!uuid.isEmpty())
            break;
    }

    return uuid;
}
