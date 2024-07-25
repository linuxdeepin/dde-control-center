//SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
//SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DOCKPLUGIN_H
#define DOCKPLUGIN_H

#include "interface/pagemodule.h"
#include "interface/plugininterface.h"

#include "dtkwidget_global.h"

#include <QDBusInterface>

namespace DCC_NAMESPACE {
class ComboxWidget;
class TitledSliderItem;
class TitleLabel;
class ItemModule;
}

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
class DListView;
DWIDGET_END_NAMESPACE

DWIDGET_USE_NAMESPACE

class DockDBusProxy;
class QCheckBox;
class QDBusMessage;
class DockItemInfo;
class QStandardItemModel;
class DockPlugin : public DCC_NAMESPACE::PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Dock" FILE "plugin-dock.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)

public:
    explicit DockPlugin(QObject *parent = nullptr);

    virtual QString name() const override;
    virtual DCC_NAMESPACE::ModuleObject *module() override;
    virtual QString follow() const override;
    virtual QString location() const override;
};

class DockModuleObject : public DCC_NAMESPACE::PageModule
{
    Q_OBJECT

public:
    explicit DockModuleObject();

protected:
    bool eventFilter(QObject *watched, QEvent *event);
    
private:
    QIcon getIcon(const QString &dccIcon, bool isDeactivate, QString itemKey) const;
    void updateIcons();

private Q_SLOTS:
    void initMode(DCC_NAMESPACE::ComboxWidget *widget);
    void initPosition(DCC_NAMESPACE::ComboxWidget *widget);
    void initStatus(DCC_NAMESPACE::ComboxWidget *widget);
    void initSizeSlider(DCC_NAMESPACE::TitledSliderItem *slider);
    void initScreenTitle(DCC_NAMESPACE::TitleLabel *label);
    QWidget *initScreen(DCC_NAMESPACE::ModuleObject *module);
    void initPluginTitle(DCC_NAMESPACE::TitleLabel *label);
    void initPluginTips(DTipLabel *label);
    void initPluginView(DListView *view);
    void onDisplayPropertiesChanged(const QDBusMessage &dbusMessage);
    void updateScreenVisible();

private:
    QScopedPointer<DockDBusProxy> m_dbusProxy;
    QScopedPointer<QDBusInterface> m_displayProxy;
    DCC_NAMESPACE::ItemModule *m_screenTitle;
    DCC_NAMESPACE::ItemModule *m_screen;
    QStandardItemModel *m_pluginModel;
    DListView *m_view;
};

#endif // DOCKPLUGIN_H
