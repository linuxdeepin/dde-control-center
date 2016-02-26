/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "dbusdccnetworkservice.h"
#include "networkmainwidget.h"

DBusDCCNetworkService::DBusDCCNetworkService(NetworkMainWidget *parent) :
    QDBusAbstractAdaptor(parent)
{
    m_mainWidget = parent;

    connect(parent->stackWidget(), &DStackWidget::depthChanged, this, &DBusDCCNetworkService::activeChanged);
    connect(parent, &NetworkMainWidget::dccVisibleChanged, this, &DBusDCCNetworkService::activeChanged);
}

bool DBusDCCNetworkService::active() const
{
    return m_mainWidget->stackWidget()->depth() == 1 && m_mainWidget->dccIsVisible();
}
