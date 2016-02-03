/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "imagenamebutton.h"
#include "deviceitemwidget.h"
#include "constants.h"
#include "adapterwidget.h"

#include <QHBoxLayout>
#include <QDBusObjectPath>

#include <libdui/dconstants.h>
#include <libdui/dthememanager.h>

DeviceItemWidget::DeviceItemWidget(BluetoothMainWidget::DeviceInfo *info, QWidget *parent)
    : QWidget(parent),
      m_info(info)
{
    m_deviceName = new QLabel;
    m_deviceName->setStyleSheet("color:#b4b4b4;");
    m_statTips = new QLabel;
    m_statTips->setStyleSheet("color:#666;");
    m_removeBtn = new DImageButton;
    m_removeBtn->setNormalPic(":/dark/images/disconnect_normal.png");
    m_removeBtn->setHoverPic(":/dark/images/disconnect_hover.png");
    m_removeBtn->setPressPic(":/dark/images/disconnect_press.png");
    m_removeBtn->hide();
    m_loadingIndicator = new DLoadingIndicator;
    m_loadingIndicator->setFixedSize(18, 18);
    m_loadingIndicator->setImageSource(QPixmap(":/dark/images/waiting.png"));
    m_loadingIndicator->setLoading(true);
    m_loadingIndicator->hide();
//    m_loadingIndicator->show();

    m_confirmWidget = new ConfrimWidget;

    QHBoxLayout *iconsLayout = new QHBoxLayout;
    iconsLayout->addWidget(m_removeBtn);
    iconsLayout->setAlignment(m_removeBtn, Qt::AlignVCenter);
    iconsLayout->addWidget(m_loadingIndicator);
    iconsLayout->setAlignment(m_loadingIndicator, Qt::AlignVCenter);
    iconsLayout->setContentsMargins(0, 5, 0, 0);
    iconsLayout->setSpacing(0);

    QWidget *iconsWidget = new QWidget;
    iconsWidget->setLayout(iconsLayout);
    iconsWidget->setFixedWidth(18);

    QHBoxLayout *mainLayout = new QHBoxLayout;
    mainLayout->addWidget(m_deviceName);
    mainLayout->addStretch(0);
    mainLayout->addWidget(m_statTips);
    mainLayout->addWidget(iconsWidget);
    mainLayout->setAlignment(iconsWidget, Qt::AlignVCenter);
    mainLayout->setSpacing(10);
    mainLayout->setContentsMargins(10, 2, 10, 2);

    setLayout(mainLayout);
    updateUI();

    connect(m_removeBtn, &DImageButton::clicked, this, &DeviceItemWidget::showConfirm);
    connect(m_confirmWidget, &ConfrimWidget::ignore, [this] {
        m_info->adapterInfo->bluetoothDbus->RemoveDevice(QDBusObjectPath(m_info->adapterInfo->path), QDBusObjectPath(m_info->path));
        m_info->adapterInfo->widget->removeConfirm(m_confirmWidget);
        m_confirmVisible = false;
    });
    connect(m_confirmWidget, &ConfrimWidget::disconnect, [this] {
        m_info->adapterInfo->bluetoothDbus->DisconnectDevice(QDBusObjectPath(m_info->path));
        m_info->adapterInfo->widget->removeConfirm(m_confirmWidget);
        m_confirmVisible = false;
    });
}

DeviceItemWidget::~DeviceItemWidget()
{
    if(m_info)
        delete m_info;
    m_confirmWidget->deleteLater();
}

void DeviceItemWidget::updateUI()
{
    m_deviceName->setText(m_info->name);

    m_statTips->setVisible(m_info->paired);
    if (m_info->state != BluetoothMainWidget::DeviceInfo::Connecting)
        m_loadingIndicator->hide();

    switch (m_info->state)
    {
    case BluetoothMainWidget::DeviceInfo::Connected:        m_statTips->setText(tr("Connected"));           break;
    case BluetoothMainWidget::DeviceInfo::Connecting:       m_statTips->clear();                            break;
    case BluetoothMainWidget::DeviceInfo::Disconnected:     m_statTips->setText(tr("Unconnect"));           break;
    }

    m_confirmWidget->setDisconnectVisible(m_info->state != BluetoothMainWidget::DeviceInfo::Disconnected);

    if (rect().contains(mapFromGlobal(QCursor::pos())))
        enterEvent(nullptr);
}

void DeviceItemWidget::enterEvent(QEvent *)
{
    if (m_info->paired && m_info->state != BluetoothMainWidget::DeviceInfo::Connecting)
        m_removeBtn->show();
}

void DeviceItemWidget::leaveEvent(QEvent *)
{
    m_removeBtn->hide();
}

void DeviceItemWidget::mouseReleaseEvent(QMouseEvent *)
{
    if (m_info->state == BluetoothMainWidget::DeviceInfo::Disconnected) {
        m_loadingIndicator->show();
        m_removeBtn->hide();
        m_info->adapterInfo->bluetoothDbus->ConnectDevice(QDBusObjectPath(m_info->path));
    }

    if (m_info->state == BluetoothMainWidget::DeviceInfo::Connected) {
        m_loadingIndicator->show();
        m_removeBtn->hide();

        QTimer::singleShot(500, this, SLOT(updateUI()));
    }

    if (m_confirmVisible)
    {
        m_confirmVisible = false;
        m_info->adapterInfo->widget->removeConfirm(m_confirmWidget);
    }
}

void DeviceItemWidget::showConfirm()
{
    m_confirmVisible = true;
    m_confirmWidget->setDisconnectVisible(m_info->state != BluetoothMainWidget::DeviceInfo::Disconnected);
    m_info->adapterInfo->widget->addConfirm(m_confirmWidget, m_info);
}
