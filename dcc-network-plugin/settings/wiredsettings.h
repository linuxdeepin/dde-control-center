// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef WIREDSETTINGS_H
#define WIREDSETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class EthernetSection;

class WiredSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit WiredSettings(ConnectionSettings::Ptr connSettings, const QString devicePath, QWidget *parent = nullptr);
    virtual ~WiredSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE;
    void resetConnectionInterfaceName() Q_DECL_OVERRIDE;

private:
    EthernetSection *m_ethernetSection;
    QString m_devicePath;
};

#endif /* WIREDSETTINGS_H */
