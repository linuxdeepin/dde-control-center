// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNPPTPSETTINGS_H
#define VPNPPTPSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class VpnPPTPSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnPPTPSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = Q_NULLPTR);
    virtual ~VpnPPTPSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE { return true; }
};

#endif /* VPNPPTPSETTINGS_H */
