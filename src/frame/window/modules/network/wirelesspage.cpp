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
#include "widgets/titlelabel.h"
#include "window/utils.h"

#include <DStyle>
#include <DStyleHelper>
#include <DDBusSender>

#include <QMap>
#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>
#include <QPointer>
#include <QPushButton>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QThread>
#include <QScroller>
#include <QDBusConnection>

#include <networkmodel.h>
#include <wirelessdevice.h>

DWIDGET_USE_NAMESPACE
using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

APItem::APItem(const QString &text, QStyle *style, DTK_WIDGET_NAMESPACE::DListView *parent)
    : DStandardItem(text)
    , m_parentView(nullptr)
    , m_dStyleHelper(style)
    , m_preLoading(false)
    , m_uuid("")
{
    setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    setCheckable(false);

    m_secureAction = new DViewItemAction(Qt::AlignCenter, QSize(), QSize(), false);
    setActionList(Qt::Edge::LeftEdge, {m_secureAction});

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
    qDebug() << text() << " is destoryed";
    if (!m_loadingIndicator.isNull()) {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        m_loadingIndicator->deleteLater();
    }
}

void APItem::setSecure(bool isSecure)
{
    if (m_secureAction) {
        m_secureAction->setIcon(m_dStyleHelper.standardIcon(isSecure ? DStyle::SP_LockElement : DStyle::SP_CustomBase, nullptr, nullptr));
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
    if (5 >= ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(0)));
    else if (5 < ss && 30 >= ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(2)));
    else if (30 < ss && 55 >= ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(4)));
    else if (55 < ss && 65 >= ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(6)));
    else if (65 < ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(8)));

    APSortInfo si = data(SortRole).value<APSortInfo>();
    si.signalstrength = ss;
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
    if (m_loadingIndicator.isNull()) {
        return isReconnect;
    }
    if (m_preLoading == isLoading) {
        return isReconnect;
    } else {
        m_preLoading = isLoading;
    }
    if (isLoading) {
        if (m_parentView) {
            QModelIndex index;
            const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(m_parentView->model());
            if (!deviceModel) {
                return isReconnect;
            }
            for (int i = 0; i < m_parentView->count(); ++i) {
                DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(i));
                if (!item) {
                    return isReconnect;
                }
                if (this == item) {
                    index = m_parentView->model()->index(i, 0);
                    break;
                }
            }
            QRect itemrect = m_parentView->visualRect(index);
            QPoint point(itemrect.x() + itemrect.width(), itemrect.y());
            m_loadingIndicator->move(point);
        }
        if (!m_arrowAction.isNull()) {
            m_arrowAction->setVisible(false);
        }
        m_loadingAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignCenter, QSize(), QSize(), false);
        m_loadingAction->setWidget(m_loadingIndicator);
        m_loadingAction->setVisible(true);
        m_loadingIndicator->start();
        m_loadingIndicator->show();
        setActionList(Qt::Edge::RightEdge, {m_loadingAction});
    } else {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        if (!m_loadingAction.isNull()) {
            m_loadingAction->setVisible(false);
        }
        m_arrowAction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        m_arrowAction->setIcon(m_dStyleHelper.standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        m_arrowAction->setClickAreaMargins(ArrowEnterClickMargin);
        m_arrowAction->setVisible(true);
        setActionList(Qt::Edge::RightEdge, {m_arrowAction});
        isReconnect = true;
    }
    if (m_parentView) {
        m_parentView->update();
    }
    return isReconnect;
}

WirelessPage::WirelessPage(WirelessDevice *dev, QWidget *parent)
    : ContentWidget(parent)
    , m_device(dev)
    , m_tipsGroup(new SettingsGroup)
    , m_closeHotspotBtn(new QPushButton)
    , m_lvAP(new DListView(this))
    , m_clickedItem(nullptr)
    , m_modelAP(new QStandardItemModel(m_lvAP))
    , m_sortDelayTimer(new QTimer(this))
    , m_airplaninter(new AirplanInter("com.deepin.daemon.AirplaneMode","/com/deepin/daemon/AirplaneMode",QDBusConnection::systemBus(),this))
{
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
    m_lvAP->setViewportMargins(0, 0, 7, 0);

    QScroller::grabGesture(m_lvAP->viewport(), QScroller::LeftMouseButtonGesture);
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
    nonbc->setSortInfo({-1, "", false});
    connect(nonbc->action(), &QAction::triggered, this, [this] { showConnectHidePage(); });
    m_modelAP->appendRow(nonbc);

    //~ contents_path /network/WirelessPage
    //初始化的时候判断一下飞行模式是否打开
    bool isAirplan = m_airplaninter->wifiEnabled();
    m_lvAP->setVisible(dev->enabled() && isAirplan);

    //关联信号和槽,防止信息不同步
    connect(m_airplaninter, &AirplanInter::WifiEnabledChanged,this, [=](const bool enabled){
            m_switch->setChecked(dev->enabled() && enabled);
    });

    TitleLabel *lblTitle = new TitleLabel(tr("Wireless Network Adapter"));//无线网卡
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_switch = new SwitchWidget(nullptr, lblTitle);
    m_switch->setChecked(dev->enabled() && isAirplan);

    connect(m_switch, &SwitchWidget::checkedChanged, this, &WirelessPage::onNetworkAdapterChanged);
    connect(m_device, &NetworkDevice::enableChanged, this, [this](const bool enabled) {
        m_switch->blockSignals(true);
        m_switch->setChecked(enabled);
        m_switch->blockSignals(false);

        if (m_lvAP) {
            m_lvAP->setVisible(enabled);
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
    //m_mainLayout->addWidget(m_tipsGroup);
    m_mainLayout->addWidget(m_closeHotspotBtn);
    m_layoutCount = m_mainLayout->layout()->count();
    updateLayout(!m_lvAP->isHidden());
    m_mainLayout->setSpacing(10);//三级菜单控件间的间隙
    m_mainLayout->setMargin(0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_mainLayout);

    setContent(mainWidget);

    connect(m_lvAP, &QListView::clicked, this, [this](const QModelIndex & idx) {
        if (idx.data(APItem::PathRole).toString().length() == 0) {
            this->showConnectHidePage();
            return;
        }
        const QStandardItemModel *deviceModel = qobject_cast<const QStandardItemModel *>(idx.model());
        if (!deviceModel) {
            return;
        }
        m_clickedItem = dynamic_cast<APItem *>(deviceModel->item(idx.row()));
        if (!m_clickedItem) {
            qDebug() << "clicked item is nullptr";
            return;
        }
        if (m_clickedItem->isConnected()) {
            return;
        }
        qDebug() << "clicked item " << m_clickedItem->text();
        this->onApWidgetConnectRequested(idx.data(APItem::PathRole).toString(),
                                         idx.data(Qt::ItemDataRole::DisplayRole).toString());
    });

    connect(m_sortDelayTimer, &QTimer::timeout, this, &WirelessPage::sortAPList);
    connect(m_closeHotspotBtn, &QPushButton::clicked, this, &WirelessPage::onCloseHotspotClicked);
    connect(m_device, &WirelessDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(m_device, &WirelessDevice::apInfoChanged, this, &WirelessPage::onAPChanged);
    connect(m_device, &WirelessDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    connect(m_device, &WirelessDevice::activeApInfoChanged, this, &WirelessPage::updateActiveAp);
    connect(m_device, &WirelessDevice::hotspotEnabledChanged, this, &WirelessPage::onHotspotEnableChanged);
    connect(m_device, &WirelessDevice::removed, this, &WirelessPage::onDeviceRemoved);
    connect(m_device, &WirelessDevice::activateAccessPointFailed, this, &WirelessPage::onActivateApFailed);
    connect(m_device, &WirelessDevice::activeWirelessConnectionInfoChanged, this, &WirelessPage::updateActiveAp);
    connect(m_device, &WirelessDevice::activeConnectionsChanged, this, [ = ] {
        //主动或被动连接ap成功后，扫描一次wifi，刷新列表
        //接收信号时获取的activeApSsid()还是上一个ssid，延迟500ms待数据同步当前连接的ssid，再判断
        QTimer::singleShot(500, this, [ = ] {
            if (!m_device->activeApSsid().isEmpty() && m_device->status() == NetworkDevice::Activated) {

                Q_EMIT requestDeviceAPList(m_device->path());
                Q_EMIT requestWirelessScan();
            }
        });
    });
    // task 34726 频繁切换连接后依据最后更改的热点连接状态，刷新列表显示状态
    connect(m_device, &WirelessDevice::activeConnectionsChanged, this, [ = ] {
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            it.value()->setLoading(it.value() == m_clickedItem);
        }
    });



    // init data
    const QJsonArray mApList = m_device->apList();
    if (!mApList.isEmpty()) {
        for (auto ap : mApList) {
            onAPAdded(ap.toObject());
            onAPChanged(ap.toObject());
        }
    }

    m_preActiveSsid = m_device->activeApSsid();
    Q_EMIT requestDeviceAPList(m_device->path());
    Q_EMIT requestWirelessScan();
}

WirelessPage::~WirelessPage()
{
    QScroller *scroller = QScroller::scroller(m_lvAP->viewport());
    if (scroller) {
        scroller->stop();
    }
}

void WirelessPage::updateLayout(bool enabled)
{
    int layCount = m_mainLayout->layout()->count();
    if (enabled) {
        if (layCount > m_layoutCount) {
            QLayoutItem *layItem = m_mainLayout->takeAt(m_layoutCount);
            if (layItem) {
                delete layItem;
            }
        }
    } else {
        if (layCount <= m_layoutCount) {
            m_mainLayout->addStretch();
        }
    }
    m_mainLayout->invalidate();
}

int WirelessPage::canUpdateApList() {
    //验证密码状态/正在连接Ap中的状态时禁止刷新ap列表
    int wdevStu = m_device->status();
    if (wdevStu >= NetworkDevice::Config
            && wdevStu <= NetworkDevice::IpCheck) {

        return false;
    }
    return true;
}

void WirelessPage::onDeviceStatusChanged(const dde::network::WirelessDevice::DeviceStatus stat)
{
    bool unavailable = stat <= NetworkDevice::Unavailable;
    if (m_preWifiStatus == Wifi_Unknown) {
        m_preWifiStatus = unavailable ? Wifi_Unavailable : Wifi_Available;
    }
    WifiStatus curWifiStatus = unavailable ? Wifi_Unavailable : Wifi_Available;
    if (curWifiStatus != m_preWifiStatus && stat > NetworkDevice::Disconnected) {
        m_switch->setChecked(!unavailable);
        onNetworkAdapterChanged(!unavailable);
        m_preWifiStatus = curWifiStatus;
    }
    if (stat == WirelessDevice::Failed) {
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            if (m_clickedItem == it.value()) {
                it.value()->setLoading(false);
                m_clickedItem = nullptr;
            }
        }
    } else if (WirelessDevice::Prepare <= stat && stat < WirelessDevice::Activated) {
        for (auto ls = m_device->activeConnections().cbegin(); ls != m_device->activeConnections().cend(); ++ls) {
            for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
                if (ls->value("Id").toString() == it.key()) {
                    it.value()->setLoading(true);
                    m_clickedItem = it.value();
                }
            }
        }
    }
}

void WirelessPage::setModel(NetworkModel *model)
{
    m_model = model;
    m_lvAP->setVisible(m_switch->checked());
    connect(m_model, &NetworkModel::deviceEnableChanged, this, [=](){
            bool isAirplan = m_airplaninter->wifiEnabled();
            m_switch->setChecked(m_device->enabled() && isAirplan);
            m_lvAP->setVisible(m_device->enabled() && isAirplan);
            updateLayout(!m_lvAP->isHidden());
   	});

    connect(m_device,
            static_cast<void (WirelessDevice::*)(WirelessDevice::DeviceStatus) const>(&WirelessDevice::statusChanged),
            this,
            &WirelessPage::onDeviceStatusChanged);
    onHotspotEnableChanged(m_device->hotspotEnabled());
    updateLayout(!m_lvAP->isHidden());
    m_switch->setChecked(m_device->enabled());
    onDeviceStatusChanged(m_device->status());
}

void WirelessPage::jumpByUuid(const QString &uuid)
{
    if (uuid.isEmpty()) return;

    QTimer::singleShot(50, this, [ = ] {
        if (m_apItems.contains(connectionSsid(uuid)))
        {
            onApWidgetEditRequested("", uuid);
        }
    });
}

void WirelessPage::onNetworkAdapterChanged(bool checked)
{
    Q_EMIT requestDeviceEnabled(m_device->path(), checked);

    if (checked) {
        Q_EMIT requestDeviceAPList(m_device->path());
        Q_EMIT requestWirelessScan();
    }

    m_clickedItem = nullptr;
    m_lvAP->setVisible(checked);
    updateLayout(!m_lvAP->isHidden());

    if (!m_apEditPage.isNull() && !checked) {
        Q_EMIT m_apEditPage->back();
    }
}

void WirelessPage::onAPAdded(const QJsonObject &apInfo)
{
    if (!canUpdateApList())
        return;

    const QString &ssid = apInfo.value("Ssid").toString();

    if (!m_apItems.contains(ssid)) {
        APItem *apItem = new APItem(ssid, style(), m_lvAP);
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
        m_sortDelayTimer->start();
    }
}

void WirelessPage::onAPChanged(const QJsonObject &apInfo)
{
    if (!canUpdateApList())
        return;

    const QString &ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid)) return;

    const QString &path = apInfo.value("Path").toString();
    const int strength = apInfo.value("Strength").toInt();
    const bool isSecure = apInfo.value("Secured").toBool();

    APItem *it = m_apItems[ssid];
    if (5 >= strength && !it->checkState() && ssid != m_device->activeApSsid()) {
        if (nullptr == m_clickedItem) {
            m_lvAP->setRowHidden(it->row(), true);
        } else if (it->uuid() != m_clickedItem->uuid()) {
            m_lvAP->setRowHidden(it->row(), true);
        }
    } else {
        m_lvAP->setRowHidden(it->row(), false);
    }

    APSortInfo si{strength, ssid, ssid == m_device->activeApSsid()};
    m_apItems[ssid]->setSortInfo(si);

    m_apItems[ssid]->setSignalStrength(strength);
    if (it->path() != path) {
        m_apItems[ssid]->setPath(path);
    }
    it->setSecure(isSecure);
    m_sortDelayTimer->start();
}

void WirelessPage::onAPRemoved(const QJsonObject &apInfo)
{
    if (!canUpdateApList())
        return;

    const QString &ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid)) return;

    const QString &path = apInfo.value("Path").toString();

    if (m_apItems[ssid]->path() == path) {
        if (m_clickedItem == m_apItems[ssid]) {
            m_clickedItem = nullptr;
            qDebug() << "remove clicked item," << QThread::currentThreadId();
        }
        m_modelAP->removeRow(m_modelAP->indexFromItem(m_apItems[ssid]).row());
        m_apItems.erase(m_apItems.find(ssid));
    }

    //已连接wifi状态下，network后端防止数据过期，数据不同步。在连接上wifi后过一段时间会清空ap列表,发出清空信号。(正常处理)
    //前端现象为wifi列表上过一段时间后只显示一个已连接ap。
    //当前端wifi列表为该状态下时，重新扫描一次。
    if (m_apItems.count()==1
            && m_apItems.contains(m_device->activeWirelessConnName())) {
        qDebug() << "Only one activedAp && requestWirelessScan";
        Q_EMIT requestWirelessScan();
    }
}

void WirelessPage::onHotspotEnableChanged(const bool enabled)
{
    m_closeHotspotBtn->setVisible(enabled);
    m_tipsGroup->setVisible(enabled);
    m_lvAP->setVisible(!enabled && m_device->enabled());
    updateLayout(!m_lvAP->isHidden());
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
    Q_EMIT requestDeviceAPList(m_device->path());
    Q_EMIT requestWirelessScan();
    // destroy self page
    Q_EMIT back();
}

void WirelessPage::onActivateApFailed(const QString &apPath, const QString &uuid)
{
    Q_UNUSED(uuid);
    onApWidgetEditRequested(apPath, connectionSsid(uuid));
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        if ((it.value()->path() == apPath) && (it.value()->uuid() == uuid)) {
            bool isReconnect = it.value()->setLoading(false);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [this, it] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(),
                                                  it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
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
    if (!m_apEditPage.isNull()) {
        return;
    }

    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), uuid);

    if (!uuid.isEmpty()) {
        m_editingUuid = uuid;
        m_apEditPage->initSettingsWidget();
    } else {
        m_apEditPage->initSettingsWidgetFromAp(apPath);
    }

    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    connect(m_apEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);

    connect(m_apEditPage, &ConnectionEditPage::requestUpdateLoader, this, [=](QString &uuid) {
        //通过connectioneditpage窗口编辑后发起连接wifi时，更新显示列表上的wifi连接状态图
        if (uuid.isEmpty())
            return;

        QString connSSid = connectionSsid(uuid);
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            if (connSSid == it.key()) {
                m_clickedItem = it.value();
                QJsonObject nilInfo;
                updateActiveAp(nilInfo);
                break;
            }
        }
    });

    Q_EMIT requestNextPage(m_apEditPage);
}

void WirelessPage::onApWidgetConnectRequested(const QString &path, const QString &ssid)
{
    const QString uuid = connectionUuid(ssid);
    // uuid could be empty
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        it.value()->setConnected(false);
        if (m_clickedItem == it.value()) {
            m_clickedItem->setUuid(uuid);
        }
    }
    if (uuid.isEmpty()) {
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            bool isReconnect = it.value()->setLoading(false);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [this, it] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(),
                                                  it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        }
    } else {
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            bool isReconnect = it.value()->setLoading(it.value() == m_clickedItem);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [this, it] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(),
                                                  it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        }
    }
    if (m_switch && m_switch->checked()) {
        Q_EMIT requestConnectAp(m_device->path(), path, uuid);
    }
}

void WirelessPage::showConnectHidePage()
{
    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), QString(), true);
    m_apEditPage->initSettingsWidget();
    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    connect(m_apEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_apEditPage);
}

void WirelessPage::updateActiveAp(const QJsonObject &activeApInfo)
{
    qDebug() << "updateActiveAp:" << QThread::currentThreadId();
    auto status = m_device->status();
    auto activedSsid = m_device->activeApSsid();
    bool isWifiConnected = status == NetworkDevice::Activated;
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        bool isConnected = it.key() == activedSsid;
        it.value()->setConnected(isConnected);
        APSortInfo info = it.value()->sortInfo();
        info.connected = isConnected;
        it.value()->setSortInfo(info);

        if (m_clickedItem == it.value()) {
            qDebug() << "click item: " << isConnected;
            bool loading = true;
            if (status == NetworkDevice::Activated || status == NetworkDevice::Disconnected) {
                loading = false;
            }
            bool isReconnect = it.value()->setLoading(loading);
            if (isReconnect) {
                connect(it.value()->action(), &QAction::triggered, this, [this, it] {
                    this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(),
                                                  it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
                });
            }
        } else {
            connect(it.value()->action(), &QAction::triggered, this, [this, it] {
                this->onApWidgetEditRequested(it.value()->data(APItem::PathRole).toString(),
                                              it.value()->data(Qt::ItemDataRole::DisplayRole).toString());
            });
        }
    }
    if (isWifiConnected && m_clickedItem) {
        bool isReconnect = m_clickedItem->setLoading(false);
        if (isReconnect) {
            connect(m_clickedItem->action(), &QAction::triggered, this, [this] {
                this->onApWidgetEditRequested(m_clickedItem->data(APItem::PathRole).toString(),
                                              m_clickedItem->data(Qt::ItemDataRole::DisplayRole).toString());
            });
        }
    }

    QString activessid = activeApInfo.value("Ssid").toString();
    if (!activessid.isEmpty()) {
        if (m_preActiveSsid != activessid) {
            m_preActiveSsid = activessid;
            m_sortDelayTimer->start();
        }
    }
}

QString WirelessPage::connectionUuid(const QString &ssid)
{
    QString uuid;
    QList<QJsonObject> connections = m_device->connections();
    for (auto item : connections) {
        if (item.value("Ssid").toString() != ssid) continue;
        uuid = item.value("Uuid").toString();
        if (!uuid.isEmpty()) break;
    }
    return uuid;
}

QString WirelessPage::connectionSsid(const QString &uuid)
{
    QString ssid;

    QList<QJsonObject> connections = m_device->connections();
    for (auto item : connections) {
        if (item.value("Uuid").toString() != uuid) continue;

        ssid = item.value("Ssid").toString();
        if (!ssid.isEmpty()) break;
    }

    return ssid;
}
