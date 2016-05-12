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

#include "network.h"
#include "networkmainwidget.h"
#include "controlcenterproxyinterface.h"

QFrame *NetworkModule::getContent() {
    qDebug() << "new Network begin";
    static Network *frame = new Network;
    qDebug() << "new Network end";
    return frame->getContent();
}

Network::Network()
{
    Q_UNUSED(QT_TRANSLATE_NOOP("ModuleName", "Network"));

    Q_INIT_RESOURCE(widgets_theme_dark);
    Q_INIT_RESOURCE(widgets_theme_light);

    m_mainWidget = new NetworkMainWidget(this);
}

Network::~Network()
{
    qDebug() << "~Network()";

    m_mainWidget->hide();
    m_mainWidget->setParent(nullptr);
    delete m_mainWidget;
}

QFrame* Network::getContent()
{
    return m_mainWidget;
}

ControlCenterProxyInterface *Network::getInterface() const
{
    return m_controlCenterProxy;
}

void Network::setProxy(ControlCenterProxyInterface *proxy)
{
    ModuleInterface::setProxy(proxy);

    connect(proxy->dccObject(), SIGNAL(visibleChanged(bool)), this, SIGNAL(dccVisibleChanged(bool)));
}
