// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef HOTSPOTSETTINGS_H
#define HOTSPOTSETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class HotspotSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit HotspotSettings(ConnectionSettings::Ptr connSettings, QWidget *parent = Q_NULLPTR);
    virtual ~HotspotSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE;
};

#endif /* HOTSPOTSETTINGS_H */
