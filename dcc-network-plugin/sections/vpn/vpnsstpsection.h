// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNSSTPSECTION_H
#define VPNSSTPSECTION_H

#include "vpnsection.h"

#include <networkmanagerqt/vpnsetting.h>

namespace dcc {
  namespace widgets {
    class FileChooseWidget;
    class SwitchWidget;
  }
}

class VpnSSTPSection : public VpnSection
{
    Q_OBJECT

public:
    explicit VpnSSTPSection(VpnSetting::Ptr vpnSetting, QFrame *parent = nullptr);
    virtual ~VpnSSTPSection() Q_DECL_OVERRIDE;

    void saveSettings() Q_DECL_OVERRIDE;

private:
    void initUI();

private:
    VpnSetting::Ptr m_vpnSetting;
    NMStringMap m_dataMap;

    FileChooseWidget *m_caFile;
    SwitchWidget *m_ignoreCAWarnings;
    SwitchWidget *m_useTLSExt;
};

#endif /* VPNSSTPSECTION_H */
