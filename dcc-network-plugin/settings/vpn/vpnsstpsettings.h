// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNSSTPSETTINGS_H
#define VPNSSTPSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class VpnSSTPSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnSSTPSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = nullptr);
    virtual ~VpnSSTPSettings() override;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE { return true; }
};

#endif /* VPNSSTPSETTINGS_H */
