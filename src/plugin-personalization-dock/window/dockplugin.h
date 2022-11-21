/*
* Copyright (C) 2021 ~ 2022 Uniontech Software Technology Co.,Ltd.
*
* Author:     fanpengcheng <fanpengcheng@uniontech.com>
*
* Maintainer: fanpengcheng <fanpengcheng@uniontech.com>
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

private Q_SLOTS:
    void initMode(DCC_NAMESPACE::ComboxWidget *widget);
    void initPosition(DCC_NAMESPACE::ComboxWidget *widget);
    void initStatus(DCC_NAMESPACE::ComboxWidget *widget);
    void initShowRecent(QCheckBox *checkBox);
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
};

#endif // DOCKPLUGIN_H
