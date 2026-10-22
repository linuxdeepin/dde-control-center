// SPDX-FileCopyrightText: 2020 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: LGPL-3.0-or-later

#ifndef WIRELESSCASTINGPLUGIN_H
#define WIRELESSCASTINGPLUGIN_H
#include "wirelesscastingitem.h"
#include "pluginsiteminterface_v2.h"

#include <QObject>

namespace dde {
namespace wirelesscasting {
class WirelessCastingPlugin : public QObject, public PluginsItemInterfaceV2
{
    Q_OBJECT
    Q_INTERFACES(PluginsItemInterfaceV2)
    Q_PLUGIN_METADATA(IID ModuleInterface_iid_V2 FILE "../wirelesscasting.json")

public:
    explicit WirelessCastingPlugin(QObject* parent = nullptr);

    const QString pluginName() const override { return "wireless-casting"; } // 在任务栏使用名称排序，不可随意修改
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
    virtual QString message(const QString &msg) override;

private:
    void notifySupportFlagChanged(bool supportFlag);

private:
    WirelessCastingItem *m_wirelessCastingItem;
    MessageCallbackFunc m_messageCallback;
};

} // namespace wirelesscasting
} // namespace dde
#endif // WIRELESSCASTINGPLUGIN_H
