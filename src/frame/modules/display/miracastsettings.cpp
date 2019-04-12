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

#include "miracastsettings.h"
#include "widgets/translucentframe.h"
#include "widgets/labels/normallabel.h"
#include "types/linkinfolist.h"
#include "widgets/settingsheaderitem.h"

#include <QTimer>

using namespace dcc;
using namespace dcc::display;

MiracastPage::MiracastPage(const QString &title, QWidget *parent) : ContentWidget(parent)
{
    setTitle(title);

    m_refreshTimer = new QTimer(this);
    m_refreshTimer->setInterval(1 * 60 * 1000);
    m_refreshTimer->setSingleShot(true);

    m_deviceSwBtn = new SwitchWidget;
    m_deviceSwBtn->setTitle(title);

    SettingsGroup *switchGrp = new SettingsGroup;
    switchGrp->appendItem(m_deviceSwBtn);

    m_tip = new NormalLabel(tr("WIFI will be disconnected when screen projection is enabled"));
    m_tip->setWordWrap(true);

    m_refreshBtn = new DImageButton;
    m_refreshBtn->setText(tr("Refresh"));
    m_refreshBtn->setObjectName("RefreshBtn");

    QHBoxLayout *rightLayout = new QHBoxLayout;
    rightLayout->setMargin(0);
    rightLayout->setSpacing(0);
    rightLayout->addWidget(m_refreshBtn);
    rightLayout->addSpacing(10);

    QWidget *rightWidget = new QWidget;
    rightWidget->setLayout(rightLayout);

    m_deviceGrp = new SettingsGroup;
    m_deviceGrp->setHeaderVisible(true);
    m_deviceGrp->headerItem()->setTitle(tr("Device List"));
    m_deviceGrp->headerItem()->setRightWidget(rightWidget);
    m_nodevice = new MiracastNoDeviceWidget;
    m_nodevice->setVisible(false);

    m_mainLayout = new QVBoxLayout;
    m_mainLayout->setMargin(0);
    m_mainLayout->setSpacing(10);

    m_mainLayout->addSpacing(10);
    m_mainLayout->addWidget(switchGrp);
    m_mainLayout->addWidget(m_tip);
    m_mainLayout->addWidget(m_deviceGrp);
    m_mainLayout->addWidget(m_nodevice);
    m_mainLayout->addStretch();

    TranslucentFrame *widget = new TranslucentFrame;
    widget->setLayout(m_mainLayout);

    setContent(widget);

    connect(m_deviceSwBtn, &SwitchWidget::checkedChanged, this, &MiracastPage::onDeviceEnableChanged);
    connect(m_deviceSwBtn, &SwitchWidget::checkedChanged, this, &MiracastPage::onDeviceStateChanged);
    connect(m_nodevice, &MiracastNoDeviceWidget::requestRefreshed, this, &MiracastPage::onRefreshed);
    connect(m_refreshBtn, &DImageButton::clicked, this, &MiracastPage::onRefreshed);
    connect(m_refreshTimer, &QTimer::timeout, this, &MiracastPage::onRefreshTimeout);
}

void MiracastPage::setModel(MiracastDeviceModel *model)
{
    m_model = model;

    connect(model, &MiracastDeviceModel::addSink, this, &MiracastPage::onItemAdded);
    connect(model, &MiracastDeviceModel::removeSink, this, &MiracastPage::onItemRemoved);
    connect(model, &MiracastDeviceModel::linkManageChanged, m_deviceSwBtn, &SwitchWidget::setChecked);
    connect(model, &MiracastDeviceModel::linkManageChanged, this, &MiracastPage::onDeviceManaged);
    connect(model, &MiracastDeviceModel::destroyed, this, &MiracastPage::back);
    connect(model, &MiracastDeviceModel::connectStateChanged, this, &MiracastPage::onSinkConnectChanged);

    m_refreshBtn->setVisible(!model->isConnected());

    m_deviceSwBtn->setChecked(model->linkInfo().m_managed);
    m_tip->setVisible(!m_deviceSwBtn->checked());

    m_nodevice->setVisible(m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());
    m_deviceGrp->setVisible(!m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());

    for (const SinkInfo &info : model->sinkList()) {
        onItemAdded(info);
    }
}

void MiracastPage::onItemAdded(const SinkInfo &info)
{
    if (m_deviceItemList.keys().contains(info.m_sinkPath.path()))
        return;

    MiracastItem *item = new MiracastItem(info);
    m_deviceItemList.insert(info.m_sinkPath.path(), item);
    m_deviceGrp->appendItem(item);
    m_deviceGrp->setVisible(true);
    m_nodevice->setVisible(false);
    m_refreshTimer->stop();

    connect(item, &MiracastItem::requestSinkConnect, this, &MiracastPage::requestDeviceConnect);
    connect(item, &MiracastItem::requestSinkDisConnect, this, &MiracastPage::requestDeviceDisConnect);
}

void MiracastPage::onItemRemoved(const SinkInfo &info)
{
    MiracastItem *item = itemByPath(info.m_sinkPath.path());

    if (item) {
        m_deviceItemList.remove(info.m_sinkPath.path());
        m_deviceGrp->removeItem(item);
        item->deleteLater();

        if (m_model->sinkList().isEmpty()) {
            m_deviceGrp->setVisible(false);
            m_nodevice->setVisible(m_deviceSwBtn->checked());
            m_nodevice->setState(MiracastNoDeviceWidget::NoDevice);
        }
    }
}

void MiracastPage::onDeviceEnableChanged(const bool enable)
{
    Q_EMIT requestDeviceEnable(m_model->linkInfo().m_dbusPath, enable);
}

void MiracastPage::onRefreshed()
{
    Q_EMIT requestDeviceRefreshed(m_model->linkInfo().m_dbusPath, true);
    m_deviceGrp->setVisible(false);
    m_nodevice->setVisible(m_deviceSwBtn->checked());
    m_nodevice->setState(MiracastNoDeviceWidget::Refreshed);
    m_refreshTimer->start();
}

void MiracastPage::onDeviceStateChanged(const bool state)
{
    m_tip->setVisible(!state);
    m_nodevice->setVisible(state);
    m_deviceGrp->setVisible(!m_model->sinkList().isEmpty() && state);
    if (state) {
        m_nodevice->setState(MiracastNoDeviceWidget::Refreshed);
        m_refreshTimer->start();
    } else {
        m_refreshTimer->stop();
    }
}

void MiracastPage::onDeviceManaged(const bool managed)
{
    if (managed)
        return;

    m_tip->setVisible(!managed);
    m_deviceGrp->setVisible(false);
    m_nodevice->setVisible(false);
}

void MiracastPage::onRefreshTimeout()
{
    Q_EMIT requestDeviceRefreshed(m_model->linkInfo().m_dbusPath, false);
    m_deviceGrp->setVisible(!m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());
    m_nodevice->setVisible(m_model->sinkList().isEmpty() && m_deviceSwBtn->checked());
    m_nodevice->setState(MiracastNoDeviceWidget::NoDevice);
}

void MiracastPage::onSinkConnectChanged(const QDBusObjectPath &sinkPath, const bool state)
{
    m_refreshBtn->setVisible(!state);

    MiracastItem *item = itemByPath(sinkPath.path());
    if (item)
        item->onConnectState(state);
}

MiracastItem *MiracastPage::itemByPath(const QString &path)
{
    for (auto it(m_deviceItemList.begin()); it != m_deviceItemList.end(); ++it) {
        if (it.key() == path)
            return it.value();
    }

    return nullptr;
}
