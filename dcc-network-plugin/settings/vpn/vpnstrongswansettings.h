// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNSTRONGSWANSETTINGS_H
#define VPNSTRONGSWANSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class VpnStrongSwanSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnStrongSwanSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = nullptr);
    virtual ~VpnStrongSwanSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE { return true; }
};

#endif /* VPNSTRONGSWANSETTINGS_H */
