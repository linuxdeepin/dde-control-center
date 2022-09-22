// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNL2TPSETTINGS_H
#define VPNL2TPSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class VpnL2tpSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnL2tpSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = Q_NULLPTR);
    virtual ~VpnL2tpSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    inline bool clearInterfaceName() Q_DECL_OVERRIDE { return true; }
};

#endif
