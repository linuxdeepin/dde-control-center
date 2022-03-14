/*
* Copyright (C) 2019 ~ 2022 Deepin Technology Co., Ltd.
*
* Author:     guoyao <guoyao@uniontech.com>

* Maintainer: guoyao <guoyao@uniontech.com>

* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* any later version.

* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
* GNU General Public License for more details.

* You should have received a copy of the GNU General Public License
* along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#pragma once

#include "interface/namespace.h"

#include <dtkcore_global.h>
#include <DMainWindow>

#include <QDBusContext>

/*************forward declaring start****************/

DCORE_BEGIN_NAMESPACE
class DConfig;
DCORE_END_NAMESPACE

DWIDGET_BEGIN_NAMESPACE
class DBackgroundGroup;
class DIconButton;
class DListView;
DWIDGET_END_NAMESPACE

class QStandardItemModel;
class QHBoxLayout;
class QVBoxLayout;
class QScrollArea;

namespace DCC_NAMESPACE
{

class ModuleObject;
class PluginManager;
class SearchWidget;
/**************forward declaring end*****************/

class MainWindow : public Dtk::Widget::DMainWindow, protected QDBusContext
{
    Q_OBJECT
public:
    enum class UrlType {
        Name,
        DisplayName
    };
    explicit MainWindow(QWidget *parent = nullptr);
    virtual ~MainWindow() override;

    /**
     * @brief 显示路径请求的页面，用于搜索或DBus接口
     * @param url 路径地址,从左至右搜索，如路径错误，只显示已搜索出的模块
     */
    void showPage(const QString &url, const UrlType &uType);

private:
    void openManual();
    void initUI();
    void initConfig();
    void loadModules();
    void toHome();
    void clearPage(QWidget *const widget);
    int getScrollPos(const int index);
    void showPage(ModuleObject *const module, const QString &url, const UrlType &uType);
    void showModule(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModuleBox(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModuleHSplit(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModuleVSplit(ModuleObject *const module, QWidget *const parent, const int index = -1);
    void showModulePage(ModuleObject *const module, QWidget *const parent, const int index = -1);
    QWidget *getPage(QWidget *const widget, const QString &title);

    inline void setCurrentModule(ModuleObject *const module) { m_currentModule = module; }
    inline ModuleObject *currentModule() const { return m_currentModule; }

private:
    QWidget                             *m_contentWidget;
    Dtk::Widget::DIconButton            *m_backwardBtn;         //回退按钮
    Dtk::Core::DConfig                  *m_dconfig;             //配置
    SearchWidget                        *m_searchWidget;        //搜索框
    ModuleObject                        *m_rootModule;
    ModuleObject                        *m_currentModule;
    PluginManager                       *m_pluginManager;
    QList<QWidget*>                     m_pages;                //保存终点的页面

};

} // namespace DCC_NAMESPACE


