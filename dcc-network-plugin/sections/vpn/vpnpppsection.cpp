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

#include "vpnpppsection.h"

#include <QDebug>

#include <widgets/contentwidget.h>
#include <widgets/switchwidget.h>
#include <widgets/comboxwidget.h>

using namespace dcc::widgets;
using namespace NetworkManager;

VpnPPPSection::VpnPPPSection(VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN PPP"), parent)
    , m_vpnSetting(vpnSetting)
    , m_dataMap(vpnSetting->data())
    , m_mppeEnable(new SwitchWidget(this))
    , m_mppeChooser(new ComboxWidget(this))
    , m_mppeStateful(new SwitchWidget(this))
    , m_currentMppeMethod("require-mppe")
{
    setAccessibleName("VpnPPPSection");
    initStrMaps();

    bool mppeEnable = true;
    // means this is a exist connection
    if (!m_dataMap.isEmpty()) {
        for (auto it = MppeMethodStrMap.cbegin(); it != MppeMethodStrMap.cend(); ++it) {
            if (m_dataMap.contains(it->second)) {
                m_currentMppeMethod = it->second;
                mppeEnable = true;
                break;
            } else {
                mppeEnable = false;
            }
        }
    }
    m_mppeEnable->setChecked(mppeEnable);

    initUI();
    initConnection();

    onMppeEnableChanged(m_mppeEnable->checked());
}

VpnPPPSection::~VpnPPPSection()
{
}

bool VpnPPPSection::allInputValid()
{
    // nothing to check
    return true;
}

void VpnPPPSection::saveSettings()
{
    // retrieve the data map
    m_dataMap = m_vpnSetting->data();

    for (auto it = MppeMethodStrMap.cbegin(); it != MppeMethodStrMap.cend(); ++it) {
        if (m_mppeEnable->checked() && m_currentMppeMethod == it->second)
            m_dataMap.insert(it->second, "yes");
        else
            m_dataMap.remove(it->second);
    }

    if (m_mppeStateful->checked())
        m_dataMap.insert("mppe-stateful", "yes");
    else
        m_dataMap.remove("mppe-stateful");

    for (auto optionWidget : m_optionsWidgets) {
        const QString &optionName = optionWidget->property("option").toString();
        if (optionWidget->checked()) {
            if (optionName == "lcp-echo-interval") {
                m_dataMap.insert(optionName, "30");
                m_dataMap.insert("lcp-echo-failure", "5");
            } else {
                m_dataMap.insert(optionName, "yes");
            }
        } else {
            if (optionName == "lcp-echo-interval")
                m_dataMap.remove("lcp-echo-failure");

            m_dataMap.remove(optionName);
        }
    }

    m_vpnSetting->setData(m_dataMap);
    m_vpnSetting->setInitialized(true);
}

void VpnPPPSection::setSupportOptions(const QStringList &supportOptions)
{
    m_supportOptions = supportOptions;

    for (auto option : m_supportOptions) {
        const QString &str = OptionsStrMap.key(option);
        if (!str.isEmpty()) {
            SwitchWidget *optionWidget = new SwitchWidget(this);
            optionWidget->setTitle(str);
            optionWidget->setProperty("option", option);
            if (option == "lcp-echo-interval")
                optionWidget->setChecked(m_dataMap.contains(option) && m_dataMap.contains("lcp-echo-failure"));
            else
                optionWidget->setChecked(m_dataMap.value(option) == "yes");

            m_optionsWidgets.append(optionWidget);
            appendItem(optionWidget);
            connect(optionWidget, &SwitchWidget::checkedChanged, this, &VpnPPPSection::editClicked);
        }
    }
}

void VpnPPPSection::initStrMaps()
{
    MppeMethodStrMap = {
        { tr("All Available (default)"), "require-mppe" },
        { tr("40-bit (less secure)"), "require-mppe-40" },
        { tr("128-bit (most secure)"), "require-mppe-128" }
    };

    OptionsStrMap = {
        { tr("Refuse EAP Authentication"), "refuse-eap" },
        { tr("Refuse PAP Authentication"), "refuse-pap" },
        { tr("Refuse CHAP Authentication"), "refuse-chap" },
        { tr("Refuse MSCHAP Authentication"), "refuse-mschap" },
        { tr("Refuse MSCHAPv2 Authentication"), "refuse-mschapv2" },
        { tr("No BSD Data Compression"), "nobsdcomp" },
        { tr("No Deflate Data Compression"), "nodeflate" },
        { tr("No TCP Header Compression"), "no-vj-comp" },
        { tr("No Protocol Field Compression"), "nopcomp" },
        { tr("No Address/Control Compression"), "noaccomp" },
        { tr("Send PPP Echo Packets"), "lcp-echo-interval" }
    };
}

void VpnPPPSection::initUI()
{
    m_mppeEnable->setTitle(tr("Use MPPE"));
    m_mppeChooser->setTitle(tr("Security"));
    QStringList comboxOptions;
    QString curOption = "";
    for (auto it = MppeMethodStrMap.cbegin(); it != MppeMethodStrMap.cend(); ++it) {
        comboxOptions << it->first;
        if (it->second == m_currentMppeMethod)
            curOption = it->first;
    }

    m_mppeChooser->setComboxOption(comboxOptions);
    m_mppeChooser->setCurrentText(curOption);

    m_mppeStateful->setTitle(tr("Stateful MPPE"));
    m_mppeStateful->setChecked(m_dataMap.value("mppe-stateful") == "yes");

    appendItem(m_mppeEnable);
    appendItem(m_mppeChooser);
    appendItem(m_mppeStateful);
}

void VpnPPPSection::initConnection()
{
    connect(m_mppeEnable, &SwitchWidget::checkedChanged, this, &VpnPPPSection::onMppeEnableChanged);
    connect(m_mppeChooser, &ComboxWidget::onSelectChanged, this, &VpnPPPSection::onMppeMethodChanged);
    connect(m_mppeEnable, &SwitchWidget::checkedChanged, this, &VpnPPPSection::editClicked);
    connect(m_mppeChooser, &ComboxWidget::onIndexChanged, this, &VpnPPPSection::editClicked);
    connect(m_mppeStateful, &SwitchWidget::checkedChanged, this, &VpnPPPSection::editClicked);
}

void VpnPPPSection::onMppeEnableChanged(const bool checked)
{
    m_mppeChooser->setVisible(checked);
    m_mppeStateful->setVisible(checked);
}

void VpnPPPSection::onMppeMethodChanged(const QString &methodKey)
{
    for (auto it = MppeMethodStrMap.cbegin(); it != MppeMethodStrMap.cend(); ++it) {
        if (it->first == methodKey) {
            m_currentMppeMethod = it->second;
            break;
        }
    }
}
