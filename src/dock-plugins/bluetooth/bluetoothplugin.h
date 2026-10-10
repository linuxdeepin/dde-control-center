// SPDX-FileCopyrightText: 2016 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef BLUETOOTHPLUGIN_H
#define BLUETOOTHPLUGIN_H

#include "bluetoothitem.h"
#include "quickpanelwidget.h"
#include "pluginsiteminterface_v2.h"

#include <QScopedPointer>

class BluetoothPlugin : public QObject, public PluginsItemInterfaceV2
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterfaceV2)
    Q_PLUGIN_METADATA(IID ModuleInterface_iid_V2 FILE "bluetooth.json")

public:
    explicit BluetoothPlugin(QObject* parent = nullptr);

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface* proxyInter) override;
    void pluginStateSwitched() override;
    bool pluginIsAllowDisable() override { return true; }
    bool pluginIsDisable() override;
    QWidget* itemWidget(const QString& itemKey) override;
    QWidget* itemTipsWidget(const QString& itemKey) override;
    QWidget* itemPopupApplet(const QString& itemKey) override;
    const QString itemContextMenu(const QString& itemKey) override;
    void invokedMenuItem(const QString& itemKey, const QString& menuId, const bool checked) override;
    int itemSortKey(const QString& itemKey) override;
    void setSortKey(const QString& itemKey, const int order) override;
    void refreshIcon(const QString& itemKey) override;
    void pluginSettingsChanged() override;
    Dock::PluginFlags flags() const override { return Dock::Type_Quick | Dock::Quick_Panel_Multi | Dock::Attribute_Normal; }
    virtual void setMessageCallback(MessageCallbackFunc cb) override { m_messageCallback = cb; }
    virtual QString message(const QString &message) override;

private:
    void refreshPluginItemsVisible();
    void setEnableState(bool enable);
    void notifySupportFlagChanged(bool supportFlag);

private:
    AdaptersManager* m_adapterManager;
    QScopedPointer<BluetoothItem> m_bluetoothItem;
    MessageCallbackFunc m_messageCallback;
    bool m_enableState;
};

#endif // BLUETOOTHPLUGIN_H
