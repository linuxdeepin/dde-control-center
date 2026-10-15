// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#pragma once

#include "pluginsiteminterface_v2.h"
#include "brightnessquickpanel.h"
#include "brightnessitem.h"

class BrightnessPlugin : public QObject, public PluginsItemInterfaceV2
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterfaceV2)
    Q_PLUGIN_METADATA(IID ModuleInterface_iid_V2 FILE "brightness.json")

public:
    explicit BrightnessPlugin(QObject *parent = nullptr);
    ~BrightnessPlugin();

    const QString pluginName() const Q_DECL_OVERRIDE;
    const QString pluginDisplayName() const Q_DECL_OVERRIDE;
    void init(PluginProxyInterface *proxyInter) Q_DECL_OVERRIDE;
    bool pluginIsAllowDisable() override { return true; }
    QWidget *itemWidget(const QString &itemKey) Q_DECL_OVERRIDE;
    QWidget *itemPopupApplet(const QString &itemKey) Q_DECL_OVERRIDE;
    QWidget *itemTipsWidget(const QString &itemKey) Q_DECL_OVERRIDE;
    const QString itemContextMenu(const QString &itemKey) Q_DECL_OVERRIDE;
    void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) Q_DECL_OVERRIDE;
    Dock::PluginFlags flags() const Q_DECL_OVERRIDE { return Dock::Type_Quick | Dock::Quick_Panel_Full | Dock::Attribute_Normal; }
    QString message(const QString &message) Q_DECL_OVERRIDE;
    virtual void setMessageCallback(MessageCallbackFunc cb) override { m_messageCallback = cb; }

private:
    void notifySupportFlagChanged(bool supportFlag);

private:
    BrightnessQuickPanel *m_quickPanelWidget;
    BrightnessItem *m_item;
    MessageCallbackFunc m_messageCallback;
};
