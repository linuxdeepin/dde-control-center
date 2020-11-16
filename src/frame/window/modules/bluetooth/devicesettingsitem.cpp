/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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

#include "devicesettingsitem.h"
#include "modules/bluetooth/device.h"
#include "widgets/labels/normallabel.h"

#include <QHBoxLayout>
#include <QMouseEvent>
#include <QTimer>

#include <DApplicationHelper>

using namespace dcc::widgets;
using namespace dcc;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

#define lightIcon ":/icons/deepin/builtin/light/buletooth_"
#define darkIcon ":icons/deepin/builtin/dark/buletooth_"
DeviceSettingsItem::DeviceSettingsItem(const Device *device, QStyle *style)
    : m_device(device)
    , m_deviceItem(new BtStandardItem)
    , m_parentDListView(nullptr)
    , m_style(style)
{
    initItemActionList();
    if (DApplicationHelper::instance()->themeType() == DApplicationHelper::LightType) {
        if (!m_device->deviceType().isEmpty())
            m_deviceItem->setIcon(QIcon(lightIcon + m_device->deviceType() + "_light.svg"));
        else
            m_deviceItem->setIcon(QIcon(lightIcon + QString("other_light.svg")));
    } else {
        if (!m_device->deviceType().isEmpty())
            m_deviceItem->setIcon(QIcon(darkIcon + m_device->deviceType() + "_dark.svg"));
        else
            m_deviceItem->setIcon(QIcon(darkIcon + QString("other_dark.svg")));
    }
    m_deviceItem->setText(m_device->alias().isEmpty() ? m_device->name() : m_device->alias());
    m_deviceItem->setActionList(Qt::RightEdge, m_dActionList);

    BtSortInfo info;
    info.connected = m_device->connectState();
    info.time = static_cast<int>(QDateTime::currentDateTime().toTime_t());
    m_deviceItem->setSortInfo(info);
}

DeviceSettingsItem::~DeviceSettingsItem()
{
    if (!m_loadingIndicator.isNull()) {
        m_loadingIndicator->hide();
        m_loadingIndicator->deleteLater();
    }
    if (!m_loadingAction.isNull()) {
        m_loadingAction->setVisible(false);
    }
}

void DeviceSettingsItem::initItemActionList()
{
    if (!m_loadingIndicator.isNull()) {
        m_loadingIndicator->deleteLater();
    }
    m_loadingIndicator = new DSpinner();
    m_loadingIndicator->setFixedSize(24, 24);
    m_loadingIndicator->hide();
    m_loadingAction = new DViewItemAction(Qt::AlignLeft | Qt::AlignCenter, QSize(), QSize(), false);
    m_loadingAction->setWidget(m_loadingIndicator);
    m_iconAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), true);
    m_textAction = new DViewItemAction(Qt::AlignLeft, QSize(), QSize(), true);
    m_iconAction->setIcon(m_style->standardIcon(QStyle::SP_ArrowRight));
    m_dActionList.clear();
    m_dActionList.append(m_loadingAction);
    m_dActionList.append(m_textAction);
    m_dActionList.append(m_iconAction);
}

void DeviceSettingsItem::setLoading(const bool loading)
{
    if (loading) {
        if (m_parentDListView) {
            QModelIndex index;
            for (int i = 0; i < m_parentDListView->count(); ++i) {
                const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(m_parentDListView->model());
                if (!deviceModel) {
                    return;
                }
                DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(i));
                if (!item) {
                    return;
                }
                if (m_deviceItem == item) {
                    index = m_parentDListView->model()->index(i, 0);
                    break;
                }
            }
            QRect itemrect = m_parentDListView->visualRect(index);
            QPoint point(itemrect.x() + itemrect.width(), itemrect.y());
            m_loadingIndicator->move(point);
        }
        m_loadingIndicator->start();
        m_loadingIndicator->show();
        m_loadingAction->setVisible(true);
        m_textAction->setVisible(false);
    } else {
        m_loadingIndicator->stop();
        m_loadingIndicator->hide();
        m_loadingAction->setVisible(false);
        m_textAction->setVisible(true);
    }
    if (m_parentDListView) {
        m_parentDListView->update();
    }
}

void DeviceSettingsItem::setDevice(const Device *device)
{
    connect(device, &Device::stateChanged, this, &DeviceSettingsItem::onDeviceStateChanged);
    connect(device, &Device::pairedChanged, this, &DeviceSettingsItem::onDevicePairedChanged);

    connect(m_textAction, &QAction::triggered, this, [this] {
        for (int i = 0; i < m_parentDListView->count(); ++i) {
            const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(m_parentDListView->model());
            if (!deviceModel) {
                return;
            }
            DStandardItem *item = dynamic_cast<DStandardItem *>(deviceModel->item(i));
            if (!item) {
                return;
            }
            if (m_deviceItem == item) {
                m_parentDListView->setCurrentIndex(m_parentDListView->model()->index(i, 0));
                m_parentDListView->clicked(m_parentDListView->model()->index(i, 0));
                break;
            }
        }
    });

    connect(m_iconAction, &QAction::triggered, this, [this] {
        Q_EMIT requestShowDetail(m_device);
    });
    connect(device, &Device::aliasChanged, this, [this](const QString &alias) {
        if (m_deviceItem) {
            m_deviceItem->setText(alias);

            BtSortInfo info = m_deviceItem->sortInfo();
            info.time = static_cast<int>(QDateTime::currentDateTime().toTime_t());
            m_deviceItem->setSortInfo(info);
        }

        Q_EMIT requestSort();
    });

    onDeviceStateChanged(device->state(), device->connectState());
    onDevicePairedChanged(device->paired());
}

BtStandardItem *DeviceSettingsItem::getStandardItem(DListView *parent)
{
    if (parent != nullptr) {
        m_parentDListView = parent;
        m_loadingIndicator->setParent(parent->viewport());

        if (m_device)
            setDevice(m_device);
    }
    return m_deviceItem;
}

BtStandardItem *DeviceSettingsItem::createStandardItem(DListView *parent)
{
    initItemActionList();
    if (parent != nullptr) {
        m_parentDListView = parent;
        m_loadingIndicator->setParent(parent->viewport());
        if (m_device)
            setDevice(m_device);
    }
    m_deviceItem = new BtStandardItem;

    if (DApplicationHelper::instance()->themeType() == DApplicationHelper::LightType) {
        if (!m_device->deviceType().isEmpty())
            m_deviceItem->setIcon(QIcon(lightIcon + m_device->deviceType() + "_light.svg"));
        else
            m_deviceItem->setIcon(QIcon(lightIcon + QString("other_light.svg")));
    } else {
        if (!m_device->deviceType().isEmpty())
            m_deviceItem->setIcon(QIcon(darkIcon + m_device->deviceType() + "_dark.svg"));
        else
            m_deviceItem->setIcon(QIcon(darkIcon + QString("other_dark.svg")));
    }
    m_deviceItem->setText(m_device->alias().isEmpty() ? m_device->name() : m_device->alias());
    m_deviceItem->setActionList(Qt::RightEdge, m_dActionList);

    BtSortInfo info;
    info.connected = m_device->connectState();
    if (m_deviceItem) {
        info.time = static_cast<int>(QDateTime::currentDateTime().toTime_t());
        m_deviceItem->setSortInfo(info);
    }

    return m_deviceItem;
}

void DeviceSettingsItem::onDeviceStateChanged(const Device::State &state, bool connectState)
{
    if (state == Device::StateAvailable) {
        setLoading(true);
        return;
    }
    QString tip;
    if (state == Device::StateConnected && connectState) {
        tip = tr("Connected");
        setLoading(false);
    } else if (state == Device::StateUnavailable || state == Device::StateDisconnecting) {
        tip = tr("Not connected");
        setLoading(false);
    }
    m_textAction->setText(tip);

    if (m_deviceItem) {
        BtSortInfo info = m_deviceItem->sortInfo();
        info.connected = (state == Device::StateConnected && connectState);
        m_deviceItem->setSortInfo(info);
    }

    Q_EMIT requestSort();
}

void DeviceSettingsItem::onDevicePairedChanged(const bool &paired)
{
    if (paired) {
        m_iconAction->setVisible(true);
    } else {
        m_iconAction->setVisible(false);
    }
}

const Device *DeviceSettingsItem::device() const
{
    return m_device;
}
