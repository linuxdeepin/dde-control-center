/*
 * Copyright (C) 2011 ~ 2017 Deepin Technology Co., Ltd.
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

#include "bluetoothmodule.h"
#include "bluetoothwidget.h"
#include "bluetoothmodel.h"
#include "bluetoothworker.h"
#include "adapterwidget.h"
#include "contentwidget.h"
#include "detailpage.h"

namespace dcc {
namespace bluetooth {

BluetoothModule::BluetoothModule(FrameProxyInterface *frame, QObject *parent)
    : QObject(parent),
    ModuleInterface(frame),

    m_bluetoothView(nullptr),
    m_bluetoothModel(nullptr),
    m_bluetoothWorker(nullptr)
{

}

void BluetoothModule::showPage(const QString &pageName)
{
    Q_UNUSED(pageName)

    showBluetoothDetail(m_bluetoothModel->adapters().first());
}

void BluetoothModule::showDetail(const Adapter *adapter, const Device *device)
{
    DetailPage *page = new DetailPage(adapter, device);

    connect(page, &DetailPage::requestIgnoreDevice, m_bluetoothWorker, &BluetoothWorker::ignoreDevice);
    connect(page, &DetailPage::requestDisconnectDevice, m_bluetoothWorker, &BluetoothWorker::disconnectDevice);

    m_frameProxy->pushWidget(this, page);
}

void BluetoothModule::showBluetoothDetail(const Adapter *adapter)
{
    AdapterWidget *w = new AdapterWidget(adapter);

    connect(w, &AdapterWidget::requestSetToggleAdapter, m_bluetoothWorker, &BluetoothWorker::setAdapterPowered);
    connect(w, &AdapterWidget::requestConnectDevice, m_bluetoothWorker, &BluetoothWorker::connectDevice);
    connect(w, &AdapterWidget::requestShowDetail, this, &BluetoothModule::showDetail);
    connect(w, &AdapterWidget::requestSetAlias, m_bluetoothWorker, &BluetoothWorker::setAlias);

    m_frameProxy->pushWidget(this, w);
}

BluetoothModule::~BluetoothModule()
{
    m_bluetoothModel->deleteLater();
    m_bluetoothWorker->deleteLater();
}

void BluetoothModule::initialize()
{
    m_bluetoothModel = new BluetoothModel;
    m_bluetoothWorker = new BluetoothWorker(m_bluetoothModel);

    m_bluetoothModel->moveToThread(qApp->thread());
    m_bluetoothWorker->moveToThread(qApp->thread());
}

void BluetoothModule::moduleActive()
{
    m_bluetoothWorker->activate();
    // refresh the view
}

void BluetoothModule::moduleDeactive()
{
    m_bluetoothWorker->deactivate();
}

void BluetoothModule::reset()
{

}

void BluetoothModule::contentPopped(ContentWidget * const w)
{
    w->deleteLater();
}

const QString BluetoothModule::name() const
{
    return QStringLiteral("bluetooth");
}

ModuleWidget *BluetoothModule::moduleWidget()
{
    if (!m_bluetoothView)
    {
        m_bluetoothView = new BluetoothWidget(m_bluetoothModel);
        m_bluetoothView->setTitle(tr("Bluetooth"));
        connect(m_bluetoothView, &BluetoothWidget::showBluetoothDetail, this, &BluetoothModule::showBluetoothDetail);
        connect(m_bluetoothView, &BluetoothWidget::requestModuleVisible, [this](const bool visible) { m_frameProxy->setModuleVisible(this, visible); });
    }

    return m_bluetoothView;
}

}
}
