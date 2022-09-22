// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNOPENCONNECTSETTINGS_H
#define VPNOPENCONNECTSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class VpnOpenConnectSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnOpenConnectSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = nullptr);
    virtual ~VpnOpenConnectSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    inline bool clearInterfaceName() Q_DECL_OVERRIDE { return true; }
};

#endif /* VPNOPENCONNECTSETTINGS_H */
