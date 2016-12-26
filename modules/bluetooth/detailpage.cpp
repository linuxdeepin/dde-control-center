/**
 * Copyright (C) 2016 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "detailpage.h"

#include <QVBoxLayout>
#include <QPushButton>

#include "device.h"

#include "translucentframe.h"

namespace dcc {
namespace bluetooth {

DetailPage::DetailPage(const Adapter *adapter, const Device *device) :
    ContentWidget(),
    m_adapter(adapter),
    m_device(device),
    m_ignoreButton(new QPushButton(tr("Ignore"))),
    m_disconnectButton(new QPushButton(tr("Disconnect")))
{
    setTitle(device->name());

    dcc::widgets::TranslucentFrame *frame = new dcc::widgets::TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setSpacing(0);
    layout->setMargin(0);
    layout->addSpacing(10);
    layout->addWidget(m_disconnectButton);
    layout->addSpacing(10);
    layout->addWidget(m_ignoreButton);

    setContent(frame);

    device->state() == Device::StateConnected ? m_disconnectButton->show() : m_disconnectButton->hide();

    connect(m_ignoreButton, &QPushButton::clicked, [this] { emit requestIgnoreDevice(m_adapter, m_device); emit back(); });
    connect(m_disconnectButton, &QPushButton::clicked, [this] { emit requestDisconnectDevice(m_device); emit back(); });
}

} // namespace bluetooth
} // namespace dcc
