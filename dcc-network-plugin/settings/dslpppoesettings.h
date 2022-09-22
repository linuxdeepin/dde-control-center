// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef DSLPPPOESETTINGS_H
#define DSLPPPOESETTINGS_H

#include "abstractsettings.h"

#include <networkmanagerqt/connectionsettings.h>

class EthernetSection;

class DslPppoeSettings : public AbstractSettings
{
    Q_OBJECT

public:
    explicit DslPppoeSettings(ConnectionSettings::Ptr connSettings, QString devicePath = QString(), QWidget *parent = nullptr);
    virtual ~DslPppoeSettings() Q_DECL_OVERRIDE;

protected:
    void initSections() Q_DECL_OVERRIDE;
    bool clearInterfaceName() Q_DECL_OVERRIDE;

private:
    QString m_devicePath;
    QObject *m_parent;
    EthernetSection *m_etherNetSection;
};

#endif /* DSLPPPOESETTINGS_H */
