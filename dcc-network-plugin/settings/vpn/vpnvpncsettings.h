// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef VPNVPNCSETTINGS_H
#define VPNVPNCSETTINGS_H

#include "../abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

#include <QVBoxLayout>

using namespace NetworkManager;

class VpnVPNCSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit VpnVPNCSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = nullptr);
    virtual ~VpnVPNCSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE { return true; }
};

#endif /* VPNVPNCSETTINGS_H */
