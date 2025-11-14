// SPDX-FileCopyrightText: 2024 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "operation/dockdbusproxy.h"
#include <qtypes.h>

#include <QDBusInterface>

class DccDockExport : public QObject
{
    Q_OBJECT
    Q_PROPERTY(DockDBusProxy *dockInter MEMBER m_dockDbusProxy CONSTANT)
    Q_PROPERTY(DockPluginModel *pluginModel MEMBER m_pluginModel CONSTANT)
    Q_PROPERTY(int displayMode READ displayMode NOTIFY displayModeChanged)
public:
    explicit DccDockExport(QObject *parent = nullptr);
    ~DccDockExport() override;

    int displayMode() const;

private:
    void initData();
    void initDisplayModeConnection();
    
public Q_SLOTS:
    void loadPluginData();

Q_SIGNALS:
    void displayModeChanged();

private:
    DockDBusProxy *m_dockDbusProxy;
    DockPluginModel *m_pluginModel;
    QDBusInterface *m_displayInter;
    int m_displayMode;
    
private Q_SLOTS:
    void onDisplayModeChanged(uint mode);
    void onPropertiesChanged(const QString &interfaceName, const QMap<QString, QVariant> &changedProperties, const QStringList &invalidatedProperties);
};
