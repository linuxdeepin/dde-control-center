// SPDX-FileCopyrightText: 2020 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef EYE_COMFORT_MODE_PLUGIN_H
#define EYE_COMFORT_MODE_PLUGIN_H

#include "pluginsiteminterface_v2.h"
#include "quickpanelwidget.h"
#include "eyecomfortmodeitem.h"

#include "dtkcore_global.h"

class EyeComfortModePlugin : public QObject, public PluginsItemInterfaceV2
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterfaceV2)
    Q_PLUGIN_METADATA(IID ModuleInterface_iid_V2 FILE "eye-comfort-mode.json")

public:
    explicit EyeComfortModePlugin(QObject *parent = nullptr);
    ~EyeComfortModePlugin();

    virtual const QString pluginName() const Q_DECL_OVERRIDE;
    virtual const QString pluginDisplayName() const Q_DECL_OVERRIDE;
    bool pluginIsAllowDisable() override { return true; }
    virtual void init(PluginProxyInterface *proxyInter) Q_DECL_OVERRIDE;
    virtual QWidget *itemWidget(const QString &itemKey) Q_DECL_OVERRIDE;
    virtual QWidget *itemPopupApplet(const QString &itemKey) Q_DECL_OVERRIDE;
    virtual QWidget *itemTipsWidget(const QString &itemKey) Q_DECL_OVERRIDE;
    virtual const QString itemContextMenu(const QString &itemKey) Q_DECL_OVERRIDE;
    virtual void invokedMenuItem(const QString &itemKey, const QString &menuId, const bool checked) Q_DECL_OVERRIDE;
    virtual int itemSortKey(const QString &itemKey) Q_DECL_OVERRIDE;
    virtual void setSortKey(const QString &itemKey, const int order) Q_DECL_OVERRIDE;
    virtual void refreshIcon(const QString &itemKey) Q_DECL_OVERRIDE;
    virtual Dock::PluginFlags flags() const override { return Dock::Type_Quick | Dock::Quick_Panel_Multi | Dock::Attribute_Normal; }
    virtual void setMessageCallback(MessageCallbackFunc cb) override { m_messageCallback = cb; }
    virtual QString message(const QString &message) override;

private:
    void notifySupportFlagChanged(bool supportFlag);

private:
    QScopedPointer<EyeComfortModeItem> m_eyeComfortModeItem;
    MessageCallbackFunc m_messageCallback;
};

#endif // EYE_COMFORT_MODE_PLUGIN_H
