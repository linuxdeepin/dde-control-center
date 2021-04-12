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

#include "wiredpage.h"
#include "connectioneditpage.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/switchwidget.h"
#include "widgets/nextpagewidget.h"
#include "widgets/tipsitem.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DStyleOption>

#include <networkmodel.h>

#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>

#include "dstyle.h"

DWIDGET_USE_NAMESPACE

namespace DCC_NAMESPACE {
namespace network {
using namespace dcc::widgets;
using namespace dde::network;

WiredPage::WiredPage(WiredDevice *dev, QWidget *parent)
    : ContentWidget(parent)
    , m_device(dev)
    , m_lvProfiles(new DListView())
{
    //有线连接
    m_lvProfiles->setModel(m_modelprofiles = new QStandardItemModel(this));
    m_lvProfiles->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvProfiles->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvProfiles->setSelectionMode(QAbstractItemView::NoSelection);
    m_lvProfiles->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    TipsItem *tips = new TipsItem;
    tips->setFixedHeight(80);
    tips->setText(tr("Plug in the network cable first"));

    m_tipsGrp = new SettingsGroup;
    m_tipsGrp->appendItem(tips);

    //~ contents_path /network/Wired Network
    QLabel *lblTitle = new QLabel(tr("Wired Network Adapter"));//有线网卡
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_switch = new SwitchWidget(nullptr, lblTitle);
    m_switch->setChecked(dev->enabled());
    GSettingWatcher::instance()->bind("wiredSwitch", m_switch);
    GSettingWatcher::instance()->bind("wiredSwitch", m_lvProfiles);
    m_tipsGrp->setVisible(dev->enabled());
    connect(m_switch, &SwitchWidget::checkedChanged, this, [this] (const bool checked) {
        Q_EMIT requestDeviceEnabled(m_device->path(), checked);
    });
    //设置有线网卡选中状态
    connect(m_device, &NetworkDevice::enableChanged, m_switch, &SwitchWidget::setChecked);

    //有线网络下的三级菜单的“+”创建按钮大小
    m_createBtn = new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement);
    m_createBtn->setMinimumSize(QSize(47, 47));
    //~ contents_path /network/Wired Network/addWiredConnection
    m_createBtn->setToolTip(tr("Add Network Connection"));
    GSettingWatcher::instance()->bind("addConnection", m_createBtn);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_switch, 0, Qt::AlignTop);
    centralLayout->addWidget(m_tipsGrp);
    QMargins itemMargins(m_lvProfiles->itemMargins());
    itemMargins.setLeft(10);
    m_lvProfiles->setItemMargins(itemMargins);
    centralLayout->addWidget(m_lvProfiles);
    centralLayout->addStretch();
    centralLayout->addWidget(m_createBtn, 0, Qt::AlignmentFlag::AlignHCenter);
    centralLayout->setSpacing(10);//三级菜单控件间的间隙
    centralLayout->setMargin(0);
    centralLayout->setContentsMargins(ThirdPageContentsMargins);
    //当点击下拉选框，设置半透明状态
    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContentsMargins(0, 0, 8, 0);
    setContent(centralWidget);
    setTitle(tr("Select Settings"));
    //点击有线连接按钮
    connect(m_lvProfiles, &DListView::clicked, this, [this](const QModelIndex &idx) {
        qDebug() << "m_lvProfiles: " << idx.data(PathRole).toString();
        this->activateConnection(idx.data(PathRole).toString());
    });

    connect(m_createBtn, &QPushButton::clicked, this, &WiredPage::createNewConnection);
    connect(m_device, &WiredDevice::connectionsChanged, this, &WiredPage::refreshConnectionList);
    connect(m_device, &WiredDevice::activeWiredConnectionInfoChanged, this, &WiredPage::checkActivatedConnection);
    connect(m_device, static_cast<void (WiredDevice::*)(WiredDevice::DeviceStatus) const>(&WiredDevice::statusChanged),
            this, &WiredPage::onDeviceStatusChanged);
    connect(m_device, &WiredDevice::removed, this, &WiredPage::onDeviceRemoved);

    onDeviceStatusChanged(m_device->status());
    QTimer::singleShot(1, this, &WiredPage::refreshConnectionList);
}

WiredPage::~WiredPage()
{
    GSettingWatcher::instance()->erase("addConnection");
    GSettingWatcher::instance()->erase("wiredSwitch");
}

void WiredPage::setModel(NetworkModel *model)
{
    m_model = model;

    QTimer::singleShot(1, this, &WiredPage::initUI);
}

void WiredPage::jumpPath(const QString &searchPath)
{
    if (searchPath == "addWiredConnection") {
        QTimer::singleShot(20, this, &WiredPage::createNewConnection);
    }
}

void WiredPage::initUI()
{
    Q_EMIT requestConnectionsList(m_device->path());
}

void WiredPage::refreshConnectionList()
{
    // get all available wired connections path
    const auto wiredConns = m_model->wireds();

    QSet<QString> availableWiredConns;
    availableWiredConns.reserve(wiredConns.size());

    m_modelprofiles->clear();
    m_connectionPath.clear();

    for (const auto &wiredConn : wiredConns) {
        const QString path = wiredConn.value("Path").toString();
        if (!path.isEmpty())
            availableWiredConns << path;
    }

    const auto connObjList = m_device->connections();
    QSet<QString> connPaths;
    for (const auto &connObj : connObjList) {
        const QString &path = connObj.value("Path").toString();
        // pass unavailable wired conns, like 'PPPoE'
        if (!availableWiredConns.contains(path))
            continue;

        connPaths << path;
        if (m_connectionPath.values().contains(path))
            continue;

        DStandardItem *it = new DStandardItem(m_model->connectionNameByPath(path));
        it->setData(path, PathRole);
        it->setCheckable(false);
        it->setCheckState(path == m_device->activeWiredConnSettingPath() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);

        DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        editaction->setClickAreaMargins(ArrowEnterClickMargin);

        connect(editaction, &QAction::triggered, [this, path] {
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
    m_editPage = new ConnectionEditPage(ConnectionEditPage::WiredConnection,
                                        m_device->path(), m_model->connectionUuidByPath(connectionPath));
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestWiredDeviceEnabled, this, &WiredPage::requestDeviceEnabled);
    connect(m_editPage, &ConnectionEditPage::activateWiredConnection, this, &WiredPage::activateEditConnection);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &WiredPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);
}

void WiredPage::createNewConnection()
{
    m_editPage = new ConnectionEditPage(ConnectionEditPage::WiredConnection, m_device->path());
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestWiredDeviceEnabled, this, &WiredPage::requestDeviceEnabled);
    connect(m_editPage, &ConnectionEditPage::activateWiredConnection, this, &WiredPage::activateEditConnection);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &WiredPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);

    //only create New Connection can set "Cancel","Save" button
    //fix: https://pms.uniontech.com/zentao/bug-view-61563.html
    m_editPage->setButtonTupleEnable(true);
}

void WiredPage::activateConnection(const QString &connectionPath)
{
    qDebug() << Q_FUNC_INFO;
    Q_EMIT requestActiveConnection(m_device->path(), m_model->connectionUuidByPath(connectionPath));
}

void WiredPage::activateEditConnection(const QString &connectPath, const QString &uuid)
{
    Q_UNUSED(connectPath);
    Q_EMIT requestActiveConnection(m_device->path(), uuid);
}

void WiredPage::checkActivatedConnection()
{
    for (auto it(m_connectionPath.cbegin()); it != m_connectionPath.cend(); ++it) {
        if (it.value() == m_device->activeWiredConnSettingPath()) {
            it.key()->setCheckState(Qt::CheckState::Checked);
        } else {
            it.key()->setCheckState(Qt::CheckState::Unchecked);
        }
    }
}

void WiredPage::onDeviceStatusChanged(const NetworkDevice::DeviceStatus stat)
{
        const bool unavailable = stat <= NetworkDevice::Unavailable;
        m_tipsGrp->setVisible(unavailable);
}

void WiredPage::onDeviceRemoved()
{
    if (!m_editPage.isNull()) {
        m_editPage->onDeviceRemoved();
    }

    Q_EMIT back();
}
}
}
