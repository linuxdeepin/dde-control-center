// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef CONNECTIONHOTSPOTEDITPAGE_H
#define CONNECTIONHOTSPOTEDITPAGE_H

#include "connectioneditpage.h"

class ConnectionHotspotEditPage : public ConnectionEditPage
{
    Q_OBJECT

public:
    explicit ConnectionHotspotEditPage(const QString &devicePath, const QString &connUuid = QString(), QWidget *parent = Q_NULLPTR);
    virtual ~ConnectionHotspotEditPage() Q_DECL_OVERRIDE;

    void initSettingsWidget() Q_DECL_OVERRIDE;      // 初始化页面设置
};

#endif /* CONNECTIONHOTSPOTEDITPAGE_H */
