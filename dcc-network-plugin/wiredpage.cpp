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

#include "wiredpage.h"
#include "connectioneditpage.h"
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

WiredPage::WiredPage(WiredDevice *dev, QWidget *parent)
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
    connect(m_switch, &SwitchWidget::checkedChanged, this, [this] (const bool checked) {
        m_device->setEnabled(checked);
    });

    connect(this, &WiredPage::requestDeviceEnabled, this, [this] (const QString & devPath, const bool checked) {
        Q_UNUSED(devPath);
        m_device->setEnabled(checked);
    });

    // 设置有线网卡选中状态
    connect(m_device, &WiredDevice::enableChanged, m_switch, &SwitchWidget::setChecked);

    // 有线网络下的三级菜单的“+”创建按钮大小
    m_createBtn = new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement, this);
    m_createBtn->setMinimumSize(QSize(47, 47));

    // ~ contents_path /network/Wired Network/addWiredConnection
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
    connect(m_lvProfiles, &DListView::clicked, this, [this](const QModelIndex & idx) {
        this->activateConnection(idx.data(PathRole).toString());
    });

    connect(m_createBtn, &QPushButton::clicked, this, &WiredPage::createNewConnection);
    connect(m_device, &WiredDevice::connectionAdded, this, &WiredPage::refreshConnectionList);
    connect(m_device, &WiredDevice::connectionRemoved, this, &WiredPage::refreshConnectionList);
    connect(m_device, &WiredDevice::connectionChanged, this, &WiredPage::onUpdateConnectionStatus);
    connect(m_device, &WiredDevice::deviceStatusChanged, this, &WiredPage::onDeviceStatusChanged);
    connect(m_device, &WiredDevice::enableChanged, this, &WiredPage::onUpdateConnectionStatus);
    connect(m_pNetworkController, &NetworkController::deviceRemoved, this, &WiredPage::onDeviceRemoved);

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
        DStandardItem *item = static_cast<DStandardItem *>(m_modelprofiles->item(i));
        WiredConnection *connObj = static_cast<WiredConnection *>(item->data(ConnectionRole).value<void *>());
        if (items.contains(connObj))
            item->setCheckState(connObj->connected() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
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

        DStandardItem *it = new DStandardItem(connObj->connection()->id());
        it->setData(path, PathRole);
        it->setData(QVariant::fromValue((void *)connObj), ConnectionRole);
        it->setCheckable(false);
        it->setCheckState(connObj->connected() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

        DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        editaction->setClickAreaMargins(ArrowEnterClickMargin);

        connect(editaction, &QAction::triggered, this, [this, path] {
            this->editConnection(path);
        });

        it->setActionList(Qt::Edge::RightEdge, {editaction});
        m_modelprofiles->appendRow(it);
        m_connectionPath.insert(it, path);
    }

    checkActivatedConnection();
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
    Q_EMIT requestNextPage(m_editPage);
}

void WiredPage::createNewConnection()
{
    editConnection(QString());

    //only create New Connection can set "Cancel","Save" button
    //fix: https://pms.uniontech.com/zentao/bug-view-61563.html
    m_editPage->setButtonTupleEnable(true);
}

void WiredPage::activateConnection(const QString &connectionPath)
{
    m_device->connectNetwork(connectionPath);
}

void WiredPage::activateEditConnection(const QString &connectPath, const QString &uuid)
{
    Q_UNUSED(uuid);
    m_device->connectNetwork(connectPath);
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
