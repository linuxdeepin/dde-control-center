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

#include "bluetoothmodule.h"
#include "bluetoothwidget.h"
#include "detailpage.h"
#include "modules/bluetooth/adapter.h"
#include "modules/bluetooth/bluetoothmodel.h"
#include "modules/bluetooth/bluetoothworker.h"
#include "modules/bluetooth/pincodedialog.h"

#include <QDebug>

using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;
using namespace dcc;
using namespace dcc::bluetooth;
BluetoothModule::BluetoothModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent)
    , ModuleInterface(frame)
    , m_bluetoothWidget(nullptr)
    , m_bluetoothModel(nullptr)
    , m_bluetoothWorker(nullptr)
{

}

void BluetoothModule::preInitialize(bool sync , FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    m_bluetoothWorker = &BluetoothWorker::Instance(sync);
    m_bluetoothModel = m_bluetoothWorker->model();
    m_bluetoothModel->moveToThread(qApp->thread());
    m_bluetoothWorker->moveToThread(qApp->thread());

    auto updateModuleVisible = [ = ] {
        qDebug() << QString("adapters size(%1) : %2")
                    .arg(sender() ? "list change" : "first")
                    .arg(m_bluetoothModel->adapters().size());
        bool visible = m_bluetoothModel->adapters().size();
        m_frameProxy->setModuleVisible(this, visible);
        setDeviceUnavailabel(!visible);
    };

    connect(m_bluetoothModel, &BluetoothModel::adpaterListChanged, this, updateModuleVisible);

    addChildPageTrans();
    initSearchData();
    updateModuleVisible();
}

void BluetoothModule::initialize()
{
    connect(m_bluetoothWorker, &BluetoothWorker::requestConfirmation, this, &BluetoothModule::showPinCode);
    connect(m_bluetoothWorker, &BluetoothWorker::pinCodeCancel, this, &BluetoothModule::closePinCode);
}

void BluetoothModule::reset()
{
}

void BluetoothModule::active()
{
    m_bluetoothWidget = new BluetoothWidget(m_bluetoothModel);
    m_bluetoothWidget->setVisible(false);
    connect(m_bluetoothWidget, &BluetoothWidget::requestSetToggleAdapter, m_bluetoothWorker, &BluetoothWorker::setAdapterPowered);
    connect(m_bluetoothWidget, &BluetoothWidget::requestConnectDevice, m_bluetoothWorker, &BluetoothWorker::connectDevice);
    connect(m_bluetoothWidget, &BluetoothWidget::requestDisconnectDevice, m_bluetoothWorker, &BluetoothWorker::disconnectDevice);
    connect(m_bluetoothWidget, &BluetoothWidget::requestSetAlias, m_bluetoothWorker, &BluetoothWorker::setAlias);
    connect(m_bluetoothWidget, &BluetoothWidget::showDeviceDetail, this, &BluetoothModule::showDeviceDetail);
    connect(m_bluetoothWidget, &BluetoothWidget::requestModuleVisible, [this](const bool visible) {
        m_frameProxy->setModuleVisible(this, visible);
        setDeviceUnavailabel(!visible);
    });
    m_frameProxy->pushWidget(this, m_bluetoothWidget);
    connect(m_bluetoothWidget, &BluetoothWidget::requestDiscoverable, m_bluetoothWorker, &BluetoothWorker::onRequestSetDiscoverable);
    m_bluetoothWidget->setVisible(true);
}

const QString BluetoothModule::name() const
{
    return QStringLiteral("bluetooth");
}

const QString BluetoothModule::displayName() const
{
    return tr("Bluetooth");
}

int BluetoothModule::load(const QString &path)
{
    if ((path == QStringLiteral("Ignore this device"))
            || (path == QStringLiteral("Disconnect"))
            || (path == QStringLiteral("Change Name"))) {
        m_bluetoothWidget->loadDetailPage();
        return 0;
    }

    return -1;
}

void BluetoothModule::addChildPageTrans() const
{
    if (m_frameProxy != nullptr) {
        //bluetooth
        m_frameProxy->addChildPageTrans("My Devices", tr("My Devices"));
        m_frameProxy->addChildPageTrans("Other Devices", tr("Other Devices"));
        m_frameProxy->addChildPageTrans("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)", tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)"));
    }
}

void BluetoothModule::initSearchData()
{
    if (!m_frameProxy || !m_bluetoothModel) {
        return;
    }

    const QString& module = displayName();
    const QString& myDevices = tr("My Devices");
    const QString& otherDevices = tr("Other Devices");
    const QString& explain = tr("Enable Bluetooth to find nearby devices (speakers, keyboard, mouse)");

    static QMap<QString, bool> gsMap = {
        {myDevices, false},
        {otherDevices, false},
        {explain, false},
    };

    auto setSearchState = [ = ](QString data, bool visible, bool first = false) {

        if (!gsMap.contains(data)) {
            return;
        }

        if (gsMap.value(data) == visible) {
            return;
        }

        if (!m_frameProxy) {
            return;
        }

        //暂无方法获取蓝牙是否开启，如果有可以配合visible一起使用
        if (myDevices == data) {
            m_frameProxy->setWidgetVisible(module, myDevices, visible);
            gsMap.insert(data, visible);
        } else if (otherDevices == data) {
            m_frameProxy->setWidgetVisible(module, otherDevices, visible);
            gsMap.insert(data, visible);
        } else if (explain == data) {
            m_frameProxy->setWidgetVisible(module, explain, visible);
            gsMap.insert(data, visible);
        } else {
            qWarning() << " [setSearchState] not match data : " << data << visible;
        }

        if (!first)
            m_frameProxy->updateSearchData(module);
    };

     auto func_process_all = [ = ](bool first = false) {
         bool bBluetoothModel = m_bluetoothModel->adapters().size() > 0;

         bool powered = false;
         QMap<QString, const Adapter *>::const_iterator i;
         for (i = m_bluetoothModel->adapters().constBegin(); i != m_bluetoothModel->adapters().constEnd(); ++i) {
             if (i.value()->powered()) {
                 powered = true;
                 break;
             }
         }

         m_frameProxy->setModuleVisible(module, bBluetoothModel);
         setSearchState(explain, bBluetoothModel, first);
         setSearchState(myDevices, bBluetoothModel && powered && m_bluetoothModel->myDeviceVisible(), first);
         setSearchState(otherDevices, bBluetoothModel && powered && m_bluetoothModel->otherDeviceVisible(), first);
     };

    connect(m_bluetoothModel, &BluetoothModel::notifyMyDeviceVisibleChanged, this, [=](bool visible) {
        Q_UNUSED(visible);
        func_process_all();
    });

    connect(m_bluetoothModel, &BluetoothModel::notifyOtherDeviceVisibleChanged, this, [=](bool visible) {
        Q_UNUSED(visible);
        func_process_all();
    });

    connect(m_bluetoothModel, &BluetoothModel::adpaterListChanged, this, [=]() {
        func_process_all();
    });

    connect(m_bluetoothModel, &BluetoothModel::adpaterPowerChanged, this, [ = ](const bool powered) {
        Q_UNUSED(powered);
        func_process_all();
    });

    func_process_all(true);
}

void BluetoothModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void BluetoothModule::showDeviceDetail(const Adapter *adapter, const Device *device)
{
    DCC_NAMESPACE::bluetooth::DetailPage *page = new DCC_NAMESPACE::bluetooth::DetailPage(m_bluetoothModel, adapter, device);
    page->setVisible(false);
    connect(page, &DetailPage::requestIgnoreDevice, m_bluetoothWorker, &BluetoothWorker::ignoreDevice);
    connect(page, &DetailPage::requestDisconnectDevice, m_bluetoothWorker, &BluetoothWorker::disconnectDevice);
    connect(page, &DetailPage::requestConnectDevice, m_bluetoothWorker, &BluetoothWorker::connectDevice);
    connect(page, &DetailPage::requestIgnoreDevice, this, &BluetoothModule::popPage);
    connect(page, &DetailPage::back, this, &BluetoothModule::popPage);
    connect(page, &DetailPage::requestSetDevAlias, m_bluetoothWorker, &BluetoothWorker::setDeviceAlias);
    connect(adapter, &Adapter::deviceRemoved, page, &DetailPage::removeDevice);
    connect(page, &DetailPage::requestBack, this, &BluetoothModule::popPage);

    m_frameProxy->pushWidget(this, page);
    page->setVisible(true);
}

void BluetoothModule::popPage()
{
    m_bluetoothWidget->setFocus();
    m_frameProxy->popWidget(this);
}

void BluetoothModule::showPinCode(const QDBusObjectPath &device, const QString &code)
{
    qDebug() << "request confirmation: " << device.path() << code;

    PinCodeDialog *dialog = PinCodeDialog::instance(code);
    m_dialogs[device] = dialog;
    if (!dialog->isVisible()) {
        int ret = dialog->exec();
        closePinCode(device);
        m_bluetoothWorker->pinCodeConfirm(device, bool(ret));
    }
}

void BluetoothModule::closePinCode(const QDBusObjectPath &device)
{
    PinCodeDialog *dialog = m_dialogs[device];
    m_dialogs.remove(device);
    QMetaObject::invokeMethod(dialog, "deleteLater", Qt::QueuedConnection);
}
