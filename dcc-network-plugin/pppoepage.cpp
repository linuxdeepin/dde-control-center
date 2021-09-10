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

#include "pppoepage.h"
#include "connectioneditpage.h"
#include "connectionpageitem.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/translucentframe.h"
#include "widgets/loadingnextpagewidget.h"
#include "widgets/switchwidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DFloatingButton>
#include <DSpinner>
#include <DListView>

#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

#include <networkcontroller.h>
#include <networkdevicebase.h>
#include <dslcontroller.h>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace dde::network;

/**
 * @brief DSL页面
 * @param parent
 */
PppoePage::PppoePage(QWidget *parent)
    : ContentWidget(parent)
    , m_lvsettings(new DListView)
    , m_modelSettings(new QStandardItemModel(this))
    , m_createBtn(new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement))
{
    setAccessibleName("PppoePage");
    m_createBtn->setMinimumSize(QSize(47, 47));
    m_createBtn->setToolTip(tr("Create PPPoE Connection"));
    m_createBtn->setAccessibleName(tr("Create PPPoE Connection"));
    GSettingWatcher::instance()->bind("createPppoe", m_createBtn);

    m_lvsettings->setAccessibleName("List_pppoelist");
    m_lvsettings->setModel(m_modelSettings);
    m_lvsettings->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_lvsettings->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvsettings->setSelectionMode(QAbstractItemView::NoSelection);
    m_lvsettings->setItemMargins(QMargins(0, 10, 10, 10));           // 设置列表内文字到列表的背景底间距为10

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(m_lvsettings);
    mainLayout->addWidget(m_createBtn, 0, Qt::AlignmentFlag::AlignHCenter);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(QMargins(10, 0, 10, 0));   // 设置列表项与背景左右间距分别为10

    QWidget *mainWidget = new TranslucentFrame;
    mainWidget->setLayout(mainLayout);

    setContentsMargins(0, 10, 0, 10);   // 设置列表项与背景上下间距分别为10
    setContent(mainWidget);
    setTitle(tr("PPP"));
    //~ contents_path /network/DSL/Create PPPoE Connection
    connect(m_createBtn, &QPushButton::clicked, this, &PppoePage::createPPPoEConnection);
    connect(m_lvsettings, &QListView::clicked, this, [ = ](const QModelIndex &idx) {
        QString uuid = idx.data(UuidRole).toString();
        DSLController *dslController = NetworkController::instance()->dslController();
        dslController->connectItem(uuid);
    });

    DSLController *dslController = NetworkController::instance()->dslController();
    connect(dslController, &DSLController::itemAdded, this, &PppoePage::onConnectionListChanged);
    connect(dslController, &DSLController::itemRemoved, this, &PppoePage::onConnectionListChanged);
    connect(dslController, &DSLController::itemChanged, this, &PppoePage::onItemChanged);
    connect(dslController, &DSLController::activeConnectionChanged, this, &PppoePage::onActiveConnectionChanged);
    connect(this, &PppoePage::refreshConnectionList, this, &PppoePage::onActiveConnectionChanged);

    onConnectionListChanged();
}

PppoePage::~PppoePage()
{
    GSettingWatcher::instance()->erase("createPppoe", m_createBtn);
    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void PppoePage::createPPPoEConnection()
{
    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    NetworkDeviceBase *device = nullptr;

    for (NetworkDeviceBase *dev : devices) {
        if (dev->deviceType() == DeviceType::Wired) {
            device = dev;
            break;
        }
    }
    m_editPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::PppoeConnection, device ? device->path() : "/");
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &PppoePage::requestFrameKeepAutoHide);

    Q_EMIT requestNextPage(m_editPage);

    //only create New Connection can set "Cancel","Save" button
    //fix: https://pms.uniontech.com/zentao/bug-view-61563.html
    m_editPage->setButtonTupleEnable(true);
}

void PppoePage::onConnectionListChanged()
{
    m_items.clear();
    m_modelSettings->clear();

    QList<NetworkDeviceBase *> devices = NetworkController::instance()->devices();
    QList<DSLItem *> items = NetworkController::instance()->dslController()->items();

    for (DSLItem *dslItem : items) {
        const QString name = dslItem->connection()->id();
        const QString uuid = dslItem->connection()->uuid();
        const QString macAddress = dslItem->connection()->hwAddress();

        ConnectionPageItem *pppoe = new ConnectionPageItem(this, m_lvsettings, dslItem->connection());
        pppoe->setText(name);
        pppoe->setData(uuid, UuidRole);
        pppoe->setItemData(dslItem);
        pppoe->setCheckable(true);

        QString devicePath = "/";
        for (NetworkDeviceBase *device : devices) {
            if (device->realHwAdr() == macAddress) {
                devicePath = device->path();
                break;
            }
        }

        connect(pppoe, &ConnectionPageItem::detailClick, [ = ] {
            m_editPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::PppoeConnection, devicePath, uuid);
            m_editPage->initSettingsWidget();

            connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
            connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &PppoePage::requestFrameKeepAutoHide);

            Q_EMIT requestNextPage(m_editPage);
        });
        m_items[uuid] = pppoe;

        m_modelSettings->appendRow(pppoe);
    }

    // 延迟100毫秒刷新，是因为如果马上刷新的话，界面的尺寸没有调整过来，导致显示状态错误
    QTimer::singleShot(100, [ this ] {
        Q_EMIT refreshConnectionList();
    });
}

void PppoePage::onItemChanged(const QList<DSLItem *> &items)
{
    for (int i = 0; i < m_modelSettings->rowCount(); i++) {
        ConnectionPageItem *pppoe = static_cast<ConnectionPageItem *>(m_modelSettings->item(i));
        DSLItem *dslItem = static_cast<DSLItem *>(pppoe->itemData());
        if (items.contains(dslItem))
            pppoe->setText(dslItem->connection()->id());
    }
}

void PppoePage::onActiveConnectionChanged()
{
    for (int i = 0; i < m_modelSettings->rowCount(); ++i)
        m_modelSettings->item(i)->setCheckState(Qt::CheckState::Unchecked);

    QList<DSLItem *> connectionItems = NetworkController::instance()->dslController()->items();
    for (DSLItem *connectionItem : connectionItems) {
        const QString &uuid = connectionItem->connection()->uuid();
        if (!m_items.contains(uuid))
            continue;

        ConnectionPageItem *item = m_items[uuid];
        item->setConnectionStatus(connectionItem->connectionStatus());
    }
}

void PppoePage::jumpPath(const QString &searchPath)
{
    if (searchPath == "Create PPPoE Connection")
        createPPPoEConnection();
}
