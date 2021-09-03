/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     listenerri <listenerri@gmail.com>
 *
 * Maintainer: listenerri <listenerri@gmail.com>
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

#include "vpnipsecsection.h"

#include <widgets/contentwidget.h>
#include <widgets/switchwidget.h>
#include <widgets/lineeditwidget.h>

using namespace dcc::widgets;
using namespace NetworkManager;

VpnIpsecSection::VpnIpsecSection(VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN IPsec"), parent)
    , m_vpnSetting(vpnSetting)
    , m_ipsecEnable(new SwitchWidget(this))
    , m_groupName(new LineEditWidget(this))
    , m_gatewayId(new LineEditWidget(this))
    , m_psk(new LineEditWidget(this))
    , m_ike(new LineEditWidget(this))
    , m_esp(new LineEditWidget(this))
{
    initUI();
    initConnection();

    onIpsecCheckedChanged(m_ipsecEnable->checked());
}

VpnIpsecSection::~VpnIpsecSection()
{
}

bool VpnIpsecSection::allInputValid()
{
    return true;
}

void VpnIpsecSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    if (m_ipsecEnable->checked()) {
        m_dataMap.insert("ipsec-enabled", "yes");
        m_dataMap.insert("ipsec-group-name", m_groupName->text());
        m_dataMap.insert("ipsec-gateway-id", m_gatewayId->text());
        m_dataMap.insert("ipsec-psk", m_psk->text());
        m_dataMap.insert("ipsec-ike", m_ike->text());
        m_dataMap.insert("ipsec-esp", m_esp->text());
    } else {
        m_dataMap.remove("ipsec-enabled");
        m_dataMap.remove("ipsec-group-name");
        m_dataMap.remove("ipsec-gateway-id");
        m_dataMap.remove("ipsec-psk");
        m_dataMap.remove("ipsec-ike");
        m_dataMap.remove("ipsec-esp");
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setInitialized(true);
}

void VpnIpsecSection::initUI()
{
    setAccessibleName("VpnIpsecSection");
    m_ipsecEnable->setTitle(tr("Enable IPsec"));
    m_ipsecEnable->setChecked(m_dataMap.value("ipsec-enabled") == "yes");

    m_groupName->setTitle(tr("Group Name"));
    m_groupName->setText(m_dataMap.value("ipsec-group-name"));

    m_gatewayId->setTitle(tr("Group ID"));
    m_gatewayId->setText(m_dataMap.value("ipsec-gateway-id"));

    m_psk->setTitle(tr("Pre-Shared Key"));
    m_psk->setText(m_dataMap.value("ipsec-psk"));

    m_ike->setTitle(tr("Phase1 Algorithms"));
    m_ike->setText(m_dataMap.value("ipsec-ike"));

    m_esp->setTitle(tr("Phase2 Algorithms"));
    m_esp->setText(m_dataMap.value("ipsec-esp"));

    appendItem(m_ipsecEnable);
    appendItem(m_groupName);
    appendItem(m_gatewayId);
    appendItem(m_psk);
    appendItem(m_ike);
    appendItem(m_esp);

    m_groupName->textEdit()->installEventFilter(this);
    m_gatewayId->textEdit()->installEventFilter(this);
    m_psk->textEdit()->installEventFilter(this);
    m_ike->textEdit()->installEventFilter(this);
    m_esp->textEdit()->installEventFilter(this);
}

void VpnIpsecSection::initConnection()
{
    connect(m_ipsecEnable, &SwitchWidget::checkedChanged, this, &VpnIpsecSection::onIpsecCheckedChanged);
    connect(m_ipsecEnable, &SwitchWidget::checkedChanged, this, &VpnIpsecSection::editClicked);
}

void VpnIpsecSection::onIpsecCheckedChanged(const bool enabled)
{
    m_groupName->setVisible(enabled);
    m_gatewayId->setVisible(enabled);
    m_psk->setVisible(enabled);
    m_ike->setVisible(enabled);
    m_esp->setVisible(enabled);
}

bool VpnIpsecSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched))
            Q_EMIT editClicked();
    }

    return QWidget::eventFilter(watched, event);
}
