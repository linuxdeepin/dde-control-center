// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CONNECTIONWIRELESSEDITPAGE_H
#define CONNECTIONWIRELESSEDITPAGE_H

#include "connectioneditpage.h"

#include <networkmanagerqt/accesspoint.h>

class ConnectionWirelessEditPage : public ConnectionEditPage
{
    Q_OBJECT

public:
    explicit ConnectionWirelessEditPage(const QString &devPath, const QString &connUuid, const QString &apPath, bool isHidden = false, QWidget *parent = nullptr);
    virtual ~ConnectionWirelessEditPage() Q_DECL_OVERRIDE;

    // This method must be called after initialization
    void initSettingsWidgetFromAp();

private:
    void initApSecretType(AccessPoint::Ptr nmAp);
};

#endif /* CONNECTIONWIRELESSEDITPAGE_H */
