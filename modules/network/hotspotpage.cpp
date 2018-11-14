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

#include "hotspotpage.h"
#include "nextpagewidget.h"
#include "switchwidget.h"
#include "settingsgroup.h"

#include <QVBoxLayout>
#include <QDebug>

#include <unistd.h>
#include <networkmodel.h>
#include <wirelessdevice.h>

using namespace dde::network;

namespace dcc {

using namespace widgets;

namespace network {

const QString defaultHotspotName()
{
    return getlogin();
}

HotspotPage::HotspotPage(WirelessDevice *wdev, QWidget *parent)
    : ContentWidget(parent),
      m_wdev(wdev)
{
    Q_ASSERT(m_wdev->supportHotspot());

    m_hotspotSwitch = new SwitchWidget;
    m_hotspotSwitch->setTitle("aaa");

    m_configureWidget = new NextPageWidget;
    m_configureWidget->setTitle(tr("Hotspot Settings"));

    SettingsGroup *sgrp = new SettingsGroup;
    sgrp->appendItem(m_hotspotSwitch);
    sgrp->appendItem(m_configureWidget);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->addWidget(sgrp);
    centralLayout->setSpacing(0);
    centralLayout->setContentsMargins(0, 10, 0, 0);

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Hotspot"));

    connect(m_wdev, &WirelessDevice::removed, this, &HotspotPage::onDeviceRemoved);
    connect(m_hotspotSwitch, &SwitchWidget::checkedChanged, this, &HotspotPage::onSwitchToggled);
    connect(m_configureWidget, &NextPageWidget::clicked, this, &HotspotPage::onConfigWidgetClicked);
}

void HotspotPage::setModel(NetworkModel *model)
{
    m_model = model;

    connect(model, &NetworkModel::connectionListChanged, this, &HotspotPage::onConnectionsChanged);
    connect(model, &NetworkModel::activeConnectionsChanged, this, &HotspotPage::onActiveConnsChanged);

    QTimer::singleShot(1, this, &HotspotPage::onConnectionsChanged);
}

void HotspotPage::onSwitchToggled(const bool checked)
{
    if (checked)
        openHotspot();
    else
        closeHotspot();
}

void HotspotPage::onConfigWidgetClicked()
{
    const QString uuid = hotspotUuid();
    Q_ASSERT(!uuid.isEmpty());

    m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
    m_editPage->initSettingsWidget();

    connect(m_editPage, &ConnectionHotspotEditPage::requestNextPage, this, &HotspotPage::requestNextPage);

    Q_EMIT requestNextPage(m_editPage);
}

void HotspotPage::onConnectionsChanged()
{
    if (!m_wdev || !m_wdev->enabled()) {
        return;
    }

    m_hotspotInfo = QJsonObject();
    for (const auto &hotspot : m_model->hotspots())
    {
        const QString &hwAddress = hotspot.value("HwAddress").toString();
        if (hwAddress == m_wdev->usingHwAdr() || hwAddress == m_wdev->realHwAdr())
        {
            m_hotspotInfo = hotspot;
            break;
        }
    }

    const QString ssid = m_hotspotInfo.value("Ssid").toString();
    m_hotspotSwitch->setTitle(ssid.isEmpty() ? defaultHotspotName() : ssid);
    m_configureWidget->setVisible(!m_hotspotInfo.isEmpty());

    QTimer::singleShot(1, this, &HotspotPage::onActiveConnsChanged);
}

void HotspotPage::onActiveConnsChanged()
{
    m_hotspotSwitch->blockSignals(true);

    if (m_hotspotInfo.isEmpty())
        m_hotspotSwitch->setChecked(false);
    else
        m_hotspotSwitch->setChecked(m_model->activeConnections().contains(hotspotUuid()));

    m_hotspotSwitch->blockSignals(false);
}

void HotspotPage::closeHotspot()
{
    const QString uuid = hotspotUuid();
    Q_ASSERT(!uuid.isEmpty());

    emit requestDisconnectConnection(uuid);
    emit requestDeviceRemanage(m_wdev->path());
}

void HotspotPage::openHotspot()
{
    const QString uuid = hotspotUuid();

    if (uuid.isEmpty()) {
        m_hotspotSwitch->setChecked(false);

        m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
        m_editPage->initSettingsWidget();

        connect(m_editPage, &ConnectionHotspotEditPage::requestNextPage, this, &HotspotPage::requestNextPage);

        Q_EMIT requestNextPage(m_editPage);
    } else
        requestActivateConnection(m_wdev->path(), uuid);
}

void HotspotPage::onDeviceRemoved()
{
    // back if ap edit page exist
    if (!m_editPage.isNull()) {
        m_editPage->onDeviceRemoved();
    }

    // destroy self page
    emit back();
}

}

}
