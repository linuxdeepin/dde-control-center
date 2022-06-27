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
class QBoxLayout;
class QScrollArea;
class QAbstractItemView;

namespace DCC_NAMESPACE
{

class LayoutBase;
class LayoutManager;
class ModuleObject;
class PluginManager;
class SearchWidget;
struct WidgetData {
    ModuleObject *module;
    QWidget *w;
    LayoutBase *layout;
};
/**************forward declaring end*****************/

class MainWindow : public Dtk::Widget::DMainWindow, protected QDBusContext
{
    Q_OBJECT
public:
    enum class UrlType {
        Name,
        DisplayName
    };
    explicit MainWindow(bool async = true, QWidget *parent = nullptr);
    virtual ~MainWindow() override;

    /**
     * @brief 显示路径请求的页面，用于搜索接口
     * @param url 路径地址,从左至右搜索，如路径错误，只显示已搜索出的模块
     */
    void showPage(const QString &url, const UrlType &uType);
    /**
     * @brief 显示路径请求的页面，用于DBus接口,会延时到所有插件加载完成再调用
     * @param url 路径地址
     */
    void showPage(const QString &url);
    QString getAllModule() const;

protected:
    void changeEvent(QEvent *event) override;

private:
    void openManual();
    void initUI();
    void initConfig();
    void configModule(QString url,ModuleObject *module);
    void loadModules(bool async);
    void toHome();
    void updateMainView();
    void clearPage(QWidget *const widget);
    void configLayout(QBoxLayout *const layout);
    void showPage(ModuleObject *const module, const QString &url, const UrlType &uType);
    void showModule(ModuleObject *const module, QWidget *const parent);
    void resizeCurrentModule(int size);

private Q_SLOTS:
    void onAddModule(ModuleObject *const module);
    void onRemoveModule(ModuleObject *const module);
    void onTriggered();
    void onChildStateChanged(ModuleObject *const child, uint32_t flag, bool state);
    void updateModuleConfig(const QString &key);
    void delayUpdateLayoutCurrent(LayoutBase *layout, ModuleObject *child);
    void updateLayoutCurrent(LayoutBase *layout, ModuleObject *child);

private:
    QWidget                             *m_contentWidget;
    Dtk::Widget::DIconButton            *m_backwardBtn;         //回退按钮
    Dtk::Core::DConfig                  *m_dconfig;             //配置
    SearchWidget                        *m_searchWidget;        //搜索框
    ModuleObject                        *m_rootModule;
    DCC_NAMESPACE::LayoutManager        *m_layoutManager;
    QList<WidgetData>                   m_currentModule;
    PluginManager                       *m_pluginManager;
    bool                                m_loadAllFinished;

    QSet<QString> m_hideModule;
    QSet<QString> m_disableModule;
};

} // namespace DCC_NAMESPACE


