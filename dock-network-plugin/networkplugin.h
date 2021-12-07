/*
 * Copyright (C) 2011 ~ 2021 Deepin Technology Co., Ltd.
 *
 * Author:     sbw <sbw@sbw.so>
 *
 * Maintainer: sbw <sbw@sbw.so>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef NETWORKPLUGIN_H
#define NETWORKPLUGIN_H

#include "pluginsiteminterface.h"
#include "../common-plugin/utils.h"

#define NETWORK_KEY "network-item-key"

NETWORKPLUGIN_BEGIN_NAMESPACE
class NetworkPluginHelper;
class NetworkDialog;
NETWORKPLUGIN_END_NAMESPACE

class NetworkPlugin : public QObject, PluginsItemInterface
{
    Q_OBJECT

    Q_INTERFACES(PluginsItemInterface)
    Q_PLUGIN_METADATA(IID "com.deepin.dock.PluginsItemInterface" FILE "network.json")

Q_SIGNALS:
    void signalShowNetworkDialog();

public Q_SLOTS:
    void showNetworkDialog(QWidget *w)  const;
    void updatePoint();
    void lockFrontVisible(bool visible);

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
    void positionChanged(const Dock::Position position) override;

private:
    void loadPlugin();
    void refreshPluginItemsVisible();

private:
    QScopedPointer<NETWORKPLUGIN_NAMESPACE::NetworkPluginHelper> m_networkHelper;
    NETWORKPLUGIN_NAMESPACE::NetworkDialog *m_networkDialog;
    int m_clickTime;
};

#endif // NETWORKPLUGIN_H
