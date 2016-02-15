/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "deviceitemwidget.h"
#include "constants.h"
#include "adapterwidget.h"

#include <QHBoxLayout>
#include <QDBusObjectPath>

#include <dconstants.h>
#include <dthememanager.h>

DeviceItemWidget::DeviceItemWidget(BluetoothObject::DeviceInfo *info, QWidget *parent)
    : QWidget(parent),
      m_info(info)
{
    m_deviceName = new QLabel;
    m_removeBtn = new DImageButton(this);
    m_removeBtn->setNormalPic(":/dark/images/connected.png");
    m_removeBtn->setFixedSize(16, 16);
    m_removeBtn->hide();
    m_loadingIndicator = new DLoadingIndicator(this);
    m_loadingIndicator->setFixedSize(16, 16);
    m_loadingIndicator->setImageSource(QPixmap(":/dark/images/waiting.png"));
    m_loadingIndicator->setLoading(true);
    m_loadingIndicator->hide();

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_deviceName);
    mainLayout->addStretch(0);
    mainLayout->setContentsMargins(15, 2, 10, 2);

    setLayout(mainLayout);
    updateUI();

    connect(m_removeBtn, &DImageButton::clicked, this, &DeviceItemWidget::disConnect);

    setMouseTracking(true);

    QFile file(":/dark/DeviceItemWidget.theme");

    if(file.open(QIODevice::ReadOnly)) {
        setStyleSheet(file.readAll());
        file.close();
    }
}

DeviceItemWidget::~DeviceItemWidget()
{
    if(m_info)
        delete m_info;
}

void DeviceItemWidget::updateUI()
{
    m_deviceName->setText(m_info->name);

    m_removeBtn->setVisible(m_info->state == BluetoothObject::DeviceInfo::Connected);
    m_loadingIndicator->setVisible(m_info->state == BluetoothObject::DeviceInfo::Connecting);
}

void DeviceItemWidget::enterEvent(QEvent *)
{
    if (m_info->state == BluetoothObject::DeviceInfo::Connected) {
        m_removeBtn->setNormalPic(":/dark/images/disconnect.png");
    }
}

void DeviceItemWidget::leaveEvent(QEvent *)
{
    if (m_info->state == BluetoothObject::DeviceInfo::Connected) {
        m_removeBtn->setNormalPic(":/dark/images/connected.png");
    }
}

void DeviceItemWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (m_info->state == BluetoothObject::DeviceInfo::Disconnected) {
        m_loadingIndicator->show();
        m_removeBtn->hide();
        m_info->adapterInfo->bluetoothDbus->ConnectDevice(QDBusObjectPath(m_info->path));
    }
}

void DeviceItemWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    m_removeBtn->move(0, height() / 2 - 8);
    m_loadingIndicator->move(0, height() / 2 - 8);
}

void DeviceItemWidget::disConnect()
{
    m_info->adapterInfo->bluetoothDbus->DisconnectDevice(QDBusObjectPath(m_info->path));
    m_removeBtn->hide();
}
