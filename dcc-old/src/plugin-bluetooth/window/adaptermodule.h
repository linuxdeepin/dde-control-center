//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#ifndef ADAPTERMODULE_H
#define ADAPTERMODULE_H

#include "interface/moduleobject.h"

#include "widgets/switchwidget.h"
#include "widgets/titlelabel.h"
#include "bluetoothmodel.h"
#include "titleedit.h"

#include <QObject>

DWIDGET_BEGIN_NAMESPACE
class DListView;
class DSpinner;
class DIconButton;
DWIDGET_END_NAMESPACE

namespace DCC_NAMESPACE {
class SettingsGroup;
class DCCListView;
}

class BluetoothWorker;
class QCheckBox;

class AdapterModule : public QObject
{
    Q_OBJECT
public:
    explicit AdapterModule(const BluetoothAdapter *adapter, BluetoothModel *model, BluetoothWorker *work, QObject *parent = nullptr);
    virtual ~AdapterModule();
    const QList<DCC_NAMESPACE::ModuleObject *> &ModuleList() const;
    void active();

public Q_SLOTS:
    void initBluetoothTitle(DCC_NAMESPACE::SettingsGroup *settingsGrp);
    void initDeviceListView(DCC_NAMESPACE::DCCListView *deviceListView);
    void initAnonymousCheckBox(QWidget *w);
    void initOtherDeviceListView(DCC_NAMESPACE::DCCListView *otherDeviceListView);
    void toggleDiscoverableSwitch(const bool checked);
    void updateVisible(bool bPower, bool bDiscovering);

Q_SIGNALS:
    void requestSetToggleAdapter(const BluetoothAdapter *adapter, const bool &toggled);
    void requestConnectDevice(const BluetoothDevice *device, const BluetoothAdapter *adapter);
    void requestDisconnectDevice(const BluetoothDevice *device);
    void requestSetAlias(const BluetoothAdapter *adapter, const QString &alias);
    void requestSetDevAlias(const BluetoothDevice *device, const QString &devAlias);
    void notifyRemoveDevice();
    void requestRefresh(const BluetoothAdapter *adapter);
    void requestDiscoverable(const BluetoothAdapter *adapter, const bool &discoverable);
    void requestSetDisplaySwitch(const bool &on);
    void requestIgnoreDevice(const BluetoothAdapter *adapter, const BluetoothDevice *device);

private Q_SLOTS:
    void contextMenu(const BluetoothAdapter *adapter, const BluetoothDevice *device, DCC_NAMESPACE::DCCListView *view);
    void deviceChanged();

private:
    void setAdapter(const BluetoothAdapter *adapter);
    bool getSwitchState();

protected:
    QList<DCC_NAMESPACE::ModuleObject *> m_moduleList;
    QSet<const BluetoothDevice *> m_devices;

    DCC_NAMESPACE::TitleEdit *m_titleEdit;
    const BluetoothAdapter *m_adapter;
    bool m_hasPaired;

    BluetoothModel *m_model;
    BluetoothWorker *m_work;

    DCC_NAMESPACE::ModuleObject *m_bluetoothTitle;
    DCC_NAMESPACE::ModuleObject *m_otherDevices;
    DCC_NAMESPACE::ModuleObject *m_anonymousCheckBox;
    DCC_NAMESPACE::ModuleObject *m_otherdevicelist;
    DCC_NAMESPACE::ModuleObject *m_devicesTitle;
    DCC_NAMESPACE::ModuleObject *m_mydevicelist;
};

#endif // ADAPTERMODULE_H
