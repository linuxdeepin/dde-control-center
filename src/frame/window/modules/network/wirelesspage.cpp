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
#include <QTime>
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
    , m_dStyleHelper(style)
    , m_preLoading(false)
    , m_uuid("")
{
    setFlags(Qt::ItemFlag::ItemIsEnabled | Qt::ItemFlag::ItemIsSelectable);
    setCheckable(false);

    m_secureAction = new DViewItemAction(Qt::AlignCenter, QSize(), QSize(), false);
    setActionList(Qt::Edge::LeftEdge, {m_secureAction});

    if (parent != nullptr) {
        //连接动图---那个三个小球转转转的图标
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
    Q_EMIT apChange();
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
    //该区间是根据需求文档中规定的
    if (65 < ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(8)));
    else if (55 < ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(6)));
    else if (30 < ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(4)));
    else if (5 < ss)
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(2)));
    else
        setIcon(QIcon::fromTheme(QString("dcc_wireless-%1").arg(0)));

    APSortInfo si = data(SortRole).value<APSortInfo>();
    si.signalstrength = ss;
    si.ssid = text();
    setData(QVariant::fromValue(si), SortRole);
    Q_EMIT apChange();
}

int APItem::signalStrength() const
{
    return data(SortRole).value<APSortInfo>().signalstrength;
}

void APItem::setConnecting(bool connecting)
{
    APSortInfo si = data(SortRole).value<APSortInfo>();
    si.connecting = connecting;
    setData(QVariant::fromValue(si), SortRole);
    Q_EMIT apChange();
}

bool APItem::isConnecting()
{
    return data(SortRole).value<APSortInfo>().connecting;
}

void APItem::setConnected(bool connected)
{
    setCheckState(connected ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    APSortInfo si = data(SortRole).value<APSortInfo>();
    si.connected = connected;
    setData(QVariant::fromValue(si), SortRole);
    Q_EMIT apChange();
}

bool APItem::isConnected()
{
    return checkState();
}

void APItem::setSortInfo(const APSortInfo &si)
{
    setData(QVariant::fromValue(si), SortRole);
    Q_EMIT apChange();
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

QString APItem::ssid() const
{
    return data(SortRole).value<APSortInfo>().ssid;
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
    APSortInfo  thisApInfo = data(SortRole).value<APSortInfo>();
    APSortInfo otherApInfo = other.data(SortRole).value<APSortInfo>();
    bool bRet = thisApInfo < otherApInfo;
    return bRet;
}

bool APItem::setLoading(bool isLoading)
{
    bool isReconnect = false;
    //连接小图标有实例
    if (m_loadingIndicator.isNull()) {
        return isReconnect;
    }
    //前一个状态是否当前状态
    if (m_preLoading == isLoading) {
        return isReconnect;
    } else {
        m_preLoading = isLoading;
    }
    //判断是否需要处于连接状态
    if (isLoading) {
        //隐藏详情页箭头
        if (!m_arrowAction.isNull()) {
            m_arrowAction->setVisible(false);
        }
        m_loadingAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignCenter, QSize(), QSize(), false);
        m_loadingAction->setWidget(m_loadingIndicator);
        m_loadingAction->setVisible(true);
        m_loadingIndicator->start();
        m_loadingIndicator->show();
        //设置ActionList会将原来旧的DViewItemAction释放掉，所以这里会出现智能指针，且一直在new的情况
        setActionList(Qt::Edge::RightEdge, {m_loadingAction});
    } else {
        //停止并隐藏动图
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
        Q_EMIT ActionChange(this);
        setActionList(Qt::Edge::RightEdge, {m_arrowAction});
        isReconnect = true;
    }
    //告知列表状态改变，准备刷新
    Q_EMIT apChange();
    return isReconnect;
}

WirelessPage::WirelessPage(WirelessDevice *dev, QWidget *parent)
    : ContentWidget(parent)
    , m_device(dev)
    , m_tipsGroup(new SettingsGroup)
    , m_closeHotspotBtn(new QPushButton)
    , m_lvAP(new DListView(this))
    , m_activateItem(nullptr)
    , m_modelAP(new QStandardItemModel(m_lvAP))
    , m_sortDelayTimer(new QTimer(this))
    , m_switchEnableTimer(new QTimer(this))
{
    initUI();
    qRegisterMetaType<APSortInfo>();
    //排序定时器
    m_sortDelayTimer->setInterval(100);
    m_sortDelayTimer->setSingleShot(true);
    //wifi开关禁用定时器
    m_switchEnableTimer->setInterval(500);
    m_switchEnableTimer->setSingleShot(true);
    initConnect();
}



void WirelessPage::initUI()
{
    //无线网卡页的标题栏
    TitleLabel *lblTitle = new TitleLabel(tr("Wireless Network Adapter"));//无线网卡
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_switch = new SwitchWidget(nullptr, lblTitle);

    //网络详情页
    m_lvAP->setAccessibleName("List_wirelesslist");
    m_lvAP->setModel(m_modelAP);
    m_lvAP->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvAP->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvAP->setSizeAdjustPolicy(QAbstractScrollArea::AdjustToContents);
    m_lvAP->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_lvAP->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_lvAP->setSelectionMode(QAbstractItemView::NoSelection);
    m_lvAP->setViewportMargins(0, 0, 7, 0);

    //设置滚动条
    QScroller::grabGesture(m_lvAP->viewport(), QScroller::LeftMouseButtonGesture);
    QScroller *scroller = QScroller::scroller(m_lvAP->viewport());
    QScrollerProperties sp;
    sp.setScrollMetric(QScrollerProperties::VerticalOvershootPolicy, QScrollerProperties::OvershootAlwaysOff);
    scroller->setScrollerProperties(sp);

    m_modelAP->setSortRole(APItem::SortRole);
    addHideWifiButton();
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
    m_mainLayout->setSpacing(10);//三级菜单控件间的间隙
    m_mainLayout->setMargin(0);

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(m_mainLayout);
    //获取当前开关状态
    onSwitch(m_device->enabled());
    setContent(mainWidget);
}

void WirelessPage::initConnect()
{
    //禁用wifi开关500毫秒
    connect(m_switchEnableTimer, &QTimer::timeout, this, &WirelessPage::onSwitchEnable);

    //延迟100毫秒刷新wifi列表
    connect(m_sortDelayTimer, &QTimer::timeout, this, &WirelessPage::sortAPList);

    //切换开关
    connect(m_switch, &SwitchWidget::checkedChanged, this, &WirelessPage::onNetworkAdapterChanged);
    //监听后端信号改变
    connect(m_device, &WirelessDevice::enableChanged, this, [=](){
        qDebug() << "Device change signal from daemon, state:" << m_device->enabled();
        onSwitch(m_device->enabled());
        //开关的时候做个清空操作，防止出现脏数据
        m_apItems.clear();
        m_modelAP->clear();
        addHideWifiButton();
        if (m_device->enabled())
            //这里会在页面创建的时候去初始化一次，所以无需在构造函数中再调用
            m_device->initWirelessData();
    });
    //更改wifi列表信息
    connect(m_device, &WirelessDevice::apAdded, this, &WirelessPage::onAPAdded);
    connect(m_device, &WirelessDevice::apRemoved, this, &WirelessPage::onAPRemoved);
    connect(m_device, &WirelessDevice::apInfoChanged, this, &WirelessPage::onAPChanged);

    //点击wifi进行连接
    connect(m_lvAP, &QListView::clicked, this, &WirelessPage::onClickApItem);

    //热点功能相关的信号槽，目前还没做
    connect(m_device, &WirelessDevice::hotspotEnabledChanged, this, &WirelessPage::onHotspotEnableChanged);
    connect(m_closeHotspotBtn, &QPushButton::clicked, this, &WirelessPage::onCloseHotspotClicked);

    //活动状态的槽
    connect(m_device, &WirelessDevice::activeConnectionsChanged, this, &WirelessPage::onActivaConnections);
    //当网络需要密码或者是企业wifi的时候，要先去后端判断
    connect(m_device, &WirelessDevice::activateAccessPointFailed, this, &WirelessPage::onActivateApFailed);
}

void WirelessPage::addHideWifiButton()
{
    //创建隐藏wifi连接选项
    APItem *nonbc = new APItem(tr("Connect to hidden network"), style());
    nonbc->setSignalStrength(-1);
    nonbc->setPath("");
    nonbc->setSortInfo({-1, "", false});
    connect(nonbc->action(), &QAction::triggered, this, [this] { showConnectHidePage(); });
    m_modelAP->appendRow(nonbc);

}

WirelessPage::~WirelessPage()
{
    QScroller *scroller = QScroller::scroller(m_lvAP->viewport());
    if (scroller) {
        scroller->stop();
    }
    qDebug() << Q_FUNC_INFO;
}

void WirelessPage::onSwitchEnable()
{
    m_switch->switchButton()->setEnabled(true);
}

void WirelessPage::onSwitch(bool enable)
{
    m_switch->setChecked(enable);
    m_lvAP->setVisible(enable);
    updateLayout(!m_lvAP->isHidden());
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

bool WirelessPage::setApLoadin(APItem *ApItem)
{
    bool isLoad = true;
    for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
        if (ApItem == it.value()) {
            it.value()->setConnecting(true);
            isLoad = it.value()->setLoading(true);
            m_activateItem = ApItem;
        } else {
            it.value()->setConnecting(false);
            it.value()->setLoading(false);
        }
    }
    //由于当ApItem = nullptr时候，则ApItem == it.value()无法进去，
    //所以需要在全部设置false后再赋值给
    if (ApItem == nullptr) {
        m_activateItem = nullptr;
    }
    return isLoad;
}

void WirelessPage::setModel(NetworkModel *model)
{
    m_model = model;
    m_lvAP->setVisible(m_switch->checked());

    //更新一下wifi数据
    m_device->initWirelessData();
    //开启之后刷新wifi数据，防止数据太旧了
    Q_EMIT m_model->updateApList();
    //更新一下当前网络状态是否正确
    Q_EMIT m_model->initDeviceEnable(m_device->path());

    //以下两个是根据热点功能进行刷新，暂时没有热点功能
    onHotspotEnableChanged(m_device->hotspotEnabled());
    updateLayout(!m_lvAP->isHidden());
    //onDeviceStatusChanged(m_device->status());
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
    qDebug() <<"click enable , set Enable =" << checked;
    //网卡开关
    Q_EMIT m_model->requestDeviceEnable(m_device->path(), checked);
    onSwitch(checked);
    //开启的时候将开关禁用500毫秒，为了防止用户疯狂操作，导致wifi状态切换频繁
    m_switch->switchButton()->setEnabled(false);
    m_switchEnableTimer->start();
    //将点击的wifi变成空
    m_activateItem = nullptr;
    m_clickItem = nullptr;

    if (!m_apEditPage.isNull() && !checked) {
        Q_EMIT m_apEditPage->back();
    }
}

void WirelessPage::onAPAdded(const QJsonObject &apInfo)
{
    //确定存在ssid
    const QString &ssid = apInfo.value("Ssid").toString("");
    const int strength = apInfo.value("Strength").toInt(0);
    //这里防止加入的wifi为空白名称，产生异常
    if (m_apItems.keys().contains(ssid) || ssid.isEmpty()) return;
    if (strength < 5) return;
    APItem *apItem = new APItem(ssid, style(), m_lvAP);
    connect(apItem, &APItem::apChange,
            this, [=](){if (!m_sortDelayTimer->isActive())
                            m_sortDelayTimer->start();});
    m_apItems[ssid] = apItem;
    m_modelAP->appendRow(apItem);
    apItem->setSecure(apInfo.value("Secured").toBool());
    apItem->setPath(apInfo.value("Path").toString());
    //这里需要判断当前的是否处于连接成功的状态，防止刚打开出现连接成功和连接中的图标
    apItem->setConnected(ssid == m_device->activeApSsid()
                         && m_device->activeApState() == Wifi_Connected);
    apItem->setUuid(apInfo.value("Uuid").toString());
    apItem->setSignalStrength(strength);
    //连接详情页
    updateAction(apItem);
    connect(apItem, &APItem::ActionChange, this, &WirelessPage::updateAction);

}

void WirelessPage::onAPChanged(const QJsonObject &apInfo)
{
    const QString &ssid = apInfo.value("Ssid").toString();
    const QString &path = apInfo.value("Path").toString();
    const int strength = apInfo.value("Strength").toInt();
    const bool isSecure = apInfo.value("Secured").toBool();
    const QString &uuid = apInfo.value("Uuid").toString();

    //当wifi之前小于5且更新后还小于5则直接跳过
    if (!m_apItems.contains(ssid) && strength < 5) return;
    //当wifi之前小于5但是更新后大于5则添加该网络到列表
    else if (!m_apItems.contains(ssid) && strength > 5) {
        onAPAdded(apInfo);
        return;
    //当wifi大于5更新到小于5则删除该网络
    } else if (m_apItems.contains(ssid) && strength < 5) {
        onAPRemoved(apInfo);
        return;
    }

    //其他情况正常更新
    APItem *it = m_apItems[ssid];
    if (5 >= strength && !it->checkState() && ssid != m_device->activeApSsid()) {
        if (nullptr == m_activateItem) {
            m_lvAP->setRowHidden(it->row(), true);
        } else if (it->uuid() != m_activateItem->uuid()) {
            m_lvAP->setRowHidden(it->row(), true);
        }
    } else {
        m_lvAP->setRowHidden(it->row(), false);
    }
    m_apItems[ssid]->setSignalStrength(strength);
    m_apItems[ssid]->setConnected(ssid == m_device->activeApSsid() && m_device->activeApState() == Wifi_Connected);
    m_apItems[ssid]->setUuid(uuid);
    if (it->path() != path) {
        m_apItems[ssid]->setPath(path);
    }
    it->setSecure(isSecure);
}

void WirelessPage::onAPRemoved(const QJsonObject &apInfo)
{
    const QString &ssid = apInfo.value("Ssid").toString();
    if (!m_apItems.contains(ssid)) return;
    if (m_activateItem == m_apItems[ssid]) {
        m_activateItem = nullptr;
        qDebug() << "remove clicked item," << QThread::currentThreadId();
    }
    m_modelAP->removeRow(m_modelAP->indexFromItem(m_apItems[ssid]).row());
    m_apItems.erase(m_apItems.find(ssid));
    //删除后刷新一下wifi列表
    m_sortDelayTimer->start();
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
    //当有其他的网络接口的时候，删除一个则其他的刷新一下网络
    Q_EMIT requestWirelessScan();
    // destroy self page
    Q_EMIT back();
}

void WirelessPage::onActivateApFailed(const QString &apPath, const QString &uuid)
{
    Q_UNUSED(uuid);
    onApWidgetEditRequested(apPath, connectionSsid(uuid));
}

void WirelessPage::sortAPList()
{
    m_modelAP->sort(0, Qt::SortOrder::DescendingOrder);
}

void WirelessPage::onApWidgetEditRequested(const QString &apPath, const QString &ssid)
{
    QString uuid = connectionUuid(ssid);
    if (!m_apEditPage.isNull()) {
        qDebug() << "apEditPage not Empty";
        return;
    }
    //由于数据一分钟更新一次，所以在快速删除后点击wifi详情页，会出现打开内容为空白的情况，
    //所以要用静态函数先去networkManger中判断当前的uuid是否还有用，再去创建详情页
    if (ConnectionWirelessEditPage::uuidIsEmpty(uuid))
        uuid = QString("");
    m_apEditPage = new ConnectionWirelessEditPage(m_device->path(), uuid);
    if (!uuid.isEmpty()) {
        m_apEditPage->initSettingsWidget();
    }  else {
        qDebug() << "uuid is Empty";
        m_apEditPage->initSettingsWidgetFromAp(apPath);
    }

    connect(m_apEditPage, &ConnectionEditPage::requestNextPage, this, &WirelessPage::requestNextPage);
    connect(m_apEditPage, &ConnectionEditPage::requestFrameAutoHide, this, &WirelessPage::requestFrameKeepAutoHide);

    connect(m_apEditPage, &ConnectionEditPage::requestUpdateLoader, this, [=](QString &uuid) {
        //通过connectioneditpage窗口编辑后发起连接wifi时，更新显示列表上的wifi连接状态图
        if (uuid.isEmpty())
            return;

        QString connSSid = connectionSsid(uuid);
        //考虑到安全红线问题，个人信息和敏感数据禁止打印
#ifdef QT_DEBUG
        qDebug() << "connSSid = " << connSSid;
#endif
        for (auto it = m_apItems.cbegin(); it != m_apItems.cend(); ++it) {
            if (connSSid == it.key()) {
                m_clickItem = it.value();
                Q_EMIT m_model->requestConnectAp(m_device->path(), m_clickItem->path(), m_clickItem->uuid());
                setApLoadin(m_clickItem);
                break;
            }
        }

    });
    connect(m_apEditPage, &ConnectionEditPage::deleteConnectAP, m_model, &NetworkModel::deleteConnection);
    connect(m_apEditPage, &ConnectionEditPage::disconnectAP, m_model, &NetworkModel::requestDisconnctAP);

    Q_EMIT requestNextPage(m_apEditPage);
}

void WirelessPage::onApWidgetConnectRequested(const QString &path, const QString &ssid)
{
    const QString uuid = connectionUuid(ssid);
    if (m_switch && m_switch->checked()) {
        Q_EMIT m_model->requestConnectAp(m_device->path(), path, uuid);
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

QString WirelessPage::connectionUuid(const QString &ssid)
{
    QString uuid;
    for (auto item : m_apItems) {
        if (item->ssid() != ssid) continue;
        uuid = item->uuid();
        if (!uuid.isEmpty()) break;
    }
    return uuid;
}

QString WirelessPage::connectionSsid(const QString &uuid)
{
    QString ssid;

    for (auto item : m_apItems) {
        if (item->uuid() != uuid) continue;

        ssid = item->ssid();
        if (!ssid.isEmpty()) break;
    }

    return ssid;
}

void WirelessPage::onActivaConnections(const QJsonObject &activeConn)
{
    const int &state = activeConn.value("State").toInt();
    const QString &ssid = activeConn.value("Id").toString();
    for (APItem *it: m_apItems) {
        //由于WirelessAccessPoints接口一分钟会刷新一次，
        //所以可能和当前连接状态的uuid不匹配的情况出现，故使用ssid进行判断连接的是哪个wifi
        if (ssid == it->ssid()) m_activateItem = it;
    }
    //考虑到安全红线问题，个人信息和敏感数据禁止打印
#ifdef QT_DEBUG
    qDebug() << "m_activateItem is nullptr :" << (m_activateItem == nullptr) << ",connect ssid =" << ssid;
#endif
    //当state=1代表连接中的状态，这个时候就要保持旋转图标不变
    if (state == Wifi_Connecting) {
        setApLoadin(m_activateItem);
        for (APItem *it: m_apItems) {
            it->setConnected(false);
        }
      //当state =2的时候，代表是连接成功的状态
    } else if(state == Wifi_Connected) {
        for (APItem *it: m_apItems) {
            it->setConnected(m_activateItem == it);
        }
        setApLoadin(nullptr);
      //当state == 3时，代表网络断开连接了
    } else {
        for (APItem *it: m_apItems) {
            it->setConnected(false);
        }
        setApLoadin(nullptr);
    }
}

void WirelessPage::updateAction(APItem *item)
{
    connect(item->action(), &QAction::triggered, [this, item] {
        this->onApWidgetEditRequested(item->data(APItem::PathRole).toString(),
                                      item->data(Qt::ItemDataRole::DisplayRole).toString());
    });
}

void WirelessPage::onClickApItem(const QModelIndex & idx)
{
    //wifi路径为空，则代表是连接隐藏wifi选项
    if (idx.data(APItem::PathRole).toString().isEmpty()) {
       this->showConnectHidePage();
       return;
    }

    const QStandardItemModel *deviceModel = qobject_cast<const QStandardItemModel *>(idx.model());
    if (!deviceModel) {
       return;
    }
    //当点击同一个wifi的时候，不再响应
    if (deviceModel->item(idx.row()) == m_clickItem)
        return;
    m_clickItem = dynamic_cast<APItem *>(deviceModel->item(idx.row()));
    if (!m_clickItem) {
       qDebug() << "clicked item is nullptr";
       return;
    }

    //当前处于连接中状态，则返回
    if (m_clickItem->isConnected()) {
       return;
    }
    this->onApWidgetConnectRequested(idx.data(APItem::PathRole).toString(),
                                    idx.data(Qt::ItemDataRole::DisplayRole).toString());
}
