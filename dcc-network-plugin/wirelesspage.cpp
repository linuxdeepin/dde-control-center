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
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DStyle>
#include <DStyleOption>
#include <DListView>
#include <DSpinner>

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
#include <QThread>
#include <QScroller>
#include <QGSettings>

#include <widgets/settingsgroup.h>
#include <widgets/switchwidget.h>
#include <widgets/translucentframe.h>
#include <widgets/tipsitem.h>
#include <widgets/titlelabel.h>

#include <wirelessdevice.h>
#include <networkdevicebase.h>
#include <networkcontroller.h>
#include <wirelessdevice.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;
using namespace dde::network;

APItem::APItem(const QString &text, QStyle *style, DListView *parent)
        : DStandardItem(text)
        , m_parentView(nullptr)
        , m_dStyleHelper(style)
        , m_preLoading(false)
        , m_uuid("")
{
    setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    setCheckable(false);

    m_secureAction = new DViewItemAction(Qt::AlignCenter, QSize(), QSize(), false);
    setActionList(Qt::Edge::LeftEdge, { m_secureAction });

    m_parentView = parent;
    if (parent != nullptr) {
        m_loadingIndicator = new DSpinner();
        m_loadingIndicator->setFixedSize(20, 20);
        m_loadingIndicator->hide();
        m_loadingIndicator->stop();
        m_loadingIndicator->setParent(parent->viewport());
    }

    m_arrowAction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
    QStyleOption opt;
    m_arrowAction->setIcon(m_dStyleHelper.standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
    m_arrowAction->setClickAreaMargins(ArrowEnterClickMargin);
    setActionList(Qt::Edge::RightEdge, {m_arrowAction});
}

APItem::~APItem()
{
    if (!m_loadingIndicator.isNull()) {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        m_loadingIndicator->deleteLater();
    }
}

void APItem::setSecure(bool isSecure)
{
    if (m_secureAction)
        m_secureAction->setIcon(m_dStyleHelper.standardIcon(isSecure ? DStyle::SP_LockElement : DStyle::SP_CustomBase, nullptr, nullptr));

    setData(isSecure, SecureRole);
}

bool APItem::secure() const
{
    return data(SecureRole).toBool();
}

void APItem::setSignalStrength(int strength)
{
    if (strength < 0) {
        setIcon(QPixmap());
        return;
    }

    if (strength <= 5)
        setIcon(QIcon::fromTheme(QString("network/wireless/dcc_wireless-0")));
    else if (strength > 5 && strength <= 30)
        setIcon(QIcon::fromTheme(QString("network/wireless/dcc_wireless-2")));
    else if (strength > 30 && strength <= 55)
        setIcon(QIcon::fromTheme(QString("network/wireless/dcc_wireless-4")));
    else if (strength > 55 && strength <= 65)
        setIcon(QIcon::fromTheme(QString("network/wireless/dcc_wireless-6")));
    else if (strength > 65)
        setIcon(QIcon::fromTheme(QString("network/wireless/dcc_wireless-8")));

    APSortInfo si = data(SortRole).value<APSortInfo>();
    si.signalstrength = strength;
    si.ssid = text();
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

bool APItem::isConnected()
{
    return checkState();
}

void APItem::setSortInfo(const APSortInfo &si)
{
    setData(QVariant::fromValue(si), SortRole);
}

APSortInfo APItem::sortInfo()
{
    return data(SortRole).value<APSortInfo>();
}

void APItem::setPath(const QString &path)
{
    setData(path, PathRole);
}

QString APItem::path() const
{
    return data(PathRole).toString();
}

void APItem::setUuid(const QString &uuid)
{
    m_uuid = uuid;
}
QString APItem::uuid() const
{
    return m_uuid;
}

QAction *APItem::action() const
{
    return m_arrowAction;
}

bool APItem::operator<(const QStandardItem &other) const
{
    APSortInfo thisApInfo = data(SortRole).value<APSortInfo>();
    APSortInfo otherApInfo = other.data(SortRole).value<APSortInfo>();
    bool bRet = thisApInfo < otherApInfo;
    return bRet;
}

bool APItem::setLoading(bool isLoading)
{
    bool isReconnect = false;
    if (m_loadingIndicator.isNull())
        return isReconnect;

    if (m_preLoading == isLoading)
        return isReconnect;

    m_preLoading = isLoading;
    if (isLoading) {
        if (m_parentView) {
            QModelIndex index;
            const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(m_parentView->model());
            if (!deviceModel)
                return isReconnect;

            for (int i = 0; i < m_parentView->count(); ++i) {
                DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(i));
                if (!item)
                    return isReconnect;

                if (this == item) {
                    index = m_parentView->model()->index(i, 0);
                    break;
                }
            }

            QRect itemrect = m_parentView->visualRect(index);
            QPoint point(itemrect.x() + itemrect.width(), itemrect.y());
            m_loadingIndicator->move(point);
        }

        if (!m_arrowAction.isNull())
            m_arrowAction->setVisible(false);

        m_loadingAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignCenter, QSize(), QSize(), false);
        m_loadingAction->setWidget(m_loadingIndicator);
        m_loadingAction->setVisible(true);
        m_loadingIndicator->start();
        m_loadingIndicator->show();
        setActionList(Qt::Edge::RightEdge, {m_loadingAction});
    } else {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        if (!m_loadingAction.isNull())
            m_loadingAction->setVisible(false);

        m_arrowAction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        m_arrowAction->setIcon(m_dStyleHelper.standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        m_arrowAction->setClickAreaMargins(ArrowEnterClickMargin);
        m_arrowAction->setVisible(true);
        setActionList(Qt::Edge::RightEdge, {m_arrowAction});
        isReconnect = true;
    }

    if (m_parentView)
        m_parentView->update();

    return isReconnect;
}

WirelessPage::WirelessPage(WirelessDevice *dev, QWidget *parent)
        : ContentWidget(parent)
        , m_device(dev)
        , m_pNetworkController(NetworkController::instance())
        , m_closeHotspotBtn(new QPushButton)
        , m_lvAP(new DListView(this))
        , m_clickedItem(nullptr)
        , m_modelAP(new QStandardItemModel(m_lvAP))
        , m_sortDelayTimer(new QTimer(this))
        , m_autoConnectHideSsid("")
        , m_wirelessScanTimer(new QTimer(this))
{
    setAccessibleName("WirelessPage");
    qRegisterMetaType<APSortInfo>();
    m_preWifiStatus = Wifi_Unknown;
    m_lvAP->setAccessibleName("List_wirelesslist");
    m_lvAP->setModel(m_modelAP);
    m_lvAP->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvAP->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvAP->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_lvAP->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_lvAP->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lvAP->setSelectionMode(QAbstractItemView::NoSelection);

    QScroller *scroller = QScroller::scroller(m_lvAP->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    m_modelAP->setSortRole(APItem::SortRole);
    m_sortDelayTimer->setInterval(100);
    m_sortDelayTimer->setSingleShot(true);

    APItem *nonbc = new APItem(tr("Connect to hidden network"), style());
    nonbc->setSignalStrength(-1);
    nonbc->setPath("");
    nonbc->setSortInfo({ -1, "", false });
    connect(nonbc->action(), &QAction::triggered, this, [this] { showConnectHidePage(); });
    m_modelAP->appendRow(nonbc);

    //~ contents_path /network/WirelessPage
    QLabel *lblTitle = new QLabel(tr("Wireless Network Adapter"));              // 无线网卡
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_switch = new SwitchWidget(nullptr, lblTitle);
    m_switch->setChecked(dev->isEnabled());
    //因为swtichbutton内部距离右间距为4,所以这里设置6就可以保证间距为10
    m_switch->getMainLayout()->setContentsMargins(10, 0, 6, 0);

    QGSettings *gsettings = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);
    GSettingWatcher::instance()->bind("wireless", m_switch);
    m_lvAP->setVisible(dev->isEnabled() && gsettings->get("wireless").toString() != "Hidden");
    connect(gsettings, &QGSettings::changed, this, [ = ] (const QString &key) {
        if ("wireless" == key) {
            m_lvAP->setVisible(dev->isEnabled() && gsettings->get("wireless").toString() != "Hidden");
            if (gsettings->get("wireless").toString() == "Enabled")
                m_lvAP->setEnabled(true);
            else if(gsettings->get("wireless").toString() == "Disabled")
                m_lvAP->setEnabled(false);
        }
    });

    connect(m_switch, &SwitchWidget::checkedChanged, this, &WirelessPage::onNetworkAdapterChanged);
    connect(m_device, & WirelessDevice::enableChanged, this, [ this ] (const bool enabled) {
        m_switch->setChecked(enabled);
        if (m_lvAP) {
            onAPAdded(m_device->accessPointItems());
            m_lvAP->setVisible(enabled && QGSettings("com.deepin.dde.control-center", QByteArray(), this).get("wireless").toString() != "Hidden");
            updateLayout(!m_lvAP->isHidden());
        }
    });

    m_closeHotspotBtn->setText(tr("Close Hotspot"));

    TipsItem *tips = new TipsItem;
    tips->setText(tr("Disable hotspot first if you want to connect to a wireless network"));

    m_tipsGroup = new SettingsGroup;
    m_tipsGroup->appendItem(tips);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(m_switch, 0, Qt::AlignTop);
    m_mainLayout->addWidget(m_lvAP);
    m_mainLayout->addWidget(m_tipsGroup);
    m_mainLayout->addWidget(m_closeHotspotBtn);
    m_layoutCount = m_mainLayout->layout()->count();
    updateLayout(!m_lvAP->isHidden());
    m_mainLayout->setSpacing(10);//三级菜单控件间的间隙
    m_mainLayout->setMargin(0);
    m_mainLayout->setContentsMargins(QMargins(10, 0, 10, 0));

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_mainLayout);

    setContent(mainWidget);

    setContentsMargins(0, 10, 0, 10);

    connect(m_lvAP, &QListView::clicked, this, [this](const QModelIndex & idx) {
        if (idx.data(APItem::PathRole).toString().length() == 0) {
            this->showConnectHidePage();
            return;
        }

        const QStandardItemModel *deviceModel = qobject_cast<const QStandardItemModel *>(idx.model());
        if (!deviceModel)
            return;

        m_autoConnectHideSsid = "";
        m_clickedItem = dynamic_cast<APItem *>(deviceModel->item(idx.row()));
        if (!m_clickedItem)
            return;

        if (m_clickedItem->isConnected())
            return;

        onApWidgetConnectRequested(idx.data(APItem::PathRole).toString(), idx.data(Qt::ItemDataRole::DisplayRole).toString());
    });

    connect(m_sortDelayTimer, &QTimer::timeout, this, &WirelessPage::sortAPList);
    connect(m_closeHotspotBtn, &QPushButton::clicked, [ = ] {
        m_device->disconnectNetwork();
    });

    connect(m_device, &WirelessDevice::networkAdded, this, &WirelessPage::onAPAdded);
    connect(m_device, &WirelessDevice::networkInfoChanged, this, &WirelessPage::onAPChanged); //需要网络库增加此信号
    connect(m_device, &WirelessDevice::networkRemoved, this, &WirelessPage::onAPRemoved);
    connect(m_device, &WirelessDevice::connectionSuccess, this, &WirelessPage::updateActiveAp);
    connect(m_device, &WirelessDevice::hotspotEnableChanged, this, &WirelessPage::onHotspotEnableChanged);
    connect(m_pNetworkController, &NetworkController::deviceRemoved, this, &WirelessPage::onDeviceRemoved);
    connect(m_device, &WirelessDevice::connectionFailed, this, &WirelessPage::onActivateApFailed);
    connect(m_device, &WirelessDevice::connectionChanged, this, &WirelessPage::updateActiveAp);

    // init data
    const QList<AccessPoints *> lstUAccessPoints = m_device->accessPointItems();
    if (!lstUAccessPoints.isEmpty() && m_device->isEnabled())
        onAPAdded(lstUAccessPoints);

    QGSettings *gsetting = new QGSettings("com.deepin.dde.control-center", QByteArray(), this);
    connect(gsetting, &QGSettings::changed, [&](const QString &key) {
        if (key == "wireless-scan-interval")
            m_wirelessScanTimer->setInterval(gsetting->get("wireless-scan-interval").toInt());
    });

    connect(m_wirelessScanTimer, &QTimer::timeout, this, [ this ] {
        m_device->scanNetwork();
    });

    m_wirelessScanTimer->start(gsetting->get("wireless-scan-interval").toInt() * 1000);

    QTimer::singleShot(100, this, [ = ] {
        m_device->scanNetwork();
    });

    m_lvAP->setVisible(m_switch->checked() && QGSettings("com.deepin.dde.control-center", QByteArray(), this).get("wireless").toString() != "Hidden");
    connect(m_device, &WirelessDevice::enableChanged, this, [ this ] { m_switch->setChecked(m_device->isEnabled()); });
    connect(m_device, &WirelessDevice::deviceStatusChanged, this, &WirelessPage::onDeviceStatusChanged);
    connect(m_device, &WirelessDevice::hotspotEnableChanged, this, &WirelessPage::onHotspotEnableChanged);
    updateLayout(!m_lvAP->isHidden());
    m_switch->setChecked(m_device->isEnabled());
    onDeviceStatusChanged(m_device->deviceStatus());
}

WirelessPage::~WirelessPage()
{
    QScroller *scroller = QScroller::scroller(m_lvAP->viewport());
    if (scroller)
        scroller->stop();

    m_wirelessScanTimer->stop();
}

void WirelessPage::updateLayout(bool enabled)
{
    int layCount = m_mainLayout->layout()->count();
    if (enabled) {
        if (layCount > m_layoutCount) {
            QLayoutItem *layItem = m_mainLayout->takeAt(m_layoutCount);
            if (layItem)
                delete layItem;
        }
    } else if (layCount <= m_layoutCount){
        m_mainLayout->addStretch();
    }

    m_mainLayout->invalidate();
}

void WirelessPage::onDeviceStatusChanged(const DeviceStatus &stat)
{
    //当wifi状态切换的时候，刷新一下列表，防止出现wifi已经连接，三级页面没有刷新出来的情况，和wifi已经断开，但是页面上还是显示该wifi
    //Q_EMIT requestWirelessScan();
    m_device->scanNetwork();

    const bool unavailable = stat <= DeviceStatus::Unavailable;
    if (m_preWifiStatus == Wifi_Unknown)
        m_preWifiStatus = unavailable ? Wifi_Unavailable : Wifi_Available;

    WifiStatus curWifiStatus = unavailable ? Wifi_Unavailable : Wifi_Available;
    if (curWifiStatus != m_preWifiStatus && stat > DeviceStatus::Disconnected) {
        m_switch->setChecked(!unavailable);
        onNetworkAdapterChanged(!unavailable);
        m_preWifiStatus = curWifiStatus;
    }

    if (stat == DeviceStatus::Failed) {
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            if (m_clickedItem == it.value()) {
                it.value()->setLoading(false);
                m_clickedItem = nullptr;
            }
        }
    } else if (DeviceStatus::Prepare <= stat && stat < DeviceStatus::Activated) {
        if (m_device->activeAccessPoints() != nullptr && m_device->isEnabled()) {
            for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
                if (m_device->activeAccessPoints()->ssid() == it.key()) {
                    it.value()->setLoading(true);
                    m_clickedItem = it.value();
                }
            }
        }
    }
}

void WirelessPage::jumpByUuid(const QString &uuid)
{
    if (uuid.isEmpty()) return;

    QTimer::singleShot(50, this, [ = ] {
        if (m_apItems.contains(connectionSsid(uuid)))
            onApWidgetEditRequested("", uuid);
    });
}

void WirelessPage::onNetworkAdapterChanged(bool checked)
{
    m_device->setEnabled(checked);

    if (checked) {
        m_device->scanNetwork();
        onAPAdded(m_device->accessPointItems());
    }

    m_clickedItem = nullptr;
    m_lvAP->setVisible(checked && QGSettings("com.deepin.dde.control-center", QByteArray(), this).get("wireless").toString() != "Hidden");
    updateLayout(!m_lvAP->isHidden());
}

void WirelessPage::onAPAdded(const QList<AccessPoints *> &addedAccessPoints)
{
    for (AccessPoints *ap: addedAccessPoints) {
        const QString &ssid = ap->ssid();
        if (!m_apItems.contains(ssid)) {
            APItem *apItem = new APItem(ssid, style(), m_lvAP);
            m_apItems[ssid] = apItem;
            m_modelAP->appendRow(apItem);
            apItem->setSecure(ap->secured());
            apItem->setPath(ap->path());
            if (ssid == m_autoConnectHideSsid) {
                if (m_clickedItem)
                    m_clickedItem->setLoading(false);

                m_clickedItem = apItem;
            }
            apItem->setConnected(ap->connectionStatus() == ConnectionStatus::Activated);
            apItem->setLoading(ap->connectionStatus() == ConnectionStatus::Activating);
            apItem->setSignalStrength(ap->strength());
            connect(apItem->action(), &QAction::triggered, [ this, apItem ] {
                this->onApWidgetEditRequested(apItem->data(APItem::PathRole).toString(), apItem->data(Qt::ItemDataRole::DisplayRole).toString());
            });

            m_sortDelayTimer->start();
        }
    }
}

void WirelessPage::onAPChanged(const QList<AccessPoints *> &lstChangedAccessPoints)
{
    for (auto ap : lstChangedAccessPoints) {
        if (!m_apItems.contains(ap->ssid())) {
            APItem *apItem = new APItem(ap->ssid(), style(), m_lvAP);
            m_apItems[ap->ssid()] = apItem;
            m_modelAP->appendRow(apItem);
            apItem->setSecure(ap->secured());
            apItem->setPath(ap->path());
            apItem->setConnected(ap->connectionStatus() == ConnectionStatus::Activated);
            apItem->setLoading(ap->connectionStatus() == ConnectionStatus::Activating);
            apItem->setSignalStrength(ap->strength());
            connect(apItem->action(), &QAction::triggered, [ this, apItem ] {
                this->onApWidgetEditRequested(apItem->data(APItem::PathRole).toString(), apItem->data(Qt::ItemDataRole::DisplayRole).toString());
            });
            m_sortDelayTimer->start();
        } else {
            const QString &path = ap->path();
            const int strength = ap->strength();
            const bool isSecure = ap->secured();
            const QString &ssid = ap->ssid();
            APItem *it = m_apItems[ssid];
            if (strength < 5 && !it->checkState() && ap != m_device->activeAccessPoints()) {
                if (nullptr == m_clickedItem || it->uuid() != m_clickedItem->uuid())
                    m_lvAP->setRowHidden(it->row(), true);
            } else {
                m_lvAP->setRowHidden(it->row(), false);
            }

            APSortInfo si{ strength, ssid, ap == m_device->activeAccessPoints() };
            m_apItems[ssid]->setSortInfo(si);

            m_apItems[ssid]->setSignalStrength(strength);
            if (it->path() != path)
                m_apItems[ssid]->setPath(path);

            it->setSecure(isSecure);
            it->setConnected(ap->connectionStatus() == ConnectionStatus::Activated);
            it->setLoading(ap->connectionStatus() == ConnectionStatus::Activating);
            m_sortDelayTimer->start();
        }
    }
}

void WirelessPage::onAPRemoved(const QList<AccessPoints *> &lstRemovedAccessPoints)
{
    for (auto ap: lstRemovedAccessPoints) {
        const QString &ssid = ap->ssid();
        // 如果移除隐藏网络
        if (ssid == m_autoConnectHideSsid)
            m_autoConnectHideSsid = "";

        if (!m_apItems.contains(ssid))
            return;

        const QString &path = ap->path();

        if (m_apItems[ssid]->path() == path) {
            if (m_clickedItem == m_apItems[ssid])
                m_clickedItem = nullptr;

            m_modelAP->removeRow(m_modelAP->indexFromItem(m_apItems[ssid]).row());
            m_apItems.erase(m_apItems.find(ssid));
        }
    }
}

void WirelessPage::onHotspotEnableChanged(const bool enabled)
{
    m_closeHotspotBtn->setVisible(enabled);
    m_tipsGroup->setVisible(enabled);
    m_lvAP->setVisible(!enabled && m_device->isEnabled() && QGSettings("com.deepin.dde.control-center", QByteArray(), this).get("wireless").toString() != "Hidden");
    updateLayout(!m_lvAP->isHidden());
}

void WirelessPage::onCloseHotspotClicked()
{
    m_device->disconnectNetwork();
}

void WirelessPage::onDeviceRemoved()
{
    // back if ap edit page exist
    if (!m_apEditPage.isNull())
        m_apEditPage->onDeviceRemoved();

    //Q_EMIT requestWirelessScan();
    m_device->scanNetwork();
    // destroy self page
    Q_EMIT back();
}

void WirelessPage::onActivateApFailed(const AccessPoints* pAccessPoints)
{
    onApWidgetEditRequested(pAccessPoints->path(), connectionSsid(pAccessPoints->ssid()));
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        if ((it.value()->path() == pAccessPoints->path()) && (it.value()->uuid() == pAccessPoints->ssid())) {
            bool isReconnect = it.value()->setLoading(false);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [ this, it ] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(), it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        }

        it.value()->setConnected(false);
    }
}

void WirelessPage::sortAPList()
{
    m_modelAP->sort(0, Qt::SortOrder::DescendingOrder);
}

void WirelessPage::onApWidgetEditRequested(const QString &apPath, const QString &ssid)
{
    const QString uuid = connectionUuid(ssid);
    if (!m_apEditPage.isNull())
        return;

    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), uuid);

    if (!uuid.isEmpty()) {
        m_editingUuid = uuid;
        m_apEditPage->initSettingsWidget();
    } else {
        m_apEditPage->initSettingsWidgetFromAp(apPath);
    }

    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    connect(m_apEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);
    connect(m_switch, &SwitchWidget::checkedChanged, m_apEditPage, [ = ] (bool checked) {
        if (!checked)
            m_apEditPage->back();
    });

    Q_EMIT requestNextPage(m_apEditPage);
}

void WirelessPage::onApWidgetConnectRequested(const QString &path, const QString &ssid)
{
    Q_UNUSED(path);

    const QString uuid = connectionUuid(ssid);
    // uuid could be empty
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        it.value()->setConnected(false);
        if (m_clickedItem == it.value())
            m_clickedItem->setUuid(uuid);
    }

    if (uuid.isEmpty()) {
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            bool isReconnect = it.value()->setLoading(false);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [ this, it ] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(), it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        }
    } else {
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            bool isReconnect = it.value()->setLoading(it.value() == m_clickedItem);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [ this, it ] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(), it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        }
    }
    if (m_switch && m_switch->checked())
        m_device->connectNetwork(ssid);
}

void WirelessPage::showConnectHidePage()
{
    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), QString(), true);
    m_apEditPage->initSettingsWidget();
    connect(m_apEditPage, &ConnectionEditPage::activateWirelessConnection, this, [ this ] (const QString &ssid, const QString &uuid) {
        Q_UNUSED(uuid);
        m_autoConnectHideSsid = ssid;
    });
    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    connect(m_apEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);
    connect(m_switch, &SwitchWidget::checkedChanged, m_apEditPage, [ = ] (bool checked) {
        if (!checked)
            m_apEditPage->back();
    });

    Q_EMIT requestNextPage(m_apEditPage);
}

void WirelessPage::updateActiveAp()
{
    auto status = m_device->deviceStatus();
    if(!m_device->activeAccessPoints())
        return;

    auto activedSsid = m_device->activeAccessPoints()->ssid();
    bool isWifiConnected = status == DeviceStatus::Activated;
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        bool isConnected = it.key() == activedSsid;
        APSortInfo info = it.value()->sortInfo();
        info.connected = isConnected;
        it.value()->setSortInfo(info);
        if (m_clickedItem == it.value() || (m_clickedItem == nullptr && isConnected)) {
            bool loading = true;
            it.value()->setConnected(false);
            if (status == DeviceStatus::Activated || status == DeviceStatus::Disconnected) {
                loading = false;
                if(status == DeviceStatus::Activated)
                    it.value()->setConnected(true);
            }

            bool isReconnect = it.value()->setLoading(loading);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [ this, it ] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(), it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        } else {
            bool isReconnect = it.value()->setLoading(false);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [ this, it ] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(), it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        }
    }
    if (isWifiConnected && m_clickedItem) {
        bool isReconnect = m_clickedItem->setLoading(false);
        if (isReconnect) {
            connect(m_clickedItem->action(), &QAction::triggered, this, [ this ] {
                this->onApWidgetEditRequested(m_clickedItem->data(APItem::PathRole).toString(), m_clickedItem->data(Qt::ItemDataRole::DisplayRole).toString());
            });
        }
    }

    m_sortDelayTimer->start();
}

QString WirelessPage::connectionUuid(const QString &ssid)
{
    const QList<WirelessConnection *> lstConnections = m_device->items();
    for (auto item : lstConnections) {
        if (item->connection()->ssid() != ssid) continue;

        QString uuid = item->connection()->uuid();
        if (!uuid.isEmpty())
            return uuid;
    }

    return QString();
}

QString WirelessPage::connectionSsid(const QString &uuid)
{
    const QList<WirelessConnection *> wirelessConnections = m_device->items();
    for (WirelessConnection *item : wirelessConnections) {
        if (item->connection()->uuid() != uuid) continue;

        QString ssid = item->connection()->ssid();
        if (!ssid.isEmpty())
            return ssid;
    }

    return QString();
}
