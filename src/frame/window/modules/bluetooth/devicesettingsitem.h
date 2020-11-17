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

#pragma once

#include "interface/namespace.h"
#include "modules/bluetooth/device.h"
#include "modules/bluetooth/adapter.h"

#include <DListView>
#include <DSpinner>

#include <QObject>
#include <QPointer>
#include <QDateTime>

DWIDGET_USE_NAMESPACE
using namespace dcc::bluetooth;

QT_BEGIN_NAMESPACE
class QStyle;
QT_END_NAMESPACE

namespace dcc {
namespace bluetooth {
class Adapter;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {

class BtStandardItem : public DStandardItem
{
public:
    using DStandardItem::DStandardItem;
};

class DeviceSettingsItem : public QObject
{
    Q_OBJECT
public:
    explicit DeviceSettingsItem(const dcc::bluetooth::Device *device, QStyle *style);
    virtual ~DeviceSettingsItem();
    BtStandardItem *getStandardItem(DTK_WIDGET_NAMESPACE::DListView *parent = nullptr);
    BtStandardItem *createStandardItem(DTK_WIDGET_NAMESPACE::DListView *parent = nullptr);
    const dcc::bluetooth::Device *device() const;
    void setLoading(const bool loading);
    void resetDeviceItem() { m_deviceItem = nullptr; }

private:
    void setDevice(const dcc::bluetooth::Device *device);
    void initItemActionList();
    void loadingStart();
    void loadingStop();

Q_SIGNALS:
    void requestConnectDevice(const dcc::bluetooth::Device *device, const dcc::bluetooth::Adapter *adapter) const;
    void requestShowDetail(const dcc::bluetooth::Device *device) const;

private Q_SLOTS:
    void onDeviceStateChanged(const dcc::bluetooth::Device::State &state, bool paired);
    void onDevicePairedChanged(const bool &paired);

    /*!
     * \brief onUpdateLoadingPos 更新加载动画
     */
    void onUpdateLoading();

private:
    const dcc::bluetooth::Device *m_device{nullptr};
    QPointer<DTK_WIDGET_NAMESPACE::DSpinner> m_loadingIndicator;
    BtStandardItem *m_deviceItem{nullptr};
    DTK_WIDGET_NAMESPACE::DListView *m_parentDListView;
    DViewItemActionList m_dActionList;
    QPointer<DViewItemAction> m_loadingAction;
    QPointer<DViewItemAction> m_textAction;
    QPointer<DViewItemAction> m_iconAction;
    QStyle *m_style;
};
} // namespace DCC_NAMESPACE
} // namespace dcc
