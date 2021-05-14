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
    TitleLabel *lblTitle = new TitleLabel(tr("Wired Network Adapter"));//有线网卡
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_switch = new SwitchWidget(nullptr, lblTitle);
    m_switch->setChecked(dev->enabled());
    m_switch->addBackground();
    m_tipsGrp->setVisible(dev->enabled());
    //设置有线网卡选中状态
    connect(m_device, &NetworkDevice::enableChanged, m_switch, &SwitchWidget::setChecked);

    //有线网络下的三级菜单的“+”创建按钮大小
    m_createBtn = new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement);
    m_createBtn->setMinimumSize(QSize(47, 47));
    m_createBtn->setToolTip(tr("Add Network Connection"));

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(m_switch, 0, Qt::AlignTop);
    centralLayout->addWidget(m_tipsGrp);
    centralLayout->addWidget(m_lvProfiles);
    centralLayout->addWidget(m_createBtn, 0, Qt::AlignmentFlag::AlignHCenter);
    centralLayout->setSpacing(10);//三级菜单控件间的间隙
    centralLayout->setMargin(0);
    //当点击下拉选框，设置半透明状态
    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Select Settings"));
    //点击有线连接按钮
    connect(m_lvProfiles, &DListView::clicked, this, [this](const QModelIndex &idx) {
        qDebug() << "m_lvProfiles: " << idx.data(PathRole).toString();
        this->activateConnection(idx.data(PathRole).toString());
    });

    //~ contents_path /network/Wired Network/addWiredConnection
    tr("Add Network Connection");
    connect(m_createBtn, &QPushButton::clicked, this, &WiredPage::createNewConnection);
    connect(m_device, &WiredDevice::connectionsChanged, this, &WiredPage::refreshConnectionList);
    connect(m_device, static_cast<void (WiredDevice::*)(WiredDevice::DeviceStatus) const>(&WiredDevice::statusChanged),
            this, &WiredPage::onDeviceStatusChanged);
    connect(m_device, &WiredDevice::removed, this, &WiredPage::onDeviceRemoved);
    connect(m_device, &WiredDevice::activeConnectionsChanged, this, &WiredPage::onActivatedConnection);
    onDeviceStatusChanged(m_device->status());
    QTimer::singleShot(1, this, &WiredPage::refreshConnectionList);
}

void WiredPage::setModel(NetworkModel *model)
{
    m_model = model;
    //初始化开关状态
    Q_EMIT m_model->initDeviceEnable(m_device->path());
    connect(m_switch, &SwitchWidget::checkedChanged, m_model, [  = ] (const bool checked) {
        qDebug() << "enable:" << checked;
        Q_EMIT m_model->requestDeviceEnable(m_device->path(), checked);
    });
}

void WiredPage::jumpPath(const QString &searchPath)
{
    if (searchPath == "addWiredConnection") {
        QTimer::singleShot(20, this, &WiredPage::createNewConnection);
    }
}

void WiredPage::refreshConnectionList()
{
    // get all available wired connections uuid
    const auto wiredConns = m_model->wireds();

    QSet<QString> availableWiredConns;
    availableWiredConns.reserve(wiredConns.size());

    m_modelprofiles->clear();
    m_connectionUuid.clear();


    for (const auto &wiredConn : wiredConns) {
        const QString uuid = wiredConn.value("Uuid").toString();
        if (!uuid.isEmpty())
            availableWiredConns << uuid;
    }

    const auto connObjList = m_device->connections();
    //考虑到安全红线问题，个人信息和敏感数据禁止打印
#ifdef  QT_DEBUG
    qDebug() << connObjList;
#endif
    QSet<QString> connuuid;
    for (const auto &connObj : connObjList) {
        const QString &uuid = connObj.value("Uuid").toString();
        const QString &path = connObj.value("Path").toString();
        // pass unavailable wired conns, like 'PPPoE'
        if (!availableWiredConns.contains(uuid))
            continue;

        connuuid << uuid;
        if (m_connectionUuid.values().contains(uuid))
            continue;

        //创建页面上的ApItem
        DStandardItem *it = new DStandardItem(m_model->connectionNameByUuid(uuid));
        it->setData(path, PathRole);
        it->setCheckable(false);
        it->setCheckState(uuid == m_device->activeWiredConnUuid() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
        //创建Ap编辑页
        DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        editaction->setClickAreaMargins(ArrowEnterClickMargin);

        connect(editaction, &QAction::triggered, [this, path] {
            this->editConnection(path);
        });
        it->setActionList(Qt::Edge::RightEdge, {editaction});

        m_modelprofiles->appendRow(it);
        m_connectionUuid.insert(it, uuid);
    }
    //获取当前连接状态
    onActivatedConnection(m_device->activeWiredConnectionData());
}

void WiredPage::editConnection(const QString &connectionPath)
{
    m_editPage = new ConnectionEditPage(ConnectionEditPage::WiredConnection,
                                        m_device->path(), m_model->connectionUuidByPath(connectionPath));
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::activateWiredConnection, this, &WiredPage::activateEditConnection);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &WiredPage::requestFrameKeepAutoHide);
    //断开连接
    connect(m_editPage, &ConnectionEditPage::disconnectAP, m_model, &NetworkModel::requestDisconnctAP);
    //删除wifi
    connect(m_editPage, &ConnectionEditPage::deleteConnectAP, m_model, &NetworkModel::deleteConnection);
    Q_EMIT requestNextPage(m_editPage);
}

void WiredPage::createNewConnection()
{
    if (m_editPage)  return;

    m_editPage = new ConnectionEditPage(ConnectionEditPage::WiredConnection, m_device->path());
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::activateWiredConnection, this, &WiredPage::activateEditConnection);
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    //断开连接
    connect(m_editPage, &ConnectionEditPage::disconnectAP, m_model, &NetworkModel::requestDisconnctAP);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &WiredPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);
}

void WiredPage::activateConnection(const QString &connectionPath)
{
    Q_EMIT requestActiveConnection(m_device->path(), m_model->connectionUuidByPath(connectionPath));
}

void WiredPage::activateEditConnection(const QString &connectPath, const QString &uuid)
{
    Q_UNUSED(connectPath);
    Q_EMIT requestActiveConnection(m_device->path(), uuid);
}

void WiredPage::onActivatedConnection(const QJsonObject &activeConn)
{
    qDebug() << "Wired connection state switching";
    for (auto it = m_connectionUuid.cbegin(); it != m_connectionUuid.cend(); ++it) {
        if (it.value() == m_device->activeWiredConnUuid()) {
            it.key()->setCheckState(Qt::CheckState::Checked);
        } else {
            it.key()->setCheckState(Qt::CheckState::Unchecked);
        }
    }

    if (!m_editPage.isNull()){
        m_editPage->updateDisconnectButtons();
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
