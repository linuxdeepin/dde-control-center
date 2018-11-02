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

#include "pppsection.h"

#include <QDebug>

using namespace dcc::network;
using namespace dcc::widgets;
using namespace NetworkManager;

PPPSection::PPPSection(NetworkManager::PppSetting::Ptr pppSetting, QFrame *parent)
    : AbstractSection(tr("PPP"), parent),
      m_pppSetting(pppSetting),
      m_mppeEnable(new SwitchWidget(this)),
      m_mppe128(new SwitchWidget(this)),
      m_mppeStateful(new SwitchWidget(this)),
      m_refuseEAP(new SwitchWidget(this)),
      m_refusePAP(new SwitchWidget(this)),
      m_refuseCHAP(new SwitchWidget(this)),
      m_refuseMSCHAP(new SwitchWidget(this)),
      m_refuseMSCHAP2(new SwitchWidget(this)),
      m_noBSDComp(new SwitchWidget(this)),
      m_noDeflate(new SwitchWidget(this)),
      m_noVJComp(new SwitchWidget(this)),
      m_lcpEchoInterval(new SwitchWidget(this))
{
    initStrMaps();
    initUI();
    initConnection();

    onMppeEnableChanged(m_mppeEnable->checked());
}

PPPSection::~PPPSection()
{
}

bool PPPSection::allInputValid()
{
    // nothing to check
    return true;
}

void PPPSection::saveSettings()
{
    m_pppSetting->setRequireMppe(m_mppeEnable->checked());
    m_pppSetting->setRequireMppe128(m_mppe128->checked());
    m_pppSetting->setMppeStateful(m_mppeStateful->checked());
    m_pppSetting->setRefuseEap(m_refuseEAP->checked());
    m_pppSetting->setRefusePap(m_refusePAP->checked());
    m_pppSetting->setRefuseChap(m_refuseCHAP->checked());
    m_pppSetting->setRefuseMschap(m_refuseMSCHAP->checked());
    m_pppSetting->setRefuseMschapv2(m_refuseMSCHAP2->checked());
    m_pppSetting->setNoBsdComp(m_noBSDComp->checked());
    m_pppSetting->setNoDeflate(m_noDeflate->checked());
    m_pppSetting->setNoVjComp(m_noVJComp->checked());

    m_pppSetting->setLcpEchoInterval(m_lcpEchoInterval->checked() ? 30 : 0);
    m_pppSetting->setLcpEchoFailure(m_lcpEchoInterval->checked() ? 5 : 0);

    m_pppSetting->setInitialized(true);
}

void PPPSection::initStrMaps()
{
    OptionsStrMap = {
        {tr("Refuse EAP Authentication"), "refuse-eap"},
        {tr("Refuse PAP Authentication"), "refuse-pap"},
        {tr("Refuse CHAP Authentication"), "refuse-chap"},
        {tr("Refuse MSCHAP Authentication"), "refuse-mschap"},
        {tr("Refuse MSCHAPv2 Authentication"), "refuse-mschapv2"},
        {tr("No BSD Data Compression"), "nobsdcomp"},
        {tr("No Deflate Data Compression"), "nodeflate"},
        {tr("No TCP Header Compression"), "no-vj-comp"},
        {tr("No Protocol Field Compression"), "nopcomp"},
        {tr("No Address/Control Compression"), "noaccomp"},
        {tr("Send PPP Echo Packets"), "lcp-echo-interval"}
    };
}

void PPPSection::initUI()
{
    bool empty = m_pppSetting->toMap().isEmpty();

    m_mppeEnable->setTitle(tr("Use MPPE"));
    m_mppeEnable->setChecked(m_pppSetting->requireMppe());

    m_mppe128->setTitle(tr("128-bit MPPE"));
    m_mppe128->setChecked(m_pppSetting->requireMppe128());

    m_mppeStateful->setTitle(tr("Stateful MPPE"));
    m_mppeStateful->setChecked(m_pppSetting->mppeStateful());

    m_refuseEAP->setTitle(OptionsStrMap.key("refuse-eap"));
    m_refuseEAP->setChecked(m_pppSetting->refuseEap());

    m_refusePAP->setTitle(OptionsStrMap.key("refuse-pap"));
    m_refusePAP->setChecked(m_pppSetting->refusePap());

    m_refuseCHAP->setTitle(OptionsStrMap.key("refuse-chap"));
    m_refuseCHAP->setChecked(m_pppSetting->refuseChap());

    m_refuseMSCHAP->setTitle(OptionsStrMap.key("refuse-mschap"));
    m_refuseMSCHAP->setChecked(m_pppSetting->refuseMschap());

    m_refuseMSCHAP2->setTitle(OptionsStrMap.key("refuse-mschapv2"));
    m_refuseMSCHAP2->setChecked(m_pppSetting->refuseMschapv2());

    m_noBSDComp->setTitle(OptionsStrMap.key("nobsdcomp"));
    m_noBSDComp->setChecked(m_pppSetting->noBsdComp());

    m_noDeflate->setTitle(OptionsStrMap.key("nodeflate"));
    m_noDeflate->setChecked(m_pppSetting->noDeflate());

    m_noVJComp->setTitle(OptionsStrMap.key("no-vj-comp"));
    m_noVJComp->setChecked(m_pppSetting->noVjComp());

    m_lcpEchoInterval->setTitle(OptionsStrMap.key("lcp-echo-interval"));
    if (empty) {
        m_lcpEchoInterval->setChecked(true);
    } else {
        m_lcpEchoInterval->setChecked(
                m_pppSetting->lcpEchoInterval() == 30 && m_pppSetting->lcpEchoFailure() == 5);
    }

    appendItem(m_mppeEnable);
    appendItem(m_mppe128);
    appendItem(m_mppeStateful);
    appendItem(m_refuseEAP);
    appendItem(m_refusePAP);
    appendItem(m_refuseCHAP);
    appendItem(m_refuseMSCHAP);
    appendItem(m_refuseMSCHAP2);
    appendItem(m_noBSDComp);
    appendItem(m_noDeflate);
    appendItem(m_noVJComp);
    appendItem(m_lcpEchoInterval);
}

void PPPSection::initConnection()
{
    connect(m_mppeEnable, &SwitchWidget::checkedChanged, this, &PPPSection::onMppeEnableChanged);
}

void PPPSection::onMppeEnableChanged(const bool checked)
{
    m_mppe128->setVisible(checked);
    m_mppeStateful->setVisible(checked);
}
