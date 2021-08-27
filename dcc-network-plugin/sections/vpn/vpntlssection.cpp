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

#include "vpntlssection.h"

#include <QComboBox>

#include "widgets/contentwidget.h"
#include "widgets/lineeditwidget.h"
#include "widgets/comboxwidget.h"
#include "widgets/filechoosewidget.h"
#include "widgets/switchwidget.h"

using namespace dcc::widgets;
using namespace NetworkManager;

VpnTLSSection::VpnTLSSection(VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN TLS Authentication"), parent)
    , m_vpnSetting(vpnSetting)
    , m_dataMap(vpnSetting->data())
    , m_remote(new LineEditWidget(this))
    , m_remoteCertTypeChooser(new ComboxWidget(this))
    , m_caCert(new FileChooseWidget(this))
    , m_customKeyDirection(new SwitchWidget(this))
    , m_keyDirectionChooser(new ComboxWidget(this))
{
    initStrMaps();
    initUI();
    initConnection();
}

VpnTLSSection::~VpnTLSSection()
{
}

bool VpnTLSSection::allInputValid()
{
    return true;
}

void VpnTLSSection::saveSettings()
{
    // 保存原来的设置
    m_dataMap = m_vpnSetting->data();

    m_dataMap.insert("tls-remote", m_remote->text());
    if (m_currentRemoteCertType == "default")
        m_dataMap.remove("remote-cert-tls");
    else
        m_dataMap.insert("remote-cert-tls", m_currentRemoteCertType);

    m_dataMap.insert("ta", m_caCert->edit()->text());
    if (m_customKeyDirection->checked())
        m_dataMap.insert("ta-dir", m_currentKeyDirection);
    else
        m_dataMap.remove("ta-dir");

    m_vpnSetting->setData(m_dataMap);

    m_vpnSetting->setInitialized(true);
}

void VpnTLSSection::initStrMaps()
{
    RemoteCertTypeStrMap = {
        { tr("Default"), "default" },
        { tr("Client"), "client" },
        { tr("Server"), "server" },
    };
}

void VpnTLSSection::initUI()
{
    m_remote->setTitle(tr("Subject Match"));
    m_remote->setText(m_dataMap.value("tls-remote"));

    m_remoteCertTypeChooser->setTitle(tr("Remote Cert Type"));
    m_currentRemoteCertType = "default";
    QString curRemoteCertOption = RemoteCertTypeStrMap.at(0).first;
    for (auto it = RemoteCertTypeStrMap.cbegin(); it != RemoteCertTypeStrMap.cend(); ++it) {
        m_remoteCertTypeChooser->comboBox()->addItem(it->first, it->second);
        if (it->second == m_dataMap.value("remote-cert-tls")) {
            m_currentRemoteCertType = it->second;
            curRemoteCertOption = it->first;
        }
    }

    m_remoteCertTypeChooser->setCurrentText(curRemoteCertOption);

    m_caCert->setTitle(tr("Key File"));
    m_caCert->edit()->setText(m_dataMap.value("ta"));

    m_customKeyDirection->setTitle(tr("Customize Key Direction"));
    m_customKeyDirection->setChecked(m_dataMap.keys().contains("ta-dir"));

    m_keyDirectionChooser->setTitle(tr("Key Direction"));
    m_keyDirectionChooser->comboBox()->addItem("0", "0");
    m_keyDirectionChooser->comboBox()->addItem("1", "1");
    m_currentKeyDirection = m_dataMap.value("ta-dir", "0");
    m_keyDirectionChooser->setCurrentText(m_currentKeyDirection);
    m_keyDirectionChooser->setVisible(m_customKeyDirection->checked());

    appendItem(m_remote);
    appendItem(m_remoteCertTypeChooser);
    appendItem(m_caCert);
    appendItem(m_customKeyDirection);
    appendItem(m_keyDirectionChooser);

    m_remote->textEdit()->installEventFilter(this);
    m_caCert->edit()->lineEdit()->installEventFilter(this);
}

void VpnTLSSection::initConnection()
{
    connect(m_remoteCertTypeChooser, &ComboxWidget::onSelectChanged, this, [ = ](const QString &dataSelected) {
        for (auto it = RemoteCertTypeStrMap.cbegin(); it != RemoteCertTypeStrMap.cend(); ++it) {
            if (it->first == dataSelected) {
                m_currentRemoteCertType = it->second;
                break;
            }
        }
    });

    connect(m_customKeyDirection, &SwitchWidget::checkedChanged, m_keyDirectionChooser, &ComboxWidget::setVisible);

    connect(m_keyDirectionChooser, &ComboxWidget::onSelectChanged, this, [ = ](const QString &dataSelected) {
        m_currentKeyDirection = dataSelected;
    });

    connect(m_caCert, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnTLSSection::requestFrameAutoHide);

    connect(m_remoteCertTypeChooser, &ComboxWidget::onIndexChanged, this, &VpnTLSSection::editClicked);
    connect(m_customKeyDirection, &SwitchWidget::checkedChanged, this, &VpnTLSSection::editClicked);
    connect(m_keyDirectionChooser, &ComboxWidget::onIndexChanged, this, &VpnTLSSection::editClicked);
}

bool VpnTLSSection::eventFilter(QObject *watched, QEvent *event)
{
    // 实现鼠标点击编辑框，确定按钮激活，统一网络模块处理，捕捉FocusIn消息
    if (event->type() == QEvent::FocusIn) {
        if (dynamic_cast<QLineEdit *>(watched))
            Q_EMIT editClicked();
    }

    return QWidget::eventFilter(watched, event);
}
