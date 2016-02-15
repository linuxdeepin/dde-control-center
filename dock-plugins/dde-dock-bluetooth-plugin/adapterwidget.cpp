/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <dthememanager.h>
#include <libdui_global.h>
#include <dlineedit.h>
#include <dtextbutton.h>
#include <dheaderline.h>

#include "adapterwidget.h"
#include "deviceitemwidget.h"
#include "constants.h"

DWIDGET_USE_NAMESPACE

AdapterWidget::AdapterWidget(BluetoothObject::AdapterInfo *info,
                             QWidget *parent) :
    QWidget(parent),
    m_info(info)
{
    initUI();
}

AdapterWidget::~AdapterWidget()
{
    if(m_info)
        delete m_info;
}

void AdapterWidget::addDevice(BluetoothObject::DeviceInfo *info)
{
    info->adapterInfo = m_info;

    m_deviceItemList->addWidget(info->item);
}

void AdapterWidget::removeDevice(BluetoothObject::DeviceInfo *info, bool isDelete)
{
    int index = m_deviceItemList->indexOf(info->item);

    if(index >= 0){
        info->adapterInfo = nullptr;
        m_deviceItemList->removeWidget(index, isDelete);
    }
}

int AdapterWidget::getHeightHint() const
{
    if(m_headerLine && m_deviceItemList) {
        if(m_info && m_info->powered)
            return m_headerLine->height() + m_deviceItemList->count() * DTK_WIDGET_NAMESPACE::EXPAND_HEADER_HEIGHT + 2;
        else
            return m_headerLine->height();
    }

    return height();
}

QSize AdapterWidget::sizeHint() const
{
    return QSize(width(), getHeightHint());
}

void AdapterWidget::updateUI()
{
    m_headerLine->setTitle(m_info->name);
    m_bluetoothSwitch->setChecked(m_info->powered);
    m_deviceItemList->setVisible(m_deviceItemList->count() > 0 && m_info->powered);
    m_listWidgetSeparator->setVisible(m_deviceItemList->isVisible());
}

void AdapterWidget::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);

    emit sizeChanged();
}

void AdapterWidget::initUI()
{
    setStyleSheet("background: transparent");
    setFixedWidth(BLUETOOTH_PLUGIN_WIDTH);

    m_headerLine = new DHeaderLine;
    m_bluetoothSwitch = new DSwitchButton;

    m_bluetoothSwitch->setChecked(m_info->powered);
    m_headerLine->setFixedWidth(BLUETOOTH_PLUGIN_WIDTH);
    m_headerLine->setTitle(m_info->name);
    m_headerLine->setContent(m_bluetoothSwitch);

    connect(m_bluetoothSwitch, &DSwitchButton::checkedChanged, this, [this](bool checked){
        if(m_info->powered != checked) {
            m_info->bluetoothDbus->SetAdapterPowered(QDBusObjectPath(m_info->path), checked);
            m_info->powered = checked;

            setFixedHeight(getHeightHint());
        }
    });

    m_deviceItemList = new DListWidget;
    m_listWidgetSeparator = new DSeparatorHorizontal;

    m_deviceItemList->setVisible(m_deviceItemList->count() > 0 && m_info->powered);
    m_deviceItemList->setMaximumHeight(DEVICE_LIST_MAX_HEIGHT);
    m_deviceItemList->setEnableVerticalScroll(true);
    m_listWidgetSeparator->hide();
    m_info->widget = this;

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);
    mainLayout->addWidget(m_headerLine);
    mainLayout->addWidget(new DSeparatorHorizontal);
    mainLayout->addWidget(m_deviceItemList);
    mainLayout->addWidget(m_listWidgetSeparator);
    setFixedHeight(getHeightHint());

    connect(m_deviceItemList, &DListWidget::countChanged, this, [this] {
        m_deviceItemList->setVisible(m_deviceItemList->count() > 0 && m_info->powered);
        m_listWidgetSeparator->setVisible(m_deviceItemList->isVisible());

        setFixedHeight(getHeightHint());
    });
}
