// SPDX-FileCopyrightText: 2018 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef NETWORKPLUGIN_H
#define NETWORKPLUGIN_H

#include "pluginsiteminterface.h"
#include "../common-plugin/utils.h"

#include <QSharedPointer>

#define NETWORK_KEY "network-item-key"

NETWORKPLUGIN_BEGIN_NAMESPACE
class NetworkPluginHelper;
class NetworkDialog;
class TrayIcon;
NETWORKPLUGIN_END_NAMESPACE

class QDBusMessage;

class NetworkPlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT

    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "network.json")

public Q_SLOTS:
    void showNetworkDialog();

public:
    explicit NetworkPlugin(QObject *parent = Q_NULLPTR);
    ~NetworkPlugin() Q_DECL_OVERRIDE;

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface *proxyInter) override;
    void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) override;
    void refreshIcon(const QString &itemKey) override;
    void pluginStateSwitched() override;
    bool pluginIsAllowDisable() override { return true; }
    bool pluginIsDisable() override;
    const QString itemCommand(const QString &itemKey) override;
    const QString itemContextMenu(const QString &itemKey) override;
    QWidget *itemWidget(const QString &itemKey) override;
    QWidget *itemTipsWidget(const QString &itemKey) override;
    QWidget *itemPopupApplet(const QString &itemKey) override;

    int itemSortKey(const QString &itemKey) override;
    void setSortKey(const QString &itemKey, const int order) override;

    void pluginSettingsChanged() override;

private:
    void loadPlugin();
    void refreshPluginItemsVisible();

private:
    QScopedPointer<NETWORKPLUGIN_NAMESPACE::NetworkPluginHelper> m_networkHelper;
    NETWORKPLUGIN_NAMESPACE::NetworkDialog *m_networkDialog;
    int m_clickTime;
    QSharedPointer<NETWORKPLUGIN_NAMESPACE::TrayIcon> m_trayIcon;
};

#endif // NETWORKPLUGIN_H
