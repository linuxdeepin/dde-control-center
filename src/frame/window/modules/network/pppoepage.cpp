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

#include "pppoepage.h"
#include "connectioneditpage.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/translucentframe.h"
#include "widgets/loadingnextpagewidget.h"
#include "widgets/switchwidget.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <networkmodel.h>
#include <networkdevice.h>
#include <DFloatingButton>

#include <QPushButton>
#include <QDebug>
#include <QVBoxLayout>

DWIDGET_USE_NAMESPACE

using namespace dcc::widgets;
using namespace DCC_NAMESPACE::network;
using namespace dde::network;

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
    //~ child_page DSL
    tr("Create PPPoE Connection");
    connect(m_createBtn, &QPushButton::clicked, this, &PppoePage::createPPPoEConnection);
    connect(m_lvsettings, &QListView::clicked, this, [this](const QModelIndex &idx) {
        this->onPPPoESelected(idx.data(PathRole).toString(), idx.data(UuidRole).toString());
    });
}

PppoePage::~PppoePage()
{
    GSettingWatcher::instance()->erase("createPppoe", m_createBtn);
    if (!m_editPage.isNull())
        m_editPage->deleteLater();
}

void PppoePage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::connectionListChanged, this, &PppoePage::onConnectionListChanged);
    connect(model, &NetworkModel::activeConnectionsChanged, this, &PppoePage::onActiveConnectionChanged);

    onConnectionListChanged();
}

void PppoePage::createPPPoEConnection()
{
    NetworkDevice *device = nullptr;
    for (auto d : m_model->devices()) {
        if (d->type() == NetworkDevice::Wired) {
            device = d;
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

    for (const auto &pppoe : m_model->pppoes()) {
        const auto name = pppoe.value("Id").toString();
        const auto uuid = pppoe.value("Uuid").toString();
        const auto macAddress = pppoe.value("HwAddress").toString();

        DeviceItem *it = new DeviceItem;
        it->setText(name);
        it->setData(uuid, UuidRole);
        it->setCheckable(true);

        for (auto device : m_model->devices()) {
            if (device->info().value("HwAddress").toString() == macAddress) {
                it->setDevice(device);
                break;
            }
        }

        it->setData(it->device() ? it->device()->path() : "/", PathRole);

        DViewItemAction *editaction = new DViewItemAction(Qt::AlignmentFlag::AlignCenter, QSize(), QSize(), true);
        QStyleOption opt;
        editaction->setIcon(DStyleHelper(style()).standardIcon(DStyle::SP_ArrowEnter, &opt, nullptr));
        editaction->setClickAreaMargins(ArrowEnterClickMargin);
        connect(editaction, &QAction::triggered, [this, uuid, it] {
            this->onConnectionDetailClicked(uuid, it);
        });
        it->setActionList(Qt::Edge::RightEdge, {editaction});
        m_items[uuid] = it;

        m_modelSettings->appendRow(it);
    }

    onActiveConnectionChanged(m_model->activeConns());
}

void PppoePage::onConnectionDetailClicked(const QString &connectionUuid, DeviceItem *item)
{
    NetworkDevice *device = item->device();
    m_editPage = new ConnectionEditPage(ConnectionEditPage::ConnectionType::PppoeConnection, device ? device->path() : "/", connectionUuid);
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &PppoePage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &PppoePage::requestFrameKeepAutoHide);

    Q_EMIT requestNextPage(m_editPage);
}

void PppoePage::onPPPoESelected(const QString devicePath, const QString &connectionUuid)
{
    Q_EMIT requestActivateConnection(devicePath, connectionUuid);
}

void PppoePage::onActiveConnectionChanged(const QList<QJsonObject> &conns)
{
    for (int i = 0; i < m_modelSettings->rowCount(); ++i) {
        m_modelSettings->item(i)->setCheckState(Qt::CheckState::Unchecked);
    }
    for (const QJsonObject &connObj : conns) {
        const QString &uuid = connObj.value("Uuid").toString();
        if (!m_items.contains(uuid)) {
            continue;
        }
        // the State of Active Connection
        // 0:Unknow, 1:Activating, 2:Activated, 3:Deactivating, 4:Deactivated
        int state = m_model->activeConnObjectByUuid(uuid).value("State").toInt(0);
        if (state == 2) {
            m_items[uuid]->setCheckState(Qt::CheckState::Checked);
            //w->setLoading(false);
        } else if (state == 1) {
            //TODO: connecting indicator?
            //w->setLoading(true);
        } else {
            //w->setLoading(false);
        }
    }
}

void PppoePage::jumpPath(const QString &searchPath)
{
    if (searchPath == "Create PPPoE Connection") {
        createPPPoEConnection();
    }
}
