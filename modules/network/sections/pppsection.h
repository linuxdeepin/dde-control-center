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
#include "contentwidget.h"
#include "switchwidget.h"
#include "comboboxwidget.h"

#include <networkmanagerqt/pppsetting.h>

namespace dcc {
namespace widgets {

class PPPSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit PPPSection(NetworkManager::PppSetting::Ptr pppSetting, QFrame *parent = 0);
    virtual ~PPPSection();

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

Q_SIGNALS:
    void requestPage(ContentWidget * const page) const;

private:
    void initUI();
    void initConnection();
    void onMppeEnableChanged(const bool checked);

private:
    NetworkManager::PppSetting::Ptr m_pppSetting;

    SwitchWidget *m_mppeEnable;
    SwitchWidget *m_mppe128;
    SwitchWidget *m_mppeStateful;
    SwitchWidget *m_refuseEAP;
    SwitchWidget *m_refusePAP;
    SwitchWidget *m_refuseCHAP;
    SwitchWidget *m_refuseMSCHAP;
    SwitchWidget *m_refuseMSCHAP2;
    SwitchWidget *m_noBSDComp;
    SwitchWidget *m_noDeflate;
    SwitchWidget *m_noVJComp;
    SwitchWidget *m_lcpEchoInterval;
};

} /* widgets */
} /* dcc */

#endif /* PPPSECTION_H */
