// SPDX-FileCopyrightText: 2019 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "interface/namespace.h"
#include "widgets/contentwidget.h"

#include <DIconButton>

QT_BEGIN_NAMESPACE
class QLabel;
class QLineEdit;
class QPushButton;
class QFileDialog;
class QGSettings;
QT_END_NAMESPACE

class TitleLabel;

namespace dcc {
namespace bluetooth {
class Device;
class Adapter;
class BluetoothModel;
}
}

namespace DCC_NAMESPACE {
namespace bluetooth {
class DetailPage : public dcc::ContentWidget
{
    Q_OBJECT
public:
    explicit DetailPage(const dcc::bluetooth::BluetoothModel *model, const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    ~DetailPage();

Q_SIGNALS:
    void requestIgnoreDevice(const dcc::bluetooth::Adapter *adapter, const dcc::bluetooth::Device *device);
    void requestDisconnectDevice(const dcc::bluetooth::Device *device);
    void requestConnectDevice(const dcc::bluetooth::Device *device, const dcc::bluetooth::Adapter *adapter);
    void requestSetDevAlias(const dcc::bluetooth::Device *device, const QString &devAlias);
    void requestBack();

public Q_SLOTS:
    void removeDevice(const QString &id);

private Q_SLOTS:
    void onDeviceAliasChanged();
    void onDeviceStatusChanged();
    void onSettingChanged(const QString &key);
private:
    const dcc::bluetooth::BluetoothModel *m_bluetoothModel;
    const dcc::bluetooth::Adapter *m_adapter;
    const dcc::bluetooth::Device *m_device;
    QPushButton *m_ignoreButton;
    QPushButton *m_disconnectButton;
    QPushButton *m_connectButton;
    QPushButton *m_transfileButton;
    QLineEdit *m_editDevAlias;
    TitleLabel *m_devNameLabel;
    QFileDialog *m_transFile;
    QGSettings *m_setting;
};
}
}
