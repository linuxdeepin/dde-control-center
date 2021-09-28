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

#include "hotspotpage.h"
#include "connectionhotspoteditpage.h"
#include "connectionpageitem.h"
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "window/utils.h"
#include "window/gsettingwatcher.h"

#include <DFloatingButton>

#include <networkcontroller.h>
#include <wirelessdevice.h>
#include <hotspotcontroller.h>

#include <QVBoxLayout>
#include <QPushButton>
#include <QDebug>

using namespace dde::network;
using namespace dcc;
using namespace dcc::widgets;

const QString defaultHotspotName()
{
    return getlogin();
}

HotspotDeviceWidget::HotspotDeviceWidget(WirelessDevice *wdev, QWidget *parent)
    : QWidget(parent)
    , m_wdev(wdev)
    , m_lvprofiles(new DListView)
    , m_modelprofiles(new QStandardItemModel(this))
    , m_createBtn(new QPushButton)
    , m_page(nullptr)
{
    Q_ASSERT(m_wdev->supportHotspot());

    m_lvprofiles->setModel(m_modelprofiles);
    m_lvprofiles->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_lvprofiles->setSelectionMode(QAbstractItemView::NoSelection);
    QMargins listItemmargin( m_lvprofiles->itemMargins());
    listItemmargin.setLeft(2);
    m_lvprofiles->setItemMargins(listItemmargin);

    QLabel *lblTitle = new QLabel(tr("Hotspot"));               // 个人热点
    DFontSizeManager::instance()->bind(lblTitle, DFontSizeManager::T5, QFont::DemiBold);
    m_hotspotSwitch = new SwitchWidget(nullptr, lblTitle);
    m_createBtn->setText(tr("Add Settings"));

    QVBoxLayout *centralLayout = new QVBoxLayout;

    centralLayout->addWidget(m_hotspotSwitch, 0, Qt::AlignTop);
    centralLayout->addWidget(m_lvprofiles);
    centralLayout->addWidget(m_createBtn);
    centralLayout->addStretch();

    centralLayout->setContentsMargins(0, 0, 0, 0);
    setLayout(centralLayout);

    connect(m_lvprofiles, &QListView::clicked, this, &HotspotDeviceWidget::onConnWidgetSelected);
    connect(m_createBtn, &QPushButton::clicked, this, [ = ] {
        openEditPage();
    });

    connect(m_wdev, &WirelessDevice::hotspotEnableChanged, this, &HotspotDeviceWidget::onHotsportEnabledChanged);

    connect(m_hotspotSwitch, &SwitchWidget::checkedChanged, this, &HotspotDeviceWidget::onSwitchToggled);
    GSettingWatcher::instance()->bind("hotspotSwitch", m_hotspotSwitch->switchButton());

    QTimer::singleShot(100, this, &HotspotDeviceWidget::onHotsportEnabledChanged);
}

HotspotDeviceWidget::~HotspotDeviceWidget()
{
}

void HotspotDeviceWidget::setPage(HotspotPage *p)
{
    m_page = p;
}

void HotspotDeviceWidget::addItems(const QList<HotspotItem *> &newItems)
{
    for (HotspotItem *item : newItems) {
        const QString &ssid = item->connection()->ssid();
        const QString &uuid = item->connection()->uuid();

        ConnectionPageItem *pageItem = new ConnectionPageItem(this, m_lvprofiles, item->connection());
        pageItem->setText(ssid);
        pageItem->setData(uuid, UuidRole);

        pageItem->setData(QVariant::fromValue(static_cast<void *>(item)), itemRole);
        pageItem->setCheckable(true);

        connect(pageItem, &ConnectionPageItem::detailClick, std::bind(&HotspotDeviceWidget::onConnEditRequested, this, uuid));

        m_modelprofiles->appendRow(pageItem);
    }
}

void HotspotDeviceWidget::removeItems(const QList<HotspotItem *> &rmItems)
{
    for (int i = m_modelprofiles->rowCount() - 1; i >= 0; i--) {
        QStandardItem *item = m_modelprofiles->item(i);
        HotspotItem *hotspotItem = static_cast<HotspotItem *>(item->data(itemRole).value<void *>());
        if (rmItems.contains(hotspotItem))
            m_modelprofiles->removeRow(i);
    }
}

void HotspotDeviceWidget::updateItemStatus(const QList<HotspotItem *> &items)
{
    for (int i = 0; i < m_modelprofiles->rowCount(); i++) {
        ConnectionPageItem *item = static_cast<ConnectionPageItem *>(m_modelprofiles->item(i));
        HotspotItem *hotspotItem = static_cast<HotspotItem *>(item->data(itemRole).value<void *>());
        if (items.contains(hotspotItem)) {
            item->setText(hotspotItem->connection()->ssid());
            item->setConnectionStatus(hotspotItem->status());
        }
    }
}

void HotspotDeviceWidget::updateCreateButtonStatus(bool showcreatebtn)
{
    m_createBtn->setVisible(showcreatebtn);
}

void HotspotDeviceWidget::closeHotspot()
{
    HotspotController *hotspotController = NetworkController::instance()->hotspotController();
    // 断开连接
    hotspotController->disconnectItem();
    // 同时禁用热点
    hotspotController->setEnabled(m_wdev, false);
}

void HotspotDeviceWidget::openHotspot()
{
    HotspotController *hotspotController = NetworkController::instance()->hotspotController();
    QList<HotspotItem *> items = hotspotController->items(m_wdev);
    if (items.size() == 0) {
        m_hotspotSwitch->setChecked(false);
        m_hotspotSwitch->setEnabled(true);
        openEditPage(QString());
    } else {
        // 如果有热点列表，则直接连接第一个即可
        hotspotController->connectItem(items[0]);
    }
}

void HotspotDeviceWidget::openEditPage(const QString &uuid)
{
    m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
    m_editPage->initSettingsWidget();

    Q_EMIT m_page->requestNextPage(m_editPage);
}

void HotspotDeviceWidget::onDeviceRemoved()
{
    // back if ap edit page exist
    if (!m_editPage.isNull())
        m_editPage->onDeviceRemoved();

    // destroy self page
    this->deleteLater();
}

void HotspotDeviceWidget::onSwitchToggled(const bool checked)
{
    m_hotspotSwitch->setEnabled(false);
    if (checked)
        openHotspot();
    else
        closeHotspot();
}

void HotspotDeviceWidget::onConnWidgetSelected(const QModelIndex &idx)
{
    const QString uuid = idx.data(UuidRole).toString();
    if (uuid.isEmpty())
        return;

    // 个人热点开启时才尝试激活连接
    if (m_hotspotSwitch->checked()) {
        HotspotController *hotspotController = NetworkController::instance()->hotspotController();
        hotspotController->connectItem(m_wdev, uuid);
    }
}

void HotspotDeviceWidget::onConnEditRequested(const QString &uuid)
{
    m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
    m_editPage->initSettingsWidget();

    connect(m_editPage, &ConnectionHotspotEditPage::requestNextPage, m_page, &HotspotPage::requestNextPage);

    Q_EMIT m_page->requestNextPage(m_editPage);
}

void HotspotDeviceWidget::onHotsportEnabledChanged()
{
    // 手动点击选中热点，以及添加热点时不应该触发打开关闭热点操作
    HotspotController *hotspotController = NetworkController::instance()->hotspotController();
    m_hotspotSwitch->blockSignals(true);
    m_hotspotSwitch->setChecked(hotspotController->enabled(m_wdev));
    m_hotspotSwitch->setEnabled(true);
    m_hotspotSwitch->blockSignals(false);
}

HotspotPage::HotspotPage(QWidget *parent)
    : QWidget(parent)
    , m_newprofile(new DFloatingButton(DStyle::StandardPixmap::SP_IncreaseElement))
{
    m_contents = new QWidget(this);
    m_vScrollLayout = new QVBoxLayout;
    m_contents->setLayout(m_vScrollLayout);
    m_contents->setContentsMargins(2, 0, 2, 0); // 外层间距为8,这里设置2保证间距为10
    ContentWidget *contentWidget = new ContentWidget(this);
    contentWidget->setContent(m_contents);
    m_mainLayout = new QVBoxLayout;
    m_mainLayout->addWidget(contentWidget);
    m_mainLayout->addWidget(m_newprofile, 0, Qt::AlignHCenter);

    setLayout(m_mainLayout);

    //~ contents_path /network/Personal Hotspot
    m_newprofile->setToolTip(tr("Create Hotspot"));
    GSettingWatcher::instance()->bind("createHotspot", m_newprofile);

    connect(m_newprofile, &QAbstractButton::clicked, this, [this] {
        if (this->m_listdevw.empty())
            return;

        this->m_listdevw.front()->openEditPage();
    });

    HotspotController *hotspotController = NetworkController::instance()->hotspotController();
    connect(hotspotController, &HotspotController::deviceAdded, this, &HotspotPage::onDeviceAdded);
    connect(hotspotController, &HotspotController::deviceRemove, this, &HotspotPage::onDeviceRemove);
    connect(hotspotController, &HotspotController::itemAdded, this, &HotspotPage::onItemAdded);
    connect(hotspotController, &HotspotController::itemRemoved, this, &HotspotPage::onItemRemoved);
    connect(hotspotController, &HotspotController::itemChanged, this, &HotspotPage::onItemChanged);
    connect(hotspotController, &HotspotController::activeConnectionChanged, this, &HotspotPage::onActiveConnectionChanged);

    qDeleteAll(m_listdevw);
    m_listdevw.clear();
    m_vScrollLayout->setContentsMargins(0, 0, 0, 0);

    QList<WirelessDevice *> devices = hotspotController->devices();
    onDeviceAdded(devices);
    QMap<WirelessDevice *, QList<HotspotItem *>> deviceItems;
    for (WirelessDevice *device : devices) {
        QList<HotspotItem *> items = hotspotController->items(device);
        if (items.size() > 0)
            deviceItems[device] = items;
    }

    onItemAdded(deviceItems);
    QTimer::singleShot(100, this, [ = ] {
        onActiveConnectionChanged(devices);
    });
}

HotspotPage::~HotspotPage()
{
    GSettingWatcher::instance()->erase("createHotspot", m_newprofile);
}

void HotspotPage::onDeviceAdded(const QList<WirelessDevice *> &devices)
{
    QList<WirelessDevice *> currentDevices;
    for (HotspotDeviceWidget *deviceWidget : m_listdevw)
        currentDevices << deviceWidget->device();

    bool showCreateButton = (devices.size() + currentDevices.size()) > 1;
    // 重新刷新原来的按钮的状态

    for (HotspotDeviceWidget *deviceWidget : m_listdevw)
        deviceWidget->updateCreateButtonStatus(showCreateButton);

    for (WirelessDevice *device : devices) {
        if (currentDevices.contains(device))
            continue;

        HotspotDeviceWidget *deviceWidget = new HotspotDeviceWidget(device, this);
        deviceWidget->updateCreateButtonStatus(showCreateButton);
        deviceWidget->setPage(this);
        m_listdevw.append(deviceWidget);
    }

    // 对设备列表进行排序，然后按照设备的顺序插入到列表中
    HotspotController *hotspotController = NetworkController::instance()->hotspotController();
    QList<WirelessDevice *> hotspotDevices = hotspotController->devices();
    qSort(m_listdevw.begin(), m_listdevw.end(), [ = ] (HotspotDeviceWidget *widget1, HotspotDeviceWidget *widget2) {
        return hotspotDevices.indexOf(widget1->device()) < hotspotDevices.indexOf(widget2->device());
    });

    for (int i = m_vScrollLayout->count() - 1; i >= 0; i--)
        m_vScrollLayout->removeItem(m_vScrollLayout->itemAt(i));

    for (HotspotDeviceWidget *device : m_listdevw)
        m_vScrollLayout->addWidget(device);

    m_newprofile->setVisible(m_listdevw.size() == 1);
}

void HotspotPage::onDeviceRemove(const QList<WirelessDevice *> &rmDevices)
{
    for (HotspotDeviceWidget *deviceWidget : m_listdevw) {
        if (rmDevices.contains(deviceWidget->device())) {
            m_vScrollLayout->removeWidget(deviceWidget);
            m_listdevw.removeOne(deviceWidget);
            delete deviceWidget;
        }
    }

    bool showCreateButton = (m_listdevw.size() > 1);
    for (HotspotDeviceWidget *deviceWidget : m_listdevw)
        deviceWidget->updateCreateButtonStatus(showCreateButton);

    m_newprofile->setVisible(m_listdevw.size() == 1);
}

void HotspotPage::onItemAdded(const QMap<WirelessDevice *, QList<HotspotItem *>> &deviceItems)
{
    for (HotspotDeviceWidget *deviceWidget : m_listdevw) {
        if (!deviceItems.contains(deviceWidget->device()))
            continue;

        deviceWidget->addItems(deviceItems.value(deviceWidget->device()));
    }
}

void HotspotPage::onItemRemoved(const QMap<WirelessDevice *, QList<HotspotItem *>> &deviceItems)
{
    QList<WirelessDevice *> wirelessDevices = deviceItems.keys();
    for (HotspotDeviceWidget *deviceWidget : m_listdevw) {
        if (!deviceItems.contains(deviceWidget->device()))
            continue;

        deviceWidget->removeItems(deviceItems.value(deviceWidget->device()));
    }
}

void HotspotPage::onItemChanged(const QMap<WirelessDevice *, QList<HotspotItem *> > &deviceItems)
{
    for (HotspotDeviceWidget *deviceWidget : m_listdevw) {
        WirelessDevice *device = deviceWidget->device();
        if (!deviceItems.contains(device))
            continue;

        QList<HotspotItem *> items = deviceItems.value(device);
        deviceWidget->updateItemStatus(items);
    }
}

void HotspotPage::onActiveConnectionChanged(const QList<WirelessDevice *> &devices)
{
    HotspotController *hotspotController = NetworkController::instance()->hotspotController();
    for (HotspotDeviceWidget *deviceWidget : m_listdevw) {
        WirelessDevice *device = deviceWidget->device();
        if (!devices.contains(device))
            continue;

        QList<HotspotItem *> items = hotspotController->items(device);
        deviceWidget->updateItemStatus(items);
    }
}
