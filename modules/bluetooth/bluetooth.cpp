/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include <QLabel>
#include <QDebug>

#include "bluetoothmainwidget.h"
#include "bluetooth.h"
#include "dbus/dbusbluetooth.h"

Bluetooth::Bluetooth()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Bluetooth"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_mainWidget = new BluetoothMainWidget;
}

Bluetooth::~Bluetooth()
{
    qDebug() << "~Bluetooth()";

    m_mainWidget->hide();
    m_mainWidget->setParent(nullptr);
    delete m_mainWidget;
}

QFrame* Bluetooth::getContent()
{
    return m_mainWidget;
}

void Bluetooth::setProxy(ControlCenterProxyInterface *proxy)
{
    ModuleInterface::setProxy(proxy);

    DBusBluetooth *inter = new DBusBluetooth(this);
    connect(m_controlCenterProxy->dccObject(), SIGNAL(visibleChanged(bool)), inter, SLOT(ClearUnpairedDevice()));
}
