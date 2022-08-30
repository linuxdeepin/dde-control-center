// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

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
    m_spaceAction = new DViewItemAction(Qt::AlignCenter | Qt::AlignRight, QSize(), QSize(), false);
    m_iconAction->setIcon(m_style->standardIcon(QStyle::SP_ArrowRight));
    m_dActionList.clear();
    m_dActionList.append(m_loadingAction);
    m_dActionList.append(m_textAction);
    m_dActionList.append(m_spaceAction);
    m_dActionList.append(m_iconAction);
}

void DeviceSettingsItem::loadingStart()
{
    m_loadingIndicator->start();
    m_loadingIndicator->show();
    m_loadingAction->setVisible(true);
    m_textAction->setVisible(false);
}

void DeviceSettingsItem::loadingStop()
{
    m_loadingIndicator->stop();
    m_loadingIndicator->hide();
    m_loadingAction->setVisible(false);
    m_textAction->setVisible(true);
}

void DeviceSettingsItem::onUpdateLoading()
{
    if (m_parentDListView) {
        QModelIndex index;
        BtStandardItem *item = nullptr;
        for (int i = 0; i < m_parentDListView->count(); ++i) {
            const QStandardItemModel *deviceModel = dynamic_cast<const QStandardItemModel *>(m_parentDListView->model());
            if (!deviceModel) {
                return;
            }
            item = dynamic_cast<BtStandardItem *>(deviceModel->item(i));
            if (!item) {
                continue;
            }
            if (m_deviceItem == item) {
                index = m_parentDListView->model()->index(i, 0);
                break;
            }
        }
        QRect itemrect = m_parentDListView->visualRect(index);
        if (item && (itemrect.height() != 0 || index.row() == 1)) {
            QPoint point(itemrect.x() + itemrect.width(), itemrect.y());
            m_loadingIndicator->move(point);
            loadingStart();
            return;
        }

        if (m_device->state() != Device::StateAvailable)
            loadingStop();
        else {
            loadingStart();
        }
    }
}

void DeviceSettingsItem::setLoading(const bool loading)
{
    if (loading) {
        onUpdateLoading();
        connect(m_parentDListView, &DListView::indexesMoved, this, &DeviceSettingsItem::onUpdateLoading);
    } else {
        loadingStop();
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
            BtStandardItem *item = dynamic_cast<BtStandardItem *>(deviceModel->item(i));
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
        }
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
}

void DeviceSettingsItem::onDevicePairedChanged(const bool &paired)
{
    m_iconAction->setVisible(paired);
    m_spaceAction->setVisible(paired);
}

const Device *DeviceSettingsItem::device() const
{
    return m_device;
}

