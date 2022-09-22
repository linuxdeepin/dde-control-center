// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WIRELESSSETTINGS_H
#define WIRELESSSETTINGS_H

#include "abstractsettings.h"
#include "../parameterscontainer.h"

#include <networkmanagerqt/connectionsettings.h>

class WirelessSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit WirelessSettings(ConnectionSettings::Ptr connSettings, ParametersContainer::Ptr parameters, QWidget *parent = nullptr);
    virtual ~WirelessSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE;

private:
    ParametersContainer::Ptr m_parameter;
};

#endif /* WIRELESSSETTINGS_H */
