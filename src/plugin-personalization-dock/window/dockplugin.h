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

#include "interface/moduleobject.h"
#include "interface/plugininterface.h"

#include "dtkwidget_global.h"

DCC_BEGIN_NAMESPACE
class ComboxWidget;
class TitledSliderItem;
class TitleLabel;
DCC_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DTipLabel;
class DListView;
DWIDGET_END_NAMESPACE

DCC_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

class DBusProxy;
class DockPlugin : public PluginInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "com.deepin.dde.ControlCenter.Dock" FILE "plugin-dock.json")
    Q_INTERFACES(DCC_NAMESPACE::PluginInterface)

public:
    explicit DockPlugin(QObject *parent = nullptr);

    virtual QString name() const override;
    virtual ModuleObject *module() override;
    virtual QString follow() const override;
    virtual int location() const override;

private:
    bool isCopyMode();

private Q_SLOTS:
    void initMode(ComboxWidget *widget);
    void initPosition(ComboxWidget *widget);
    void initStatus(ComboxWidget *widget);
    void initSizeSlider(TitledSliderItem *slider);
    void initScreenTitle(TitleLabel *label);
    void initScreen(ComboxWidget *widget);
    void initPluginTitle(TitleLabel *label);
    void initPluginTips(DTipLabel *label);
    void initPluginView(DListView *view);

private:
    QSharedPointer<DBusProxy> m_dbusProxy;
};

#endif // DOCKPLUGIN_H
