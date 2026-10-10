// SPDX-FileCopyrightText: 2011 - 2022 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef SOUNDPLUGIN_H
#define SOUNDPLUGIN_H

#include "pluginsiteminterface_v2.h"
#include "soundcontroller.h"
#include "soundview.h"
#include "soundquickpanel.h"

class SoundPlugin : public QObject, public PluginsItemInterfaceV2 {
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterfaceV2)
    Q_PLUGIN_METADATA(IID ModuleInterface_iid_V2 FILE "sound.json")

public:
    explicit SoundPlugin(QObject* parent = 0);

    const QString pluginName() const override;
    const QString pluginDisplayName() const override;
    void init(PluginProxyInterface* proxyInter) override;
    bool pluginIsAllowDisable() override { return true; }
    QWidget* itemWidget(const QString& itemKey) override;
    QWidget* itemTipsWidget(const QString& itemKey) override;
    QWidget* itemPopupApplet(const QString& itemKey) override;
    const QString itemContextMenu(const QString& itemKey) override;
    void invokedMenuItem(const QString& itemKey, const QString& menuId, const bool checked) override;
    int itemSortKey(const QString& itemKey) override;
    void setSortKey(const QString& itemKey, const int order) override;
    void refreshIcon(const QString& itemKey) override;
    void pluginSettingsChanged() override;
    Dock::PluginFlags flags() const override { return Dock::Type_Quick | Dock::Quick_Panel_Full; }
    virtual QString message(const QString &) override;

private:
    void refreshPluginItemsVisible();

private:
    QScopedPointer<SoundView> m_soundItem;
    QScopedPointer<SoundQuickPanel> m_soundWidget;
};

#endif // SOUNDPLUGIN_H
