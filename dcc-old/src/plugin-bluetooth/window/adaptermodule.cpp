//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later

#include "adaptermodule.h"
#include "bluetoothdevicemodel.h"
#include "bluetoothworker.h"
#include "widgets/widgetmodule.h"
#include "widgets/settingsgroup.h"
#include "widgets/dcclistview.h"

#include <DIconButton>
#include <DListView>
#include <DSwitchButton>
#include <DSpinner>
#include <DIconTheme>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QHBoxLayout>
#include <QTimer>
#include <QStyle>
#include <QApplication>
#include <QMenu>
#include <QFileDialog>
#include <QCheckBox>

using namespace DCC_NAMESPACE;
DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

AdapterModule::AdapterModule(const BluetoothAdapter *adapter, BluetoothModel *model, BluetoothWorker *work, QObject *parent)
    : QObject(parent)
    , m_adapter(adapter)
    , m_hasPaired(false)
    , m_model(model)
    , m_work(work)
{
    m_moduleList.append(new WidgetModule<SettingsGroup>("bluetoothTitleGroup", tr("Allow other Bluetooth devices to find this device"), this, &AdapterModule::initBluetoothTitle));
    m_bluetoothTitle = new WidgetModule<QLabel>("bluetoothTitle", tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)"), [](QLabel *tip) {
        tip->setText(tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)"));
        tip->setWordWrap(true);
        tip->setContentsMargins(16, 0, 10, 0);
    });
    m_moduleList.append(m_bluetoothTitle);
    m_devicesTitle = new WidgetModule<TitleLabel>("devicesTitle", tr("My Devices"), [](TitleLabel *title) {
        title->setText(tr("My Devices"));
        DFontSizeManager::instance()->bind(title, DFontSizeManager::T5, QFont::DemiBold); // 设置字体
    });
    m_moduleList.append(m_devicesTitle);
    m_mydevicelist = new WidgetModule<DCCListView>("mydeviceList", tr("My Devices"), this, &AdapterModule::initDeviceListView);
    m_moduleList.append(m_mydevicelist);
    m_otherDevices = new WidgetModule<TitleLabel>("otherDevices", tr("Other Devices"), [](TitleLabel *title) {
        title->setText(tr("Other Devices"));
        DFontSizeManager::instance()->bind(title, DFontSizeManager::T5, QFont::DemiBold); // 设置字体
    });
    m_moduleList.append(m_otherDevices);
    m_anonymousCheckBox = new WidgetModule<QWidget>("anonymousCheckBox", tr("Other Devices"), this, &AdapterModule::initAnonymousCheckBox);
    m_moduleList.append(m_anonymousCheckBox);
    m_otherdevicelist = new WidgetModule<DCCListView>("otherdeviceList", "", this, &AdapterModule::initOtherDeviceListView);
    m_moduleList.append(m_otherdevicelist);

    setAdapter(m_adapter);
}

AdapterModule::~AdapterModule()
{
    qDeleteAll(m_moduleList);
}

const QList<DCC_NAMESPACE::ModuleObject *> &AdapterModule::ModuleList() const
{
    return m_moduleList;
}

void AdapterModule::active()
{
    updateVisible(m_adapter->powered(), m_adapter->discovering());
    if (m_adapter->powered() && !m_adapter->discovering())
        Q_EMIT requestRefresh(m_adapter);
}

void AdapterModule::initBluetoothTitle(DCC_NAMESPACE::SettingsGroup *settingsGrp)
{
    m_titleEdit = new TitleEdit;
    m_titleEdit->setTitle(m_adapter->name());
    DSpinner *spinnerBtn = new DSpinner(m_titleEdit);
    spinnerBtn->setFixedSize(24, 24);
    spinnerBtn->start();
    spinnerBtn->hide();
    m_titleEdit->setMinimumWidth(10);
    SwitchWidget *powerSwitch = new SwitchWidget(nullptr, m_titleEdit);
    //把动画按钮放在蓝牙开关前面
    powerSwitch->getMainLayout()->insertWidget(1, spinnerBtn, Qt::AlignVCenter);
    powerSwitch->setObjectName("powerSwitch");

    powerSwitch->setFixedHeight(36);
    powerSwitch->setContentsMargins(0, 0, 0, 0);
    powerSwitch->setChecked(m_adapter->powered());

    SwitchWidget *discoverySwitch = new SwitchWidget(tr("Allow other Bluetooth devices to find this device"));
    discoverySwitch->leftWidget()->setMinimumWidth(10);
    discoverySwitch->setContentsMargins(0, 0, 0, 0);
    discoverySwitch->setFixedHeight(36);
    discoverySwitch->setObjectName("discoverySwitch");
    discoverySwitch->setChecked(m_adapter->discoverabled());

    settingsGrp->setBackgroundStyle(SettingsGroup::GroupBackground);
    settingsGrp->setContentsMargins(0, 0, 0, 0);
    settingsGrp->layout()->setMargin(0);
    settingsGrp->setSpacing(1);

    settingsGrp->appendItem(powerSwitch);
    settingsGrp->appendItem(discoverySwitch);
    discoverySwitch->setChecked(m_adapter->discoverabled());
    discoverySwitch->setVisible(m_adapter->powered());

    connect(m_titleEdit, &TitleEdit::requestSetBluetoothName, this, [=](const QString &alias) {
        Q_EMIT requestSetAlias(m_adapter, alias);
    });
    connect(m_adapter, &BluetoothAdapter::nameChanged, m_titleEdit, &TitleEdit::setTitle, Qt::QueuedConnection);
    connect(powerSwitch, &SwitchWidget::checkedChanged, this, [this, powerSwitch, discoverySwitch](const bool check) {
        //关闭蓝牙的时候,直接隐藏列表
        if (!check) {
            discoverySwitch->setVisible(false);
            updateVisible(false, false);
            if (m_adapter) {
                Q_EMIT m_adapter->closeDetailPage();
            }
        }
        powerSwitch->switchButton()->setEnabled(false);
        Q_EMIT requestSetToggleAdapter(m_adapter, check);
    });
    connect(m_adapter, &BluetoothAdapter::poweredChanged, powerSwitch, [powerSwitch, spinnerBtn] {
        powerSwitch->switchButton()->setEnabled(true);
        spinnerBtn->hide();
    });
    connect(m_adapter, &BluetoothAdapter::loadStatus, powerSwitch, [powerSwitch, spinnerBtn] {
        powerSwitch->switchButton()->setEnabled(false);
        spinnerBtn->show();
    });
    connect(discoverySwitch, &SwitchWidget::checkedChanged, this, &AdapterModule::toggleDiscoverableSwitch);
    connect(m_adapter, &BluetoothAdapter::discoverableChanged, discoverySwitch, [this, discoverySwitch] {
        discoverySwitch->setChecked(m_adapter->discoverabled());
    });
    connect(m_adapter, &BluetoothAdapter::poweredChanged, powerSwitch, [powerSwitch, discoverySwitch](bool bPower, bool) {
        powerSwitch->setEnabled(true);
        powerSwitch->setChecked(bPower);
        powerSwitch->setVisible(true);
        discoverySwitch->setEnabled(true);
        discoverySwitch->setVisible(bPower);
    });
}

void AdapterModule::initDeviceListView(DCCListView *deviceListView)
{
    deviceListView->setAccessibleName("List_mydevicelist");
    deviceListView->setObjectName("myDeviceListView");
    deviceListView->setFrameShape(QFrame::NoFrame);
    BluetoothDeviceModel *model = new BluetoothDeviceModel(m_adapter, true, deviceListView);
    deviceListView->setModel(model);
    deviceListView->setItemDelegate(new BluetoothDeviceDelegate(deviceListView));
    deviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    deviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    deviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    deviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    deviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    deviceListView->setViewportMargins(0, 0, 0, 0);

    connect(model, &BluetoothDeviceModel::requestSetDevAlias, this, &AdapterModule::requestSetDevAlias);
    connect(model, &BluetoothDeviceModel::detailClick, this, [this, deviceListView](const BluetoothDevice *device) {
        contextMenu(m_adapter, device, deviceListView);
    });
    connect(deviceListView, &DListView::clicked, this, [this](const QModelIndex &idx) {
        const BluetoothDevice *device = static_cast<const BluetoothDevice *>(idx.internalPointer());
        if (device->state() != BluetoothDevice::StateConnected) {
            Q_EMIT requestConnectDevice(device, m_adapter);
        }
    });

    connect(deviceListView, &DListView::activated, deviceListView, &DListView::clicked);
}

void AdapterModule::initAnonymousCheckBox(QWidget *w)
{
    QCheckBox *showAnonymousCheckBox = new QCheckBox(w);
    showAnonymousCheckBox->setAccessibleName("AnonymousCheckBox");
    showAnonymousCheckBox->setChecked(m_model->displaySwitch());

    showAnonymousCheckBox->setText(tr("Show Bluetooth devices without names"));

    showAnonymousCheckBox->setFixedHeight(36);
    showAnonymousCheckBox->setMinimumWidth(10);

    DSpinner *spinner = new DSpinner(w);
    spinner->setFixedSize(24, 24);
    spinner->start();
    spinner->setVisible(m_adapter->discovering());

    DIconButton *refreshBtn = new DIconButton(w);
    refreshBtn->setFixedSize(36, 36);
    refreshBtn->setIcon(DIconTheme::findQIcon("dcc_refresh"));
    refreshBtn->setVisible(!m_adapter->discovering());

    QHBoxLayout *phlayoutShowAnonymous = new QHBoxLayout;
    phlayoutShowAnonymous->addWidget(showAnonymousCheckBox);
    phlayoutShowAnonymous->addStretch();
    phlayoutShowAnonymous->addWidget(spinner);
    phlayoutShowAnonymous->addWidget(refreshBtn);

    connect(refreshBtn, &DIconButton::clicked, this, [this] {
        Q_EMIT requestRefresh(m_adapter);
    });
    connect(m_model, &BluetoothModel::displaySwitchChanged, showAnonymousCheckBox, &QCheckBox::setChecked);
    connect(showAnonymousCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
        if (state == Qt::CheckState::Unchecked) {
            if (m_model->displaySwitch()) {
                Q_EMIT requestSetDisplaySwitch(false);
            }
        } else {
            if (!m_model->displaySwitch()) {
                Q_EMIT requestSetDisplaySwitch(true);
            }
        }
    });
    connect(m_adapter, &BluetoothAdapter::poweredChanged, spinner, [spinner, refreshBtn](bool bPower, bool bDiscovering) {
        spinner->setVisible(bPower && bDiscovering);
        refreshBtn->setVisible(bPower && !bDiscovering);
    });
    w->setLayout(phlayoutShowAnonymous);
}

void AdapterModule::initOtherDeviceListView(DCCListView *otherDeviceListView)
{
    otherDeviceListView->setAccessibleName("List_otherdevicelist");
    otherDeviceListView->setObjectName("otherDeviceListView");
    otherDeviceListView->setFrameShape(QFrame::NoFrame);
    BluetoothDeviceModel *model = new BluetoothDeviceModel(m_adapter, false, otherDeviceListView);
    otherDeviceListView->setModel(model);
    otherDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    otherDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    otherDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    otherDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    otherDeviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    otherDeviceListView->setViewportMargins(0, 0, 0, 0);
    model->showAnonymous(m_model->displaySwitch());

    connect(m_model, &BluetoothModel::displaySwitchChanged, model, &BluetoothDeviceModel::showAnonymous);

    connect(otherDeviceListView, &DListView::clicked, this, [this](const QModelIndex &idx) {
        const BluetoothDevice *device = static_cast<const BluetoothDevice *>(idx.internalPointer());
        if (device->state() != BluetoothDevice::StateConnected) {
            Q_EMIT requestConnectDevice(device, m_adapter);
        }
    });

    connect(otherDeviceListView, &DListView::activated, otherDeviceListView, &DListView::clicked);
}

void AdapterModule::setAdapter(const BluetoothAdapter *adapter)
{
    connect(adapter, &BluetoothAdapter::poweredChanged, this, &AdapterModule::updateVisible, Qt::QueuedConnection);
    connect(adapter, &BluetoothAdapter::deviceAdded, this, &AdapterModule::deviceChanged, Qt::QueuedConnection);
    connect(adapter, &BluetoothAdapter::deviceRemoved, this, &AdapterModule::deviceChanged, Qt::QueuedConnection);
    deviceChanged();
    updateVisible(adapter->powered(), adapter->discovering());
}

bool AdapterModule::getSwitchState()
{
    return m_adapter->powered();
}

void AdapterModule::toggleDiscoverableSwitch(const bool checked)
{
    Q_EMIT requestDiscoverable(m_adapter, checked);
}

void AdapterModule::updateVisible(bool bPower, bool bDiscovering)
{
    // NOTE: maybe it should be used?
    Q_UNUSED(bDiscovering)
    m_bluetoothTitle->setHidden(bPower);
    m_otherDevices->setHidden(!bPower);
    m_anonymousCheckBox->setHidden(!bPower);
    m_otherdevicelist->setHidden(!bPower);
    m_devicesTitle->setHidden(!bPower || !m_hasPaired);
    m_mydevicelist->setHidden(!bPower || !m_hasPaired);
}

void AdapterModule::contextMenu(const BluetoothAdapter *adapter, const BluetoothDevice *device, DCCListView *view)
{
    QMenu *menu = new QMenu(view);
    menu->setAccessibleName("DetailMenu");
    QAction *connectAction = menu->addAction(tr("Connect"));
    QAction *disconnectAction = menu->addAction(tr("Disconnect"));
    QAction *renameAction = menu->addAction(tr("Rename"));
    QAction *transfileAction = menu->addAction(tr("Send Files"));
    menu->addSeparator();
    QAction *ignoreAction = menu->addAction(tr("Ignore this device"));
    connectAction->setObjectName("connectAction");
    switch (device->state()) {
    case BluetoothDevice::StateConnected:
        if (device->connectState()) {
            menu->removeAction(connectAction);
        } else {
            menu->removeAction(disconnectAction);
            menu->removeAction(transfileAction);
            menu->removeAction(ignoreAction);
            ignoreAction->setEnabled(false);
        }
        break;
    case BluetoothDevice::StateAvailable:
        connectAction->setText(tr("Connecting"));
        menu->removeAction(disconnectAction);
        menu->removeAction(transfileAction);
        ignoreAction->setEnabled(false);
        break;
    case BluetoothDevice::StateDisconnecting:
        connectAction->setText(tr("Disconnecting"));
        menu->removeAction(disconnectAction);
        menu->removeAction(transfileAction);
        ignoreAction->setEnabled(false);
        break;
    case BluetoothDevice::StateUnavailable:
        menu->removeAction(disconnectAction);
        menu->removeAction(transfileAction);
        break;
    }
    if (!device->canSendFile())
        menu->removeAction(transfileAction);
    QAction *action = menu->exec(QCursor::pos());
    if (action == nullptr) {
        // 没有不处理，但要先判断
    } else if (action == connectAction) {
        Q_EMIT requestConnectDevice(device, adapter);
    } else if (action == disconnectAction) {
        Q_EMIT requestDisconnectDevice(device);
    } else if (action == renameAction) {
        BluetoothDeviceModel *model = qobject_cast<BluetoothDeviceModel *>(view->model());
        view->edit(model->index(device));
    } else if (action == transfileAction) {
        QFileDialog *transFile = new QFileDialog(view);
        transFile->setModal(true);
        transFile->setFileMode(QFileDialog::ExistingFiles);
        transFile->setAcceptMode(QFileDialog::AcceptOpen);
        transFile->setDirectory(QDir::homePath());
        connect(transFile, &QFileDialog::finished, this, [this, transFile, device](int result) {
            if (result == QFileDialog::Accepted) {
                QStringList selectedFiles = transFile->selectedFiles();
                if (selectedFiles.count() <= 0)
                    return;
                // 蓝牙传输dbus接口: SendFile(destination string, filename string) 接口支持文件多选
                m_work->showBluetoothTransDialog(device->address(), selectedFiles);
            }
        });
        transFile->show();
    } else if (action == ignoreAction) {
        Q_EMIT requestIgnoreDevice(adapter, device);
    }
    menu->deleteLater();
}

void AdapterModule::deviceChanged()
{
    bool hasPaired = false;
    for (auto &&dev : m_adapter->devices()) {
        hasPaired |= dev->paired();
        if (!m_devices.contains(dev)) {
            connect(dev, &BluetoothDevice::pairedChanged, this, &AdapterModule::deviceChanged, Qt::QueuedConnection);
            connect(dev, &BluetoothDevice::destroyed, this, [this]() {
                m_devices.remove(qobject_cast<const BluetoothDevice *>(sender()));
            });
            m_devices.insert(dev);
        }
    }
    if (hasPaired != m_hasPaired) {
        m_hasPaired = hasPaired;
        updateVisible(m_adapter->powered(), m_adapter->discovering());
    }
}
