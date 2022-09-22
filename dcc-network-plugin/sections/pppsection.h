// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef PPPSECTION_H
#define PPPSECTION_H

#include "abstractsection.h"

#include <networkmanagerqt/pppsetting.h>

namespace dcc {
  namespace widgets {
    class SwitchWidget;
  }
}

using namespace NetworkManager;
using namespace dcc::widgets;

class PPPSection : public AbstractSection
{
    Q_OBJECT

public:
    explicit PPPSection(PppSetting::Ptr pppSetting, QFrame *parent = nullptr);
    virtual ~PPPSection() Q_DECL_OVERRIDE;

    bool allInputValid() Q_DECL_OVERRIDE;
    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initStrMaps();
    void initUI();
    void initConnection();
    void onMppeEnableChanged(const bool checked);

private:
    QMap<QString, QString> OptionsStrMap;

    PppSetting::Ptr m_pppSetting;

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

#endif /* PPPSECTION_H */
