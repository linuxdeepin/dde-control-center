// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "operation/dockdbusproxy.h"

class DccDockExport : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DockDBusProxy *dockInter MEMBER m_dockDbusProxy CONSTANT)
    Q_PROPERTY(DockPluginModel *pluginModel MEMBER m_pluginModel CONSTANT)
public:
    explicit DccDockExport(QObject *parent = nullptr);

private:
    void initData();
    
public Q_SLOTS:
    void reload();

private:
    DockDBusProxy *m_dockDbusProxy;
    DockPluginModel *m_pluginModel;
};
