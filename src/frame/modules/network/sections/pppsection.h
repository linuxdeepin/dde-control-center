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

#ifndef PPPSECTION_H
#define PPPSECTION_H

#include "abstractsection.h"
#include "widgets/contentwidget.h"
#include "widgets/switchwidget.h"
#include "widgets/comboboxwidget.h"

#include <networkmanagerqt/pppsetting.h>

namespace dcc {
namespace network {

class PPPSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit PPPSection(NetworkManager::PppSetting::Ptr pppSetting, QFrame *parent = 0);
    virtual ~PPPSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onMppeEnableChanged(const bool checked);

private:
    QMap<QString, QString> OptionsStrMap;

    NetworkManager::PppSetting::Ptr m_pppSetting;

    dcc::widgets::SwitchWidget *m_mppeEnable;
    dcc::widgets::SwitchWidget *m_mppe128;
    dcc::widgets::SwitchWidget *m_mppeStateful;
    dcc::widgets::SwitchWidget *m_refuseEAP;
    dcc::widgets::SwitchWidget *m_refusePAP;
    dcc::widgets::SwitchWidget *m_refuseCHAP;
    dcc::widgets::SwitchWidget *m_refuseMSCHAP;
    dcc::widgets::SwitchWidget *m_refuseMSCHAP2;
    dcc::widgets::SwitchWidget *m_noBSDComp;
    dcc::widgets::SwitchWidget *m_noDeflate;
    dcc::widgets::SwitchWidget *m_noVJComp;
    dcc::widgets::SwitchWidget *m_lcpEchoInterval;
};

} /* network */
} /* dcc */

#endif /* PPPSECTION_H */
