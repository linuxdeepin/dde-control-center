// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "wiredpage.h"
#include "connectioneditpage.h"
#include "connectionpageitem.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DStyleOption>
#include <DListView>
#include <DFloatingButton>
#include <DStyle>

#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

#include <widgets/settingsgroup.h>
#include <widgets/settingsheaderitem.h>
#include <widgets/switchwidget.h>
#include <widgets/nextpagewidget.h>
#include <widgets/tipsitem.h>

#include <wireddevice.h>
#include <networkdevicebase.h>
#include <networkcontroller.h>
#include <networkconst.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dde::network;

WiredPage::WiredPage(dde::network::WiredDevice *dev, QWidget *parent)
    : ContentWidget(parent)
    , m_device(dev)
    , m_pNetworkController(NetworkController::instance())
    , m_lvProfiles(new DListView(this))
{
    // 有线连接
    m_lvProfiles->setAccessibleName("lvProfiles");
    m_lvProfiles->setModel(m_modelprofiles = new QStandardItemModel(this));
    m_lvProfiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvProfiles->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvProfiles->setSelectionMode(QAbstractItemView::NoSelection);
    m_lvProfiles->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_modelprofiles->setSortRole(SortRole);

    TipsItem *tips = new TipsItem(this);
    tips->setFixedHeight(80);
    tips->setText(tr("Plug in the network cable first"));

    m_tipsGrp = new SettingsGroup;
    m_tipsGrp->appendItem(tips);

    // ~ contents_path /network/Wired Network
    QLabel *lblTitle = new QLabel(tr("Wired Network Adapter"), this); //有线网卡
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_switch = new SwitchWidget(this, lblTitle);
    m_switch->setChecked(dev->isEnabled());
    // 因为swtichbutton内部距离右间距为4,所以这里设置6就可以保证间距为10
    m_switch->getMainLayout()->setContentsMargins(10, 0, 6, 0);

    GSettingWatcher::instance()->bind("wiredSwitch", m_switch);
    GSettingWatcher::instance()->bind("wiredSwitch", m_lvProfiles);
    m_tipsGrp->setVisible(dev->isEnabled());
    connect(m_switch, &SwitchWidget::checkedChanged, this, [ this ] (const bool checked) {
        m_device->setEnabled(checked);
    });

    connect(this, &WiredPage::requestDeviceEnabled, this, [ this ] (const QString & devPath, const bool checked) {
        Q_UNUSED(devPath);
        m_device->setEnabled(checked);
    });

    // 设置有线网卡选中状态
    connect(m_device, &WiredDevice::enableChanged, m_switch, &SwitchWidget::setChecked);

    // 有线网络下的三级菜单的“+”创建按钮大小
    m_createBtn = new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement, this);
    m_createBtn->setMinimumSize(QSize(47, 47));

    m_createBtn->setToolTip(tr("Add Network Connection"));
    GSettingWatcher::instance()->bind("addConnection", m_createBtn);

    QVBoxLayout *centralLayout = new QVBoxLayout(this);
    centralLayout->addWidget(m_switch, 0, Qt::AlignTop);
    centralLayout->addWidget(m_tipsGrp);
    QMargins itemMargins(m_lvProfiles->itemMargins());
    itemMargins.setLeft(2);
    m_lvProfiles->setItemMargins(itemMargins);
    centralLayout->addWidget(m_lvProfiles);
    centralLayout->addWidget(m_createBtn, 0, Qt::AlignmentFlag::AlignHCenter);
    centralLayout->setSpacing(10);                              // 三级菜单控件间的间隙
    centralLayout->setMargin(0);
    centralLayout->setContentsMargins(QMargins(10, 0, 10, 0));  // 设置左右间距为10

    // 当点击下拉选框，设置半透明状态
    QWidget *centralWidget = new TranslucentFrame(this);
    centralWidget->setLayout(centralLayout);

    setContentsMargins(0, 10, 0, 10);  // 设置上下间距为10
    setContent(centralWidget);
    setTitle(tr("Select Settings"));

    // 点击有线连接按钮
    connect(m_lvProfiles, &DListView::clicked, this, [ this ](const QModelIndex & idx) {
        if (idx.row() < 0 || idx.row() >= m_modelprofiles->rowCount())
            return;

        ConnectionPageItem *item = static_cast<ConnectionPageItem *>(m_modelprofiles->item(idx.row()));
        WiredConnection *connObj = static_cast<WiredConnection *>(item->itemData());
        if (!connObj->connected())
            m_device->connectNetwork(idx.data(PathRole).toString());

        // 如果正在编辑状态，则切换到当前连接
        if (!m_editPage.isNull() && m_editPage->connectionUuid() != connObj->connection()->uuid()) {
            this->editConnection(connObj->connection()->path());
            m_editPage->setLeftButtonEnable(true);
        }
    });

    connect(m_createBtn, &QPushButton::clicked, this, &WiredPage::createNewConnection);
    connect(m_device, &WiredDevice::connectionAdded, this, [ = ] (const QList<WiredConnection*> newConnections) {
        refreshConnectionList();
        if (!m_newConnectionPath.isEmpty()) {
            for (WiredConnection *connection : newConnections) {
                if (connection->connection()->path() == m_newConnectionPath) {
                    m_device->connectNetwork(connection);
                    m_newConnectionPath.clear();
                    break;
                }
            }
        }
    });
    connect(m_device, &WiredDevice::connectionRemoved, this, &WiredPage::refreshConnectionList);
    connect(m_device, &WiredDevice::connectionChanged, this, &WiredPage::onUpdateConnectionStatus);
    connect(m_device, &WiredDevice::connectionPropertyChanged, this, &WiredPage::onConnectionPropertyChanged);
    connect(m_device, &WiredDevice::deviceStatusChanged, this, &WiredPage::onDeviceStatusChanged);
    connect(m_device, &WiredDevice::enableChanged, this, &WiredPage::onUpdateConnectionStatus);
    connect(m_device, &WiredDevice::activeConnectionChanged, this, &WiredPage::onUpdateConnectionStatus);
    connect(m_device, &WiredDevice::removed, this, &WiredPage::onDeviceRemoved);

    onDeviceStatusChanged(m_device->deviceStatus());
    QTimer::singleShot(1, this, &WiredPage::refreshConnectionList);
}

WiredPage::~WiredPage()
{
    GSettingWatcher::instance()->erase("addConnection");
    GSettingWatcher::instance()->erase("wiredSwitch");
}

void WiredPage::jumpPath(const QString &searchPath)
{
    if (searchPath == "addWiredConnection")
        QTimer::singleShot(20, this, &WiredPage::createNewConnection);
}

void WiredPage::onUpdateConnectionStatus()
{
    QList<WiredConnection *> items = m_device->items();

    // m_modelprofiles
    for (int i = 0; i < m_modelprofiles->rowCount(); i++) {
        ConnectionPageItem *item = static_cast<ConnectionPageItem *>(m_modelprofiles->item(i));
        WiredConnection *connObj = static_cast<WiredConnection *>(item->itemData());
        if (items.contains(connObj)) {
            item->setConnectionStatus(connObj->status());
            item->setData(items.indexOf(connObj), SortRole);
        }
    }

    // 连接状态变化后更新编辑界面按钮状态
    if (!m_editPage.isNull()) {
        m_editPage->initHeaderButtons();
    }

    m_modelprofiles->sort(0);

    // 刷新网络状态时刷新网线是否插入提示信息
    onDeviceStatusChanged(m_device->deviceStatus());
}

void WiredPage::onConnectionPropertyChanged(const QList<WiredConnection *> &changedConnection)
{
    for (int i = 0; i < m_modelprofiles->rowCount(); i++) {
        ConnectionPageItem *item = static_cast<ConnectionPageItem *>(m_modelprofiles->item(i));
        WiredConnection *connObj = static_cast<WiredConnection *>(item->itemData());
        if (changedConnection.contains(connObj))
            item->setText(connObj->connection()->id());
    }
}

void WiredPage::refreshConnectionList()
{
    // get all available wired connections path
    m_modelprofiles->clear();
    m_connectionPath.clear();

    QList<WiredConnection *> connObjList = m_device->items();
    for (WiredConnection *connObj : connObjList) {
        const QString &path = connObj->connection()->path();
        if (path.isEmpty())
            continue;

        if (m_connectionPath.values().contains(path))
            continue;

        ConnectionPageItem *it = new ConnectionPageItem(this, m_lvProfiles, connObj->connection());
        it->setText(connObj->connection()->id());
        it->setData(path, PathRole);
        it->setItemData(connObj);
        it->setConnectionStatus(connObj->status());
        it->setData(connObjList.indexOf(connObj), SortRole);
        connect(it, &ConnectionPageItem::detailClick, this, [ this, path ] {
            this->editConnection(path);
            m_editPage->setLeftButtonEnable(true);
        });
        m_modelprofiles->appendRow(it);
        m_connectionPath.insert(it, path);
    }

    // 连接状态变化后更新编辑界面按钮状态
    if (!m_editPage.isNull()) {
        m_editPage->initHeaderButtons();
    }

    checkActivatedConnection();
    m_modelprofiles->sort(0);
}

void WiredPage::editConnection(const QString &connectionPath)
{
    QString uuid = "";
    if (!connectionPath.isEmpty()) {
        QList<WiredConnection *> connObjList = m_device->items();
        for (WiredConnection *connObj : connObjList) {
            if (connObj->connection()->path() == connectionPath) {
                uuid = connObj->connection()->uuid();
                break;
            }
        }
    }

    m_editPage = new ConnectionEditPage(ConnectionEditPage::WiredConnection, m_device->path(), uuid, this);
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestWiredDeviceEnabled, this, &WiredPage::requestDeviceEnabled);
    connect(m_editPage, &ConnectionEditPage::activateWiredConnection, this, &WiredPage::activateEditConnection);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &WiredPage::requestFrameKeepAutoHide);
    connect(m_editPage, &ConnectionEditPage::disconnect, this, [ this ] {
         m_device->disconnectNetwork();
    });

    Q_EMIT requestNextPage(m_editPage);
}

void WiredPage::createNewConnection()
{
    editConnection(QString());

    //only create New Connection can set "Cancel","Save" button
    //fix: https://pms.uniontech.com/zentao/bug-view-61563.html
    m_editPage->setButtonTupleEnable(true);
}

void WiredPage::activateEditConnection(const QString &connectPath, const QString &uuid)
{
    Q_UNUSED(uuid);
    if(!m_device->connectNetwork(connectPath))
        m_newConnectionPath = connectPath;

    Q_EMIT back();
}

void WiredPage::checkActivatedConnection()
{
    QList<WiredConnection *> connObjList = m_device->items();
    QString activePath;
    for (WiredConnection *connObj : connObjList) {
        if (connObj->connected())
            activePath = connObj->connection()->path();
    }

    for (auto it(m_connectionPath.cbegin()); it != m_connectionPath.cend(); ++it) {
        if (it.value() == activePath)
            it.key()->setCheckState(Qt::CheckState::Checked);
        else
            it.key()->setCheckState(Qt::CheckState::Unchecked);
    }
}

void WiredPage::onDeviceStatusChanged(const DeviceStatus &stat)
{
    const bool unavailable = stat <= DeviceStatus::Unavailable;
    m_tipsGrp->setVisible(unavailable);
}

void WiredPage::onDeviceRemoved()
{
    if (!m_editPage.isNull())
        m_editPage->onDeviceRemoved();

    Q_EMIT back();
}
