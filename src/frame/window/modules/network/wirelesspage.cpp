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
#include "connectionwirelesseditpage.h"
#include "widgets/settingsgroup.h"
#include "widgets/switchwidget.h"
#include "widgets/translucentframe.h"
#include "widgets/tipsitem.h"

#include <DStyle>
#include <DStyleHelper>
#include <networkmodel.h>
#include <wirelessdevice.h>

#include <QMap>
#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>
#include <QPointer>
#include <QPushButton>
#include <DDBusSender>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardItem>
#include <QStandardItemModel>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

APItem::APItem(const QString &text, QStyle *style)
    : DStandardItem(text)
{
    setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    setCheckable(true);

    DStyleHelper dstyle(style);
    m_secureAction = new DViewItemAction(Qt::AlignCenter, QSize(11, 11), QSize(), false);
    m_secureAction->setIcon(dstyle.standardIcon(DStyle::SP_LockElement, nullptr, nullptr));
    m_secureAction->setVisible(false);
    setActionList(Qt::Edge::LeftEdge, { m_secureAction });

    DViewItemAction *editAction = new DViewItemAction(Qt::AlignmentFlag::AlignRight, QSize(24, 24), QSize(), true);
    editAction->setIcon(QIcon::fromTheme("arrow-right"));
    setActionList(Qt::Edge::RightEdge, { editAction });
}

void APItem::setSecure(bool isSecure)
{
    if (m_secureAction) {
        m_secureAction->setVisible(isSecure);
    }
    setData(isSecure, SecureRole);
}

bool APItem::secure() const
{
    return data(SecureRole).toBool();
}

void APItem::setSignalStrength(int ss)
{
    if (ss < 0) {
        setIcon(QPixmap());
        return;
    }
    setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(ss / 10 & ~1)));
    APSortInfo si = data(SortRole).value<APSortInfo>();
    si.signalstrength = ss;
    setData(QVariant::fromValue(si), SortRole);
}

int APItem::signalStrength() const
{
    return data(SortRole).value<APSortInfo>().signalstrength;
}

void APItem::setConnected(bool connected)
{
    setCheckState(connected ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
}

void APItem::setSortInfo(const APSortInfo &si)
{
    setData(QVariant::fromValue(si), SortRole);
}

void APItem::setPath(const QString &path)
{
    setData(path, PathRole);
}

QString APItem::path() const
{
    return data(PathRole).toString();
}

QAction *APItem::action() const
{
    return actionList(Qt::Edge::RightEdge).front();
}

bool APItem::operator<(const QStandardItem &other) const
{
    return data(SortRole).value<APSortInfo>() < other.data(SortRole).value<APSortInfo>();
}

WirelessPage::WirelessPage(WirelessDevice *dev, QWidget *parent)
    : ContentWidget(parent)
    , m_device(dev)
    , m_tipsGroup(new SettingsGroup)
    , m_closeHotspotBtn(new QPushButton)
    , m_sortDelayTimer(new QTimer(this))
    , m_indicatorDelayTimer(new QTimer(this))
    , m_lvAP(new DListView(this))
    , m_modelAP(new QStandardItemModel(m_lvAP))
    , m_switch(new SwitchWidget())
{
    qRegisterMetaType<APSortInfo>();
    m_lvAP->setModel(m_modelAP);
    m_lvAP->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_modelAP->setSortRole(APItem::SortRole);
    m_sortDelayTimer->setInterval(100);
    m_sortDelayTimer->setSingleShot(true);

    m_indicatorDelayTimer->setInterval(300);
    m_indicatorDelayTimer->setSingleShot(true);

    APItem *nonbc = new APItem(tr("Connect to hidden network"), style());
    nonbc->setSignalStrength(-1);
    nonbc->setPath("");
    nonbc->setSortInfo({-1, "", false});
    m_modelAP->appendRow(nonbc);

    m_switch->setTitle(tr("Wireless Network Adapter"));
    m_switch->setChecked(dev->enabled());
    connect(m_switch, &SwitchWidget::checkedChanged, this,
            std::bind(&WirelessPage::requestDeviceEnabled, this, dev->path(), std::placeholders::_1));
    connect(m_device, &NetworkDevice::enableChanged, m_switch, &SwitchWidget::setChecked);

    m_closeHotspotBtn->setText(tr("Close Hotspot"));

    TipsItem *tips = new TipsItem;
    tips->setText(tr("Please disable hotspot sharing first if you want to connect to a wireless network"));

    m_tipsGroup->appendItem(tips);

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_switch);
    mainLayout->addWidget(m_lvAP);
    mainLayout->addWidget(m_tipsGroup);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(m_closeHotspotBtn);
    mainLayout->addSpacing(10);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContent(mainWidget);
#ifdef QT_DEBUG
    setTitle(m_device->path());
#else
    setTitle(tr("WLAN"));
#endif

    connect(m_lvAP, &QListView::clicked, this, [this](const QModelIndex &idx) {
        if (idx.data(APItem::PathRole).toString().length() == 0) {
            this->showConnectHidePage();
            return;
        }
        this->onApWidgetConnectRequested(idx.data(APItem::PathRole).toString(),
                                         idx.data(Qt::ItemDataRole::DisplayRole).toString());
    });

    connect(m_sortDelayTimer, &QTimer::timeout, this, &WirelessPage::sortAPList);
    connect(m_indicatorDelayTimer, &QTimer::timeout, this, &WirelessPage::refreshLoadingIndicator);
    connect(m_closeHotspotBtn, &QPushButton::clicked, this, &WirelessPage::onCloseHotspotClicked);
    connect(m_device, &WirelessDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(m_device, &WirelessDevice::apInfoChanged, this, &WirelessPage::onAPChanged);
    connect(m_device, &WirelessDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    connect(m_device, &WirelessDevice::activeApInfoChanged, this, &WirelessPage::updateActiveAp);
    connect(m_device, &WirelessDevice::hotspotEnabledChanged, this, &WirelessPage::onHotspotEnableChanged);
    connect(m_device, &WirelessDevice::removed, this, &WirelessPage::onDeviceRemoved);
    connect(m_device, &WirelessDevice::activateAccessPointFailed, this, &WirelessPage::onActivateApFailed);
    connect(m_device, &WirelessDevice::activeConnectionsChanged, m_indicatorDelayTimer, static_cast<void (QTimer::*)()>(&QTimer::start));

    // init data
    const QJsonArray mApList = m_device->apList();
    if (!mApList.isEmpty()) {
        for (auto ap : mApList) {
            onAPAdded(ap.toObject());
        }
    }
    QTimer::singleShot(100, this, [ = ] {
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
        APItem *apItem = new APItem(ssid, style());
        m_apItems[ssid] = apItem;
        m_modelAP->appendRow(apItem);
        apItem->setSecure(apInfo.value("Secured").toBool());
        apItem->setPath(apInfo.value("Path").toString());
        apItem->setConnected(ssid == m_device->activeApSsid());
        apItem->setSignalStrength(apInfo.value("Strength").toInt());
        connect(apItem->action(), &QAction::triggered, [this, apItem] {
            this->onApWidgetEditRequested(apItem->data(APItem::PathRole).toString(),
                                          apItem->data(Qt::ItemDataRole::DisplayRole).toString());
        });
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
    const bool isSecure = apInfo.value("Secured").toBool();

    APItem *it = m_apItems[ssid];
    APSortInfo si{strength, ssid, ssid == m_device->activeApSsid()};

    m_apItems[ssid]->setSortInfo(si);

    if (it->path() == path) {
        m_apItems[ssid]->setSignalStrength(strength);
    } else if (strength > it->signalStrength()) {
        m_apItems[ssid]->setSignalStrength(strength);
        m_apItems[ssid]->setPath(path);
    }

    //TODO: Wireless security icon

    m_sortDelayTimer->start();
}

void WirelessPage::onAPRemoved(const QJsonObject &apInfo)
{
    const QString &ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid))
        return;

    const QString &path = apInfo.value("Path").toString();

    if (m_apItems[ssid]->path() ==  path) {
        m_modelAP->removeRow(m_modelAP->indexFromItem(m_apItems[ssid]).row());
        m_apItems.erase(m_apItems.find(ssid));
    }
}

void WirelessPage::onHotspotEnableChanged(const bool enabled)
{
    m_closeHotspotBtn->setVisible(enabled);
    m_tipsGroup->setVisible(enabled);
    m_lvAP->setVisible(!enabled);
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

    qDebug() << "wireless connect failed and may require more configuration,"
             << "path:" << apPath << "ssid" << connectionSsid(uuid);

    onApWidgetEditRequested(apPath, connectionSsid(uuid));
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
    //TODO (legacy feature): loading indicator?
}

void WirelessPage::sortAPList()
{
    m_modelAP->sort(0, Qt::SortOrder::DescendingOrder);

    m_indicatorDelayTimer->start();
}

void WirelessPage::onApWidgetEditRequested(const QString &apPath, const QString &ssid)
{
    const QString uuid = connectionUuid(ssid);

    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), uuid);

    if (!uuid.isEmpty()) {
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
    const QString uuid = connectionUuid(ssid);
    // uuid could be empty
    // Q_ASSERT(!uuid.isEmpty());
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
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        it.value()->setConnected(it.key() == m_device->activeApSsid());
    }

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

QString WirelessPage::connectionSsid(const QString &uuid)
{
    QString ssid;

    QList<QJsonObject> connections = m_device->connections();
    for (auto item : connections) {
        if (item.value("Uuid").toString() != uuid)
            continue;

        ssid = item.value("Ssid").toString();
        if (!ssid.isEmpty())
            break;
    }

    return ssid;
}
