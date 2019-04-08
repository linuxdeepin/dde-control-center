/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *             kirigaya <kirigaya@mkacg.com>
 *             Hualet <mr.asianwang@gmail.com>
 *             listenerri <listenerri@gmail.com>
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

#include "wiredpage.h"
#include "widgets/settingsgroup.h"
#include "widgets/settingsheaderitem.h"
#include "widgets/nextpagewidget.h"
#include "widgets/tipsitem.h"

#include <QTimer>
#include <QDebug>
#include <QVBoxLayout>
#include <QPushButton>
#include <QPointer>
#include <DHiDPIHelper>

#include <networkmodel.h>
#include <wireddevice.h>

DWIDGET_USE_NAMESPACE

namespace dcc {

namespace network {

using namespace widgets;
using namespace dde::network;

WiredPage::WiredPage(WiredDevice *dev, QWidget *parent)
    : ContentWidget(parent),

      m_device(dev)
{
    m_settingsGrp = new SettingsGroup;
    m_settingsGrp->setHeaderVisible(true);
    m_settingsGrp->headerItem()->setTitle(tr("Setting List"));

    TipsItem *tips = new TipsItem;
    tips->setFixedHeight(80);
    tips->setText(tr("Plug in the network cable first"));

    m_tipsGrp = new SettingsGroup;
    m_tipsGrp->appendItem(tips);

    m_createBtn = new QPushButton;
    m_createBtn->setText(tr("Add Settings"));

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addSpacing(10);
    centralLayout->addWidget(m_tipsGrp);
    centralLayout->addWidget(m_settingsGrp);
    centralLayout->addWidget(m_createBtn);
    centralLayout->addStretch();
    centralLayout->setSpacing(10);
    centralLayout->setMargin(0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Select Settings"));

    connect(m_createBtn, &QPushButton::clicked, this, &WiredPage::createNewConnection);
    connect(m_device, &WiredDevice::connectionsChanged, this, &WiredPage::refreshConnectionList);
    connect(m_device, &WiredDevice::activeWiredConnectionInfoChanged, this, &WiredPage::checkActivatedConnection);
    connect(m_device, static_cast<void (WiredDevice::*)(WiredDevice::DeviceStatus) const>(&WiredDevice::statusChanged), this, &WiredPage::onDeviceStatusChanged);
    connect(m_device, &WiredDevice::removed, this, &WiredPage::onDeviceRemoved);

    onDeviceStatusChanged(m_device->status());
    QTimer::singleShot(1, this, &WiredPage::refreshConnectionList);
}

void WiredPage::setModel(NetworkModel *model)
{
    m_model = model;

    QTimer::singleShot(1, this, &WiredPage::initUI);
}

void WiredPage::initUI()
{
    Q_EMIT requestConnectionsList(m_device->path());
}

void WiredPage::refreshConnectionList()
{
    // get all available wired connections path
    const auto wiredConns = m_model->wireds();

    QSet<QString> availableWiredConns;
    availableWiredConns.reserve(wiredConns.size());

    m_settingsGrp->clear();
    qDeleteAll(m_connectionPath.keys());
    m_connectionPath.clear();

    for (const auto &wiredConn : wiredConns)
    {
        const QString path = wiredConn.value("Path").toString();
        if (!path.isEmpty())
            availableWiredConns << path;
    }

    const auto connObjList = m_device->connections();
    QSet<QString> connPaths;
    for (const auto &connObj : connObjList) {
        const QString &path = connObj.value("Path").toString();
        // pass unavailable wired conns, like 'PPPoE'
        if (!availableWiredConns.contains(path))
            continue;

        connPaths << path;
        if (m_connectionPath.values().contains(path))
            continue;

        NextPageWidget *w = new NextPageWidget;
        w->setTitle(m_model->connectionNameByPath(path));

        connect(w, &NextPageWidget::acceptNextPage, this, &WiredPage::editConnection);
        connect(w, &NextPageWidget::selected, this, &WiredPage::activeConnection);

        m_settingsGrp->appendItem(w);
        m_connectionPath.insert(w, path);
    }

    // clear removed items
    for (auto it(m_connectionPath.begin()); it != m_connectionPath.end();)
    {
        if (!connPaths.contains(it.value()))
        {
            it.key()->deleteLater();
            it = m_connectionPath.erase(it);
        } else {
            ++it;
        }
    }

    checkActivatedConnection();
}

void WiredPage::editConnection()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    const QString connPath = m_connectionPath[w];

    m_editPage = new ConnectionEditPage(ConnectionEditPage::WiredConnection,
            m_device->path(), m_model->connectionUuidByPath(connPath));
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &WiredPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);
}

void WiredPage::createNewConnection()
{
    m_editPage = new ConnectionEditPage(ConnectionEditPage::WiredConnection, m_device->path());
    m_editPage->initSettingsWidget();
    connect(m_editPage, &ConnectionEditPage::requestNextPage, this, &WiredPage::requestNextPage);
    connect(m_editPage, &ConnectionEditPage::requestFrameAutoHide, this, &WiredPage::requestFrameKeepAutoHide);
    Q_EMIT requestNextPage(m_editPage);
}

void WiredPage::activeConnection()
{
    NextPageWidget *w = qobject_cast<NextPageWidget *>(sender());
    Q_ASSERT(w);

    const QString connPath = m_connectionPath[w];

    Q_EMIT requestActiveConnection(m_device->path(), m_model->connectionUuidByPath(connPath));
}

void WiredPage::checkActivatedConnection()
{
    for (auto it(m_connectionPath.cbegin()); it != m_connectionPath.cend(); ++it) {
        if (it.value() == m_device->activeWiredConnSettingPath()) {
            it.key()->setIcon(DHiDPIHelper::loadNxPixmap(":/network/themes/dark/icons/select.svg"));
        } else {
            it.key()->clearValue();
        }
    }
}

void WiredPage::onDeviceStatusChanged(const NetworkDevice::DeviceStatus stat)
{
    const bool unavailable = stat <= NetworkDevice::Unavailable;

    m_tipsGrp->setVisible(unavailable);
}

void WiredPage::onDeviceRemoved()
{
    if (!m_editPage.isNull()) {
        m_editPage->onDeviceRemoved();
    }

    Q_EMIT back();
}

}

}
