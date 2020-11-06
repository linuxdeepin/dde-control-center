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
#include <QApplication>
#include <DDesktopServices>

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
    m_connectButton = new QPushButton(tr("Connect"));
    setTitle(device->name());
    dcc::widgets::TranslucentFrame *frame = new dcc::widgets::TranslucentFrame;
    QVBoxLayout *layout = new QVBoxLayout(frame);
    layout->setSpacing(0);
    layout->setMargin(0);
    DIconButton *backWidgetBtn = new DIconButton(this);
    backWidgetBtn->setAccessibleName("back_btn");
    backWidgetBtn->setFlat(true);
    backWidgetBtn->setFixedSize(30, 48);
    backWidgetBtn->setIcon(QStyle::SP_ArrowBack);
    layout->addWidget(backWidgetBtn, Qt::AlignLeft);
    m_devNameLabel = new TitleLabel(device->name());
    layout->addWidget(m_devNameLabel, 0, Qt::AlignCenter);
    layout->addSpacing(10);
    m_editDevAlias = new QLineEdit;
    m_editDevAlias->setPlaceholderText(device->alias().isEmpty() ? device->name() : device->alias());
    layout->addWidget(m_editDevAlias);
    layout->addSpacing(10);
    layout->addWidget(m_disconnectButton);
    layout->addWidget(m_connectButton);
    layout->addSpacing(10);
    layout->addWidget(m_ignoreButton);
    layout->addStretch();
    setContent(frame);
    onDeviceStatusChanged();
    connect(m_ignoreButton, &QPushButton::clicked, this, [this] {
        if(m_device->state() == Device::StateConnected)
            Q_EMIT requestDisconnectDevice(m_device);
        Q_EMIT requestIgnoreDevice(m_adapter, m_device);
        QApplication::focusWidget()->clearFocus();
    });
    connect(m_disconnectButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestDisconnectDevice(m_device);
        QApplication::focusWidget()->clearFocus();
    });
    connect(m_connectButton, &QPushButton::clicked, this, [this] {
        Q_EMIT requestConnectDevice(m_device, m_adapter);
        QApplication::focusWidget()->clearFocus();
    });
    connect(m_editDevAlias, &QLineEdit::textEdited, this, [ = ](const QString &str){
        if (str.length() > 32) {
            m_editDevAlias->backspace();
            DDesktopServices::playSystemSoundEffect(DDesktopServices::SSE_Error);
        }
    });
    connect(m_device, &Device::nameChanged, m_devNameLabel, &QLabel::setText);
    connect(m_device, &Device::aliasChanged, m_editDevAlias, &QLineEdit::setText);
    connect(m_device, &Device::stateChanged, this, [this] {
        onDeviceStatusChanged();
    });
    connect(m_device, &Device::pairedChanged, this, [this](const bool paired) {
        if (!paired) {
            Q_EMIT back();
        }
    });
    connect(adapter, &Adapter::poweredChanged, this, [this](const bool &powered, const bool &discovering) {
        Q_UNUSED(discovering)
        if (!powered) {
            Q_EMIT back();
        }
    });
    connect(m_editDevAlias, &QLineEdit::editingFinished, this, &DetailPage::onDeviceAliasChanged);
    connect(adapter, &Adapter::destroyed, this, &DetailPage::back);
    connect(backWidgetBtn, &DIconButton::clicked, this, &DetailPage::back);
}

void DetailPage::onDeviceStatusChanged()
{
    if (m_device->state() == Device::StateConnected && m_device->connectState()) {
        m_disconnectButton->show();
        m_connectButton->hide();
        m_ignoreButton->setEnabled(true);
    } else if (m_device->state() == Device::StateAvailable) {
        m_connectButton->show();
        m_connectButton->setText(tr("Connecting"));
        m_connectButton->setDisabled(true);
        m_disconnectButton->hide();
        m_ignoreButton->setEnabled(false);
    } else if (m_device->state() == Device::StateDisconnecting) {
        m_connectButton->show();
        m_connectButton->setText(tr("Disconnecting"));
        m_connectButton->setDisabled(true);
        m_disconnectButton->hide();
        m_ignoreButton->setEnabled(false);
    } else {
        m_connectButton->show();
        m_connectButton->setText(tr("Connect"));
        m_connectButton->setEnabled(true);
        m_disconnectButton->hide();
        m_ignoreButton->setEnabled(true);
    }
}

void DetailPage::onDeviceAliasChanged()
{
    QString devAlias = m_editDevAlias->text();
    QString devName(m_device->name());
    if (devAlias.isEmpty()) {
        m_editDevAlias->setPlaceholderText(m_device->name());
        Q_EMIT requestSetDevAlias(m_device, devName);
    } else if (devAlias != m_device->alias()) {
        Q_EMIT requestSetDevAlias(m_device, devAlias);
    }
}
