/*
 * Copyright (C) 2011 ~ 2018 Deepin Technology Co., Ltd.
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

#include "vpnsstpsection.h"

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

VpnSSTPSection::VpnSSTPSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : VpnSection(vpnSetting, parent),
      m_vpnSetting(vpnSetting),
      m_caFile(new FileChooseWidget(this)),
      m_ignoreCAWarnings(new SwitchWidget(this)),
      m_useTLSExt(new SwitchWidget(this))
{
    m_dataMap = vpnSetting->data();

    initUI();

    connect(m_caFile, &FileChooseWidget::requestFrameKeepAutoHide, this, &VpnSSTPSection::requestFrameAutoHide);
}

VpnSSTPSection::~VpnSSTPSection()
{
}

void VpnSSTPSection::saveSettings()
{
    // save parent settigns before
    VpnSection::saveSettings();

    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    if (m_caFile->edit()->text().isEmpty()) {
        m_dataMap.remove("ca-cert");
    } else {
        m_dataMap.insert("ca-cert", m_caFile->edit()->text());
    }

    if (m_ignoreCAWarnings->checked()) {
        m_dataMap.insert("ignore-cert-warn", "yes");
    } else {
        m_dataMap.remove("ignore-cert-warn");
    }

    if (m_useTLSExt->checked()) {
        m_dataMap.insert("tls-ext", "yes");
    } else {
        m_dataMap.remove("tls-ext");
    }

    m_vpnSetting->setData(m_dataMap);

    m_vpnSetting->setInitialized(true);
}

void VpnSSTPSection::initUI()
{
    m_caFile->setTitle(tr("CA File"));
    m_caFile->edit()->setText(m_dataMap.value("ca-cert"));

    m_ignoreCAWarnings->setTitle(tr("Ignore Certificate Warnings"));
    m_ignoreCAWarnings->setChecked(m_dataMap.value("ignore-cert-warn") == "yes");

    m_useTLSExt->setTitle(tr("Use TLS Hostname Extensions"));
    m_useTLSExt->setChecked(m_dataMap.value("tls-ext") == "yes");

    appendItem(m_caFile);
    appendItem(m_ignoreCAWarnings);
    appendItem(m_useTLSExt);
}
