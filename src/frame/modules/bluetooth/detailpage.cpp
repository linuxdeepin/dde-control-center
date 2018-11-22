/*
 * Copyright (C) 2016 ~ 2018 Deepin Technology Co., Ltd.
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

#include "detailpage.h"

#include <QVBoxLayout>
#include <QPushButton>

#include "device.h"
#include "adapter.h"

#include "widgets/translucentframe.h"

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

    connect(m_ignoreButton, &QPushButton::clicked, [this] { Q_EMIT requestIgnoreDevice(m_adapter, m_device); Q_EMIT back(); });
    connect(m_disconnectButton, &QPushButton::clicked, [this] { Q_EMIT requestDisconnectDevice(m_device); Q_EMIT back(); });

    connect(adapter, &Adapter::destroyed, this, &DetailPage::back);
}

} // namespace bluetooth
} // namespace dcc
