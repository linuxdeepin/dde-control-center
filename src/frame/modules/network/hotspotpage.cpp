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
#include "widgets/nextpagewidget.h"
#include "widgets/switchwidget.h"
#include "widgets/settingsgroup.h"

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
      m_wdev(wdev),
      m_hotspotSwitch(new SwitchWidget),
      m_connectionsGroup(new SettingsGroup),
      m_createBtn(new QPushButton),
      m_refreshActiveTimer(new QTimer)
{
    Q_ASSERT(m_wdev->supportHotspot());

    m_hotspotSwitch->setTitle(tr("Hotspot"));
    m_createBtn->setText(tr("Add Settings"));

    m_refreshActiveTimer->setInterval(300);
    m_refreshActiveTimer->setSingleShot(true);

    SettingsGroup *sgrp = new SettingsGroup;
    sgrp->appendItem(m_hotspotSwitch);

    QVBoxLayout *centralLayout = new QVBoxLayout;
    centralLayout->setSpacing(10);
    centralLayout->setContentsMargins(0, 10, 0, 0);

    centralLayout->addWidget(sgrp);
    centralLayout->addWidget(m_connectionsGroup);
    centralLayout->addWidget(m_createBtn);
    centralLayout->addStretch();

    QWidget *centralWidget = new TranslucentFrame;
    centralWidget->setLayout(centralLayout);

    setContent(centralWidget);
    setTitle(tr("Hotspot"));

    connect(m_createBtn, &QPushButton::clicked, this, [=] {openEditPage();});

    connect(m_refreshActiveTimer, &QTimer::timeout, this, &HotspotPage::refreshActiveConnection);

    connect(m_wdev, &WirelessDevice::removed, this, &HotspotPage::onDeviceRemoved);
    connect(m_wdev, &WirelessDevice::hotspotEnabledChanged, this, &HotspotPage::onHotsportEnabledChanged);
    connect(m_wdev, &WirelessDevice::hostspotConnectionsChanged, this, &HotspotPage::refreshHotspotConnectionList);

    connect(m_hotspotSwitch, &SwitchWidget::checkedChanged, this, &HotspotPage::onSwitchToggled);
}

void HotspotPage::setModel(NetworkModel *model)
{
    m_model = model;

    QTimer::singleShot(0, this, &HotspotPage::onHotsportEnabledChanged);
    QTimer::singleShot(0, this, &HotspotPage::refreshHotspotConnectionList);
}

void HotspotPage::closeHotspot()
{
    const QString uuid = m_wdev->activeHotspotUuid();
    Q_ASSERT(!uuid.isEmpty());

    Q_EMIT requestDisconnectConnection(uuid);
    Q_EMIT requestDeviceRemanage(m_wdev->path());
}

void HotspotPage::openHotspot()
{
    const QList<QJsonObject> &connsObj = m_wdev->hotspotConnections();

    if (connsObj.isEmpty()) {
        m_hotspotSwitch->setChecked(false);
        openEditPage(QString());
    } else {
        // use the first connection of the hotspot connection list
        requestActivateConnection(m_wdev->path(), connsObj.first().value("Uuid").toString());
    }
}

void HotspotPage::openEditPage(const QString &uuid)
{
    m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
    m_editPage->initSettingsWidget();

    connect(m_editPage, &ConnectionHotspotEditPage::requestNextPage, this, &HotspotPage::requestNextPage);

    Q_EMIT requestNextPage(m_editPage);
}

QString HotspotPage::uuidByConnWidget(QObject *connWidget)
{
    QString uuid;

    NextPageWidget *cw = dynamic_cast<NextPageWidget *>(connWidget);
    if (cw) {
        uuid = m_connWidgetUuidMap.value(cw);
        if (uuid.isEmpty()) {
            qDebug() << "Warning: can not find UUID of the clicked connection";
        }
    } else {
        qDebug() << "Warning: can not find the clicked connection wdiget";
    }

    return uuid;
}

void HotspotPage::onDeviceRemoved()
{
    // back if ap edit page exist
    if (!m_editPage.isNull()) {
        m_editPage->onDeviceRemoved();
    }

    // destroy self page
    Q_EMIT back();
}

void HotspotPage::onSwitchToggled(const bool checked)
{
    if (checked)
        openHotspot();
    else
        closeHotspot();
}

void HotspotPage::onConnWidgetSelected()
{
    const QString uuid = uuidByConnWidget(sender());
    if (uuid.isEmpty()) {
        return;
    }

    requestActivateConnection(m_wdev->path(), uuid);
}

void HotspotPage::onConnWidgetNextPage()
{
    const QString uuid = uuidByConnWidget(sender());
    if (uuid.isEmpty()) {
        return;
    }

    m_editPage = new ConnectionHotspotEditPage(m_wdev->path(), uuid);
    m_editPage->initSettingsWidget();

    connect(m_editPage, &ConnectionHotspotEditPage::requestNextPage, this, &HotspotPage::requestNextPage);

    Q_EMIT requestNextPage(m_editPage);
}

void HotspotPage::onHotsportEnabledChanged()
{
    m_hotspotSwitch->setChecked(m_wdev->hotspotEnabled());

    m_refreshActiveTimer->start();
}

void HotspotPage::refreshHotspotConnectionList()
{
    if (!m_wdev || !m_wdev->enabled()) {
        return;
    }

    const QList<widgets::NextPageWidget *> &connWidgetList = m_connWidgetUuidMap.keys();
    m_connWidgetUuidMap.clear();
    m_connectionsGroup->clear();
    qDeleteAll(connWidgetList);


    for (auto connObj : m_wdev->hotspotConnections()) {
        const QString &ssid = connObj.value("Ssid").toString();
        const QString &uuid = connObj.value("Uuid").toString();

        NextPageWidget *conn = new NextPageWidget;
        conn->setTitle(ssid);

        connect(conn, &NextPageWidget::selected, this, &HotspotPage::onConnWidgetSelected);
        connect(conn, &NextPageWidget::acceptNextPage, this, &HotspotPage::onConnWidgetNextPage);

        m_connectionsGroup->appendItem(conn);
        m_connWidgetUuidMap.insert(conn, uuid);
    }

    m_refreshActiveTimer->start();
}

void HotspotPage::refreshActiveConnection()
{
    QString activeHotspotUuid;

    if (m_wdev->hotspotEnabled()) {
        activeHotspotUuid = m_wdev->activeHotspotUuid();
        if (activeHotspotUuid.isEmpty()) {
            qDebug() << "Warning: uuid of active hotspot connection is empty";
            return;
        }
    }

    for (auto it = m_connWidgetUuidMap.begin(); it != m_connWidgetUuidMap.end(); ++it) {
        const QString &uuid = it.value();
        NextPageWidget *widget = it.key();

        if (uuid == activeHotspotUuid) {
            widget->setIcon(QPixmap(":/network/themes/dark/icons/select.svg"));
        } else {
            widget->setIcon(QPixmap());
        }
    }
}

}

}
