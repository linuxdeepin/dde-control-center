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

#include "vpnpppsection.h"

#include <QDebug>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

static const QMap<QString, QString> MppeMethodStrMap {
    {"All Available (default)", "require-mppe"},
    {"40-bit (less secure)", "require-mppe-40"},
    {"128-bit (most secure)", "require-mppe-128"}
};

static const QMap<QString, QString> OptionsStrMap {
    {"Refuse EAP Authentication", "refuse-eap"},
    {"Refuse PAP Authentication", "refuse-pap"},
    {"Refuse CHAP Authentication", "refuse-chap"},
    {"Refuse MSCHAP Authentication", "refuse-mschap"},
    {"Refuse MSCHAPv2 Authentication", "refuse-mschapv2"},
    {"No BSD Data Compression", "nobsdcomp"},
    {"No Deflate Data Compression", "nodeflate"},
    {"No TCP Header Compression", "no-vj-comp"},
    {"No Protocol Field Compression", "nopcomp"},
    {"No Address/Control Compression", "noaccomp"},
    {"Send PPP Echo Packets", "lcp-echo-interval"}
};

VpnPPPSection::VpnPPPSection(NetworkManager::VpnSetting::Ptr vpnSetting, QFrame *parent)
    : AbstractSection(tr("VPN PPP"), parent),
      m_vpnSetting(vpnSetting),
      m_mppeEnable(new SwitchWidget(this)),
      m_mppeChooser(new ComboBoxWidget(this)),
      m_mppeStateful(new SwitchWidget(this))
{
    m_dataMap = vpnSetting->data();

    m_currentMppeMethod = "require-mppe";
    bool mppeEnable = false;
    for (auto mppeMethod : MppeMethodStrMap.values()) {
        if (m_dataMap.contains(mppeMethod)) {
            m_currentMppeMethod = mppeMethod;
            mppeEnable = true;
            break;
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

    for (auto method : MppeMethodStrMap.values()) {
        if (m_mppeEnable->checked() && m_currentMppeMethod == method) {
            m_dataMap.insert(method, "yes");
        } else {
            m_dataMap.remove(method);
        }
    }

    if (m_mppeStateful->checked()) {
        m_dataMap.insert("mppe-stateful", "yes");
    } else {
        m_dataMap.remove("mppe-stateful");
    }

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
            if (optionName == "lcp-echo-interval") {
                m_dataMap.remove("lcp-echo-failure");
            }
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
            if (option == "lcp-echo-interval") {
                optionWidget->setChecked(m_dataMap.contains(option) && m_dataMap.contains("lcp-echo-failure"));
            } else {
                optionWidget->setChecked(m_dataMap.value(option) == "yes");
            }
            m_optionsWidgets.append(optionWidget);
            appendItem(optionWidget);
        } else {
            qDebug() << "Unsupport mppe option:" << option;
        }
    }
}

void VpnPPPSection::initUI()
{
    m_mppeEnable->setTitle(tr("Use MPPE"));

    m_mppeChooser->setTitle(tr("Security"));
    for (const QString &key : MppeMethodStrMap.keys()) {
        m_mppeChooser->appendOption(key, MppeMethodStrMap.value(key));
    }
    m_mppeChooser->setCurrent(m_currentMppeMethod);

    m_mppeStateful->setTitle("Stateful MPPE");
    m_mppeStateful->setChecked(m_dataMap.value("mppe-stateful") == "yes");

    appendItem(m_mppeEnable);
    appendItem(m_mppeChooser);
    appendItem(m_mppeStateful);
}

void VpnPPPSection::initConnection()
{
    connect(m_mppeEnable, &SwitchWidget::checkedChanged, this, &VpnPPPSection::onMppeEnableChanged);

    connect(m_mppeChooser, &ComboBoxWidget::requestPage, this, &VpnPPPSection::requestPage);
    connect(m_mppeChooser, &ComboBoxWidget::dataChanged, this, [=](const QVariant &data) {
        onMppeMethodChanged(data.value<QString>());
    });
}

void VpnPPPSection::onMppeEnableChanged(const bool checked)
{
    m_mppeChooser->setVisible(checked);
    m_mppeStateful->setVisible(checked);
}

void VpnPPPSection::onMppeMethodChanged(const QString &method)
{
    if (!method.isEmpty()) {
        m_currentMppeMethod = method;
    }
}
