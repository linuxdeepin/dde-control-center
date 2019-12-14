/*
 * Copyright (C) 2019 Deepin Technology Co., Ltd.
 *
 * Author:     andywang <andywang_cm@deepin.com>
 *
 * Maintainer: andywang <andywang_cm@deepin.com>
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
#include "modules/bluetooth/device.h"
#include "modules/bluetooth/adapter.h"
#include "widgets/translucentframe.h"
#include "widgets/titlelabel.h"

#include <QVBoxLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QString>

using namespace dcc;
using namespace dcc::bluetooth;
using namespace DCC_NAMESPACE;
using namespace DCC_NAMESPACE::bluetooth;

DetailPage::DetailPage(const Adapter *adapter, const Device *device)
    : ContentWidget()
    , m_adapter(adapter)
    , m_device(device)
{
    m_ignoreButton = new QPushButton(tr("Ignore this device"));
    m_disconnectButton = new QPushButton(tr("Disconnect"));
    setTitle(device->name());
    dcc::widgets::TranslucentFrame *frame = new dcc::widgets::TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setSpacing(0);
    layout->setMargin(0);
    DIconButton *backWidgetBtn;
    backWidgetBtn = new DIconButton(this);
    backWidgetBtn->setFlat(true);
    backWidgetBtn->setFixedSize(30, 48);
    backWidgetBtn->setIcon(QStyle::SP_ArrowBack);
    layout->addWidget(backWidgetBtn, Qt::AlignLeft);
    m_devNameLabel = new TitleLabel(tr("Change Name"));
    layout->addWidget(m_devNameLabel, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    m_editDevName = new QLineEdit;
    m_editDevName->setPlaceholderText(device->name());
    layout->addWidget(m_editDevName);
    layout->addSpacing(10);
    layout->addWidget(m_disconnectButton);
    layout->addSpacing(10);
    layout->addWidget(m_ignoreButton);
    layout->addStretch();
    setContent(frame);
    onDeviceStatusChanged();
    connect(m_ignoreButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestIgnoreDevice(m_adapter, m_device);
    });
    connect(m_disconnectButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestDisconnectDevice(m_device);
    });
    connect(m_device, &Device::nameChanged, m_editDevName, &QLineEdit::setText);
    connect(m_device, &Device::stateChanged, this, [this] {
        onDeviceStatusChanged();
    });
    connect(m_device, &Device::pairedChanged, this, [this](const bool paired) {
        if (!paired) {
            Q_EMIT back();
        }
    });
    connect(adapter, &Adapter::poweredChanged, this, [this](const bool &powered) {
        if (!powered) {
            Q_EMIT back();
        }
    });
    connect(m_editDevName, &QLineEdit::editingFinished, this, &DetailPage::onDeviceNameChanged);
    connect(adapter, &Adapter::destroyed, this, &DetailPage::back);
    connect(backWidgetBtn, &DIconButton::clicked, this, &DetailPage::back);
}

void DetailPage::onDeviceStatusChanged()
{
    m_device->state() == Device::StateConnected ? m_disconnectButton->show() : m_disconnectButton->hide();
    m_device->state() == Device::StateConnected ? m_devNameLabel->show() : m_devNameLabel->hide();
    m_device->state() == Device::StateConnected ? m_editDevName->show() : m_editDevName->hide();
}

void DetailPage::onDeviceNameChanged()
{
    QString devAlias = m_editDevName->text();
    if (devAlias.isEmpty()) {
        m_editDevName->setPlaceholderText(m_device->name());
        return;
    }
    Q_EMIT requestSetDevAlias(m_device, devAlias);
}
