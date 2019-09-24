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

using namespace DCC_NAMESPACE::network;
using namespace dcc::widgets;
using namespace NetworkManager;

PPPSection::PPPSection(NetworkManager::PppSetting::Ptr pppSetting, QFrame *parent)
    : AbstractSection(tr("PPP"), parent)
    , m_pppSetting(pppSetting)
    , m_mppeEnable(new NetSwitchWidget(this))
    , m_mppe128(new NetSwitchWidget(this))
    , m_mppeStateful(new NetSwitchWidget(this))
    , m_refuseEAP(new NetSwitchWidget(this))
    , m_refusePAP(new NetSwitchWidget(this))
    , m_refuseCHAP(new NetSwitchWidget(this))
    , m_refuseMSCHAP(new NetSwitchWidget(this))
    , m_refuseMSCHAP2(new NetSwitchWidget(this))
    , m_noBSDComp(new NetSwitchWidget(this))
    , m_noDeflate(new NetSwitchWidget(this))
    , m_noVJComp(new NetSwitchWidget(this))
    , m_lcpEchoInterval(new NetSwitchWidget(this))
{
    initStrMaps();
    initUI();
    initConnection();

    onMppeEnableChanged(m_mppeEnable->switchWidget()->checked());
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
    m_pppSetting->setRequireMppe(m_mppeEnable->switchWidget()->checked());
    m_pppSetting->setRequireMppe128(m_mppe128->switchWidget()->checked());
    m_pppSetting->setMppeStateful(m_mppeStateful->switchWidget()->checked());
    m_pppSetting->setRefuseEap(m_refuseEAP->switchWidget()->checked());
    m_pppSetting->setRefusePap(m_refusePAP->switchWidget()->checked());
    m_pppSetting->setRefuseChap(m_refuseCHAP->switchWidget()->checked());
    m_pppSetting->setRefuseMschap(m_refuseMSCHAP->switchWidget()->checked());
    m_pppSetting->setRefuseMschapv2(m_refuseMSCHAP2->switchWidget()->checked());
    m_pppSetting->setNoBsdComp(m_noBSDComp->switchWidget()->checked());
    m_pppSetting->setNoDeflate(m_noDeflate->switchWidget()->checked());
    m_pppSetting->setNoVjComp(m_noVJComp->switchWidget()->checked());

    m_pppSetting->setLcpEchoInterval(m_lcpEchoInterval->switchWidget()->checked() ? 30 : 0);
    m_pppSetting->setLcpEchoFailure(m_lcpEchoInterval->switchWidget()->checked() ? 5 : 0);

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
    m_mppeEnable->switchWidget()->setChecked(m_pppSetting->requireMppe());

    m_mppe128->setTitle(tr("128-bit MPPE"));
    m_mppe128->switchWidget()->setChecked(m_pppSetting->requireMppe128());

    m_mppeStateful->setTitle(tr("Stateful MPPE"));
    m_mppeStateful->switchWidget()->setChecked(m_pppSetting->mppeStateful());

    m_refuseEAP->setTitle(OptionsStrMap.key("refuse-eap"));
    m_refuseEAP->switchWidget()->setChecked(m_pppSetting->refuseEap());

    m_refusePAP->setTitle(OptionsStrMap.key("refuse-pap"));
    m_refusePAP->switchWidget()->setChecked(m_pppSetting->refusePap());

    m_refuseCHAP->setTitle(OptionsStrMap.key("refuse-chap"));
    m_refuseCHAP->switchWidget()->setChecked(m_pppSetting->refuseChap());

    m_refuseMSCHAP->setTitle(OptionsStrMap.key("refuse-mschap"));
    m_refuseMSCHAP->switchWidget()->setChecked(m_pppSetting->refuseMschap());

    m_refuseMSCHAP2->setTitle(OptionsStrMap.key("refuse-mschapv2"));
    m_refuseMSCHAP2->switchWidget()->setChecked(m_pppSetting->refuseMschapv2());

    m_noBSDComp->setTitle(OptionsStrMap.key("nobsdcomp"));
    m_noBSDComp->switchWidget()->setChecked(m_pppSetting->noBsdComp());

    m_noDeflate->setTitle(OptionsStrMap.key("nodeflate"));
    m_noDeflate->switchWidget()->setChecked(m_pppSetting->noDeflate());

    m_noVJComp->setTitle(OptionsStrMap.key("no-vj-comp"));
    m_noVJComp->switchWidget()->setChecked(m_pppSetting->noVjComp());

    m_lcpEchoInterval->setTitle(OptionsStrMap.key("lcp-echo-interval"));
    if (empty) {
        m_lcpEchoInterval->switchWidget()->setChecked(true);
    } else {
        m_lcpEchoInterval->switchWidget()->setChecked(
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
    connect(m_mppeEnable->switchWidget(), &SwitchWidget::checkedChanged, this, &PPPSection::onMppeEnableChanged);
}

void PPPSection::onMppeEnableChanged(const bool checked)
{
    m_mppe128->setVisible(checked);
    m_mppeStateful->setVisible(checked);
}
