/*
* Copyright (C) 2021 ~ 2023 Deepin Technology Co., Ltd.
*
* Author:     caixiangrong <caixiangrong@uniontech.com>
*
* Maintainer: caixiangrong <caixiangrong@uniontech.com>
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
#include "adaptermodule.h"
#include "widgetmodule.h"
#include "bluetoothdevicemodel.h"
#include "bluetoothworker.h"
#include "widgets/settingsgroup.h"

#include <DIconButton>
#include <DListView>
#include <DSwitchButton>
#include <DSpinner>

#include <QDBusConnection>
#include <QDBusInterface>
#include <QHBoxLayout>
#include <QTimer>
#include <QStyle>
#include <QApplication>
#include <QMenu>
#include <QFileDialog>
#include <QCheckBox>

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

AdapterModule::AdapterModule(const BluetoothAdapter *adapter, BluetoothModel *model, BluetoothWorker *work, QObject *parent)
    : QObject(parent)
    , m_adapter(adapter)
    , m_model(model)
    , m_work(work)
    , m_powerSwitch(nullptr)
    , m_showAnonymousCheckBox(nullptr)
    , m_myDeviceListView(nullptr)
    , m_spinner(nullptr)
    , m_otherDeviceListView(nullptr)
    , m_refreshBtn(nullptr)
    , m_discoverySwitch(nullptr)
{
    deactive();
    m_moduleList.append(new WidgetModule<SettingsGroup>("bluetoothTitle", tr("Allow other Bluetooth devices to find this device"), this, &AdapterModule::initBluetoothTitle));
    m_moduleList.append(new WidgetModule<QLabel>("bluetoothTitle", tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)"), [this](QLabel *w) {
        m_tip = w;
        m_tip->setText(tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)"));
        m_tip->setWordWrap(true);
        m_tip->setContentsMargins(16, 0, 10, 0);
        m_tip->setVisible(!m_adapter->powered());
    }));
    m_moduleList.append(new WidgetModule<TitleLabel>("DevicesGroup", tr("My Devices"), [this](TitleLabel *w) {
        m_myDevicesTitle = w;
        w->setText(tr("My Devices"));
        DFontSizeManager::instance()->bind(w, DFontSizeManager::T5, QFont::DemiBold); // 设置字体
        w->setVisible(m_adapter->powered());
    }));
    m_moduleList.append(new WidgetModule<DListView>("List_mydevicelist", tr("My Devices"), this, &AdapterModule::initDeviceListView));
    m_moduleList.append(new WidgetModule<TitleLabel>("Other Devices", tr("Other Devices"), [this](TitleLabel *w) {
        m_otherDevicesTitle = w;
        w->setText(tr("Other Devices"));
        DFontSizeManager::instance()->bind(w, DFontSizeManager::T5, QFont::DemiBold); // 设置字体
        w->setVisible(m_adapter->powered());
        connect(m_adapter, &BluetoothAdapter::poweredChanged, w, &TitleLabel::setVisible, Qt::QueuedConnection);
    }));
    m_moduleList.append(new WidgetModule<QWidget>("AnonymousCheckBox", tr("Other Devices"), this, &AdapterModule::initAnonymousCheckBox));
    m_moduleList.append(new WidgetModule<DListView>("List_otherdevicelist", "", this, &AdapterModule::initOtherDeviceListView));

    setAdapter(m_adapter);
}

AdapterModule::~AdapterModule()
{
}

void AdapterModule::deactive()
{
    m_powerSwitch = nullptr;
    m_showAnonymousCheckBox = nullptr;
    m_myDeviceListView = nullptr;
    m_spinner = nullptr;
    m_otherDeviceListView = nullptr;
    m_refreshBtn = nullptr;
    m_discoverySwitch = nullptr;
    m_myDevicesTitle = nullptr;
    m_otherDevicesTitle = nullptr;
}

const QList<DCC_NAMESPACE::ModuleObject *> &AdapterModule::ModuleList() const
{
    return m_moduleList;
}

void AdapterModule::initBluetoothTitle(DCC_NAMESPACE::SettingsGroup *settingsGrp)
{
    m_titleEdit = new TitleEdit;
    m_titleEdit->setTitle(m_adapter->name());
    m_spinnerBtn = new DSpinner(m_titleEdit);
    m_spinnerBtn->setFixedSize(24, 24);
    m_spinnerBtn->start();
    m_spinnerBtn->hide();
    m_titleEdit->setMinimumWidth(10);
    m_powerSwitch = new SwitchWidget(nullptr, m_titleEdit);
    //把动画按钮放在蓝牙开关前面
    m_powerSwitch->getMainLayout()->insertWidget(1, m_spinnerBtn, Qt::AlignVCenter);
    m_powerSwitch->setObjectName("powerSwitch");

    m_powerSwitch->setFixedHeight(36);
    m_powerSwitch->setContentsMargins(0, 0, 0, 0);

    m_discoverySwitch = new SwitchWidget(tr("Allow other Bluetooth devices to find this device"));
    m_discoverySwitch->leftWidget()->setMinimumWidth(10);
    m_discoverySwitch->setContentsMargins(0, 0, 0, 0);
    m_discoverySwitch->setFixedHeight(36);
    m_discoverySwitch->setObjectName("discoverySwitch");
    m_discoverySwitch->setChecked(m_adapter->discoverabled());

    settingsGrp->setBackgroundStyle(SettingsGroup::GroupBackground);
    settingsGrp->setContentsMargins(0, 0, 0, 0);
    settingsGrp->layout()->setMargin(0);
    settingsGrp->setSpacing(1);

    settingsGrp->appendItem(m_powerSwitch);
    settingsGrp->appendItem(m_discoverySwitch);
    m_discoverySwitch->setChecked(m_adapter->discoverabled());

    connect(m_titleEdit, &TitleEdit::requestSetBluetoothName, this, [=](const QString &alias) {
        Q_EMIT requestSetAlias(m_adapter, alias);
    });
    connect(m_adapter, &BluetoothAdapter::nameChanged, m_titleEdit, &TitleEdit::setTitle, Qt::QueuedConnection);
    connect(m_powerSwitch, &SwitchWidget::checkedChanged, this, [=](const bool check) {
        //关闭蓝牙的时候,直接隐藏列表
        if (!check) {
            onPowerStatus(false, false);
            if (m_adapter) {
                Q_EMIT m_adapter->closeDetailPage();
            }
        }
        m_powerSwitch->switchButton()->setEnabled(false);
        Q_EMIT requestSetToggleAdapter(m_adapter, check);
    });
    connect(m_adapter,&BluetoothAdapter::poweredChanged,m_powerSwitch, [=] {
        m_powerSwitch->switchButton()->setEnabled(true);
        m_spinnerBtn->hide();
    });
    connect(m_adapter, &BluetoothAdapter::loadStatus, m_powerSwitch, [=] {
        m_powerSwitch->switchButton()->setEnabled(false);
        m_spinnerBtn->show();
    });
    connect(m_discoverySwitch, &SwitchWidget::checkedChanged, this, &AdapterModule::toggleDiscoverableSwitch);
    connect(m_adapter, &BluetoothAdapter::discoverableChanged, m_discoverySwitch, [=] {
        m_discoverySwitch->setChecked(m_adapter->discoverabled());
    });
}

void AdapterModule::initDeviceListView(DListView *deviceListView)
{
    m_myDeviceListView = deviceListView;
    m_myDeviceListView->setAccessibleName("List_mydevicelist");
    m_myDeviceListView->setObjectName("myDeviceListView");
    m_myDeviceListView->setFrameShape(QFrame::NoFrame);
    BluetoothDeviceModel *model = new BluetoothDeviceModel(m_adapter, true, m_myDeviceListView);
    m_myDeviceListView->setModel(model);
    m_myDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_myDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_myDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_myDeviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_myDeviceListView->setViewportMargins(0, 0, 0, 0);

    m_myDeviceListView->setVisible(m_adapter->powered());
    connect(m_model, &BluetoothModel::notifyMyDeviceVisibleChanged, m_myDeviceListView, &DListView::setVisible, Qt::QueuedConnection);
    connect(model, &BluetoothDeviceModel::requestSetDevAlias, this, &AdapterModule::requestSetDevAlias);
    connect(model, &BluetoothDeviceModel::detailClick, this, [this](const BluetoothDevice *device) {
        contextMenu(m_adapter, device);
    });
    connect(m_myDeviceListView, &DListView::clicked, this, [this](const QModelIndex &idx) {
        m_otherDeviceListView->clearSelection();
        const BluetoothDevice *device = static_cast<const BluetoothDevice *>(idx.internalPointer());
        if (device->state() != BluetoothDevice::StateConnected) {
            Q_EMIT requestConnectDevice(device, m_adapter);
        }
    });

    connect(m_myDeviceListView, &DListView::activated, m_myDeviceListView, &DListView::clicked);
}

void AdapterModule::initAnonymousCheckBox(QWidget *w)
{
    m_showAnonymousCheckBox = new QCheckBox(w);
    m_showAnonymousCheckBox->setAccessibleName("AnonymousCheckBox");
    m_showAnonymousCheckBox->setChecked(m_model->displaySwitch());

    m_showAnonymousCheckBox->setText(tr("Show Bluetooth devices without names"));

    m_showAnonymousCheckBox->setFixedHeight(36);
    m_showAnonymousCheckBox->setMinimumWidth(10);

    m_spinner = new DSpinner(w);
    m_spinner->setFixedSize(24, 24);
    m_spinner->start();
    m_spinner->setVisible(false);

    m_refreshBtn = new DIconButton(w);
    m_refreshBtn->setFixedSize(36, 36);
    m_refreshBtn->setIcon(QIcon::fromTheme("dcc_refresh"));

    QHBoxLayout *phlayoutShowAnonymous = new QHBoxLayout;
    phlayoutShowAnonymous->addWidget(m_showAnonymousCheckBox);
    phlayoutShowAnonymous->addStretch();
    phlayoutShowAnonymous->addWidget(m_spinner);
    phlayoutShowAnonymous->addWidget(m_refreshBtn);

    connect(m_refreshBtn, &DIconButton::clicked, this, [=] {
        Q_EMIT requestRefresh(m_adapter);
    });
    connect(m_model, &BluetoothModel::displaySwitchChanged, m_showAnonymousCheckBox, &QCheckBox::setChecked);
    connect(m_showAnonymousCheckBox, &QCheckBox::stateChanged, this, [=](int state) {
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

    w->setLayout(phlayoutShowAnonymous);
}

void AdapterModule::initOtherDeviceListView(DTK_WIDGET_NAMESPACE::DListView *otherDeviceListView)
{
    m_otherDeviceListView = otherDeviceListView;
    m_otherDeviceListView->setAccessibleName("List_otherdevicelist");
    m_otherDeviceListView->setObjectName("otherDeviceListView");
    m_otherDeviceListView->setFrameShape(QFrame::NoFrame);
    BluetoothDeviceModel *model = new BluetoothDeviceModel(m_adapter, false, m_otherDeviceListView);
    m_otherDeviceListView->setModel(model);
    m_otherDeviceListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_otherDeviceListView->setBackgroundType(DStyledItemDelegate::BackgroundType::ClipCornerBackground);
    m_otherDeviceListView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_otherDeviceListView->setSelectionMode(QAbstractItemView::NoSelection);
    m_otherDeviceListView->setViewportMargins(0, 0, 0, 0);

    m_otherDeviceListView->setVisible(m_adapter->powered());
    connect(m_model, &BluetoothModel::displaySwitchChanged, model, &BluetoothDeviceModel::showAnonymous);
    connect(m_model, &BluetoothModel::notifyOtherDeviceVisibleChanged, m_otherDeviceListView, &DListView::setVisible, Qt::QueuedConnection);

    connect(m_otherDeviceListView, &DListView::clicked, this, [this](const QModelIndex &idx) {
        m_myDeviceListView->clearSelection();
        const BluetoothDevice *device = static_cast<const BluetoothDevice *>(idx.internalPointer());
        if (device->state() != BluetoothDevice::StateConnected) {
            Q_EMIT requestConnectDevice(device, m_adapter);
        }
    });

    connect(m_otherDeviceListView, &DListView::activated, m_otherDeviceListView, &DListView::clicked);
    onPowerStatus(m_adapter->powered(), m_adapter->discovering());
}

void AdapterModule::setAdapter(const BluetoothAdapter *adapter)
{
    connect(adapter, &BluetoothAdapter::poweredChanged, this, &AdapterModule::onPowerStatus, Qt::QueuedConnection);
    onPowerStatus(adapter->powered(), adapter->discovering());
}

void AdapterModule::onPowerStatus(bool bPower, bool bDiscovering)
{
    Q_EMIT notifyLoadFinished();
    if (!m_powerSwitch || !m_discoverySwitch || !m_tip || !m_showAnonymousCheckBox
        || !m_spinner || !m_refreshBtn || !m_myDeviceListView || !m_otherDeviceListView)
        return;
    m_powerSwitch->setEnabled(true);
    m_powerSwitch->setChecked(bPower);
    m_powerSwitch->setVisible(true);
    m_discoverySwitch->setEnabled(true);
    m_discoverySwitch->setVisible(bPower);
    m_tip->setVisible(!bPower);

    m_showAnonymousCheckBox->setVisible(bPower);
    m_spinner->setVisible(bPower && bDiscovering);
    m_refreshBtn->setVisible(bPower && !bDiscovering);
    m_myDeviceListView->setVisible(bPower);
    m_myDevicesTitle->setVisible(bPower);
    m_otherDeviceListView->setVisible(bPower);
    m_otherDevicesTitle->setVisible(bPower);
}

bool AdapterModule::getSwitchState()
{
    return m_adapter->powered();
}

void AdapterModule::toggleDiscoverableSwitch(const bool checked)
{
    Q_EMIT requestDiscoverable(m_adapter, checked);
}

void AdapterModule::contextMenu(const BluetoothAdapter *adapter, const BluetoothDevice *device)
{
    QMenu *menu = new QMenu(m_myDeviceListView);
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
        }
        break;
    case BluetoothDevice::StateAvailable:
        connectAction->setText(tr("Connecting"));
        menu->removeAction(disconnectAction);
        menu->removeAction(transfileAction);
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
    QAction *action = menu->exec(QCursor::pos());
    if (action == nullptr) {
        // 没有不处理，但要先判断
    } else if (action == connectAction) {
        Q_EMIT requestConnectDevice(device, adapter);
    } else if (action == disconnectAction) {
        Q_EMIT requestDisconnectDevice(device);
    } else if (action == renameAction) {
        BluetoothDeviceModel *model = qobject_cast<BluetoothDeviceModel *>(m_myDeviceListView->model());
        m_myDeviceListView->edit(model->index(device));
    } else if (action == transfileAction) {
        QFileDialog *transFile = new QFileDialog(m_myDeviceListView);
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
