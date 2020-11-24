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

BluetoothModule::~BluetoothModule()
{
    if (m_workerThread) {
        m_workerThread->quit();
        m_workerThread->wait();
    }
}

void BluetoothModule::preInitialize(bool sync, FrameProxyInterface::PushType pushtype)
{
    Q_UNUSED(pushtype);
    m_workerThread = QSharedPointer<QThread>(new QThread);
    m_bluetoothModel = QSharedPointer<BluetoothModel>(new BluetoothModel);
    m_bluetoothWorker = QSharedPointer<BluetoothWorker>(new BluetoothWorker(m_bluetoothModel.get()));
    m_bluetoothWorker->moveToThread(m_workerThread.get());
    m_workerThread->start(QThread::Priority::LowestPriority);

    connect(m_bluetoothModel.get(), &BluetoothModel::adpaterListChanged, this, [=] {
        qDebug() << QString("adapters size(%1) : %2")
                        .arg(sender() ? "list change" : "first")
                        .arg(m_bluetoothModel->adapters().size());
        m_frameProxy->setModuleVisible(this, m_bluetoothModel->adapters().size());
    });

    QMetaObject::invokeMethod(m_bluetoothWorker.get(), "init", Qt::QueuedConnection);
    QMetaObject::invokeMethod(m_bluetoothWorker.get(), "activate", Qt::QueuedConnection);

    Q_EMIT m_bluetoothModel->adpaterListChanged();
}

void BluetoothModule::initialize()
{
    connect(m_bluetoothWorker.get(), &BluetoothWorker::requestConfirmation, this, &BluetoothModule::showPinCode);
    connect(m_bluetoothWorker.get(), &BluetoothWorker::pinCodeCancel, this, &BluetoothModule::closePinCode);
    connect(m_bluetoothModel.get(), &BluetoothModel::adapterAdded, this, [=](const Adapter *adapter) {
        m_bluetoothWorker->setAdapterDiscovering(QDBusObjectPath(adapter->id()), true);
    });
    connect(m_bluetoothModel.get(), &BluetoothModel::adapterRemoved, this, [=](const Adapter *adapter) {
        m_bluetoothWorker->setAdapterDiscovering(QDBusObjectPath(adapter->id()), false);
    });

    QMetaObject::invokeMethod(m_bluetoothWorker.get(), "activate", Qt::QueuedConnection);
}

void BluetoothModule::reset()
{
}

void BluetoothModule::active()
{
    m_bluetoothWidget = new BluetoothWidget(m_bluetoothModel.get());
    m_bluetoothWidget->setVisible(false);
    connect(m_bluetoothWidget, &BluetoothWidget::requestSetToggleAdapter, m_bluetoothWorker.get(), &BluetoothWorker::setAdapterPowered);
    connect(m_bluetoothWidget, &BluetoothWidget::requestConnectDevice, m_bluetoothWorker.get(), &BluetoothWorker::connectDevice);
    connect(m_bluetoothWidget, &BluetoothWidget::requestDisconnectDevice, m_bluetoothWorker.get(), &BluetoothWorker::disconnectDevice);
    connect(m_bluetoothWidget, &BluetoothWidget::requestSetAlias, m_bluetoothWorker.get(), &BluetoothWorker::setAlias);
    connect(m_bluetoothWidget, &BluetoothWidget::showDeviceDetail, this, &BluetoothModule::showDeviceDetail);
    connect(m_bluetoothWidget, &BluetoothWidget::destroyed, this, [=] {
        for (const Adapter *adapter : m_bluetoothModel->adapters()) {
            m_bluetoothWorker->setAdapterDiscovering(QDBusObjectPath(adapter->id()), false);
        }
    });
    connect(m_bluetoothWidget, &BluetoothWidget::requestModuleVisible, [this](const bool visible) {
        m_frameProxy->setModuleVisible(this, visible);
    });
    connect(m_bluetoothWidget, &BluetoothWidget::requestRefresh, [=](const Adapter *adapter) {
        m_bluetoothWorker->setAdapterDiscoverable(adapter->id());
    });

    m_frameProxy->pushWidget(this, m_bluetoothWidget);
    connect(m_bluetoothWidget, &BluetoothWidget::requestDiscoverable, m_bluetoothWorker.get(), &BluetoothWorker::onRequestSetDiscoverable);
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

void BluetoothModule::contentPopped(QWidget *const w)
{
    Q_UNUSED(w);
}

void BluetoothModule::showDeviceDetail(const Adapter *adapter, const Device *device)
{
    DCC_NAMESPACE::bluetooth::DetailPage *page = new DCC_NAMESPACE::bluetooth::DetailPage(m_bluetoothModel.get(), adapter, device);
    page->setVisible(false);
    connect(page, &DetailPage::requestIgnoreDevice, m_bluetoothWorker.get(), &BluetoothWorker::ignoreDevice);
    connect(page, &DetailPage::requestDisconnectDevice, m_bluetoothWorker.get(), &BluetoothWorker::disconnectDevice);
    connect(page, &DetailPage::requestConnectDevice, m_bluetoothWorker.get(), &BluetoothWorker::connectDevice);
    connect(page, &DetailPage::requestIgnoreDevice, this, &BluetoothModule::popPage);
    connect(page, &DetailPage::back, this, &BluetoothModule::popPage);
    connect(page, &DetailPage::requestSetDevAlias, m_bluetoothWorker.get(), &BluetoothWorker::setDeviceAlias);

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
