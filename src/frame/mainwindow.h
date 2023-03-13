// SPDX-FileCopyrightText: 2018 - 2023 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
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

namespace DCC_NAMESPACE {

class ModuleObject;
class PluginManager;
class SearchWidget;

/**************forward declaring end*****************/

class MainWindow : public Dtk::Widget::DMainWindow, protected QDBusContext
{
    Q_OBJECT
public:
    enum class UrlType { Name, DisplayName };
    explicit MainWindow(QWidget *parent = nullptr);
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
    void loadModules(bool async, const QStringList &dirs);

    QString GrandSearchSearch(const QString json);
    bool GrandSearchStop(const QString &json);
    bool GrandSearchAction(const QString &json);

protected:
    void changeEvent(QEvent *event) override;
    void closeEvent(QCloseEvent *event) override;
    virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void openManual();
    void initUI();
    void initConfig();
    void configModule(QString url, ModuleObject *module);
    void toHome();
    void updateMainView();
    void clearPage(QWidget *const widget);
    void configLayout(QBoxLayout *const layout);
    void showPage(ModuleObject *const module, const QString &url, const UrlType &uType);
    ModuleObject *findModule(ModuleObject *const module,
                             const QString &url,
                             const UrlType &uType,
                             bool fuzzy = true);
    void showModule(ModuleObject *const module);
    void resizeCurrentModule(int size);

private Q_SLOTS:
    void onAddModule(ModuleObject *const module);
    void onRemoveModule(ModuleObject *const module);
    void onTriggered();
    void onChildStateChanged(ModuleObject *const child, uint32_t flag, bool state);
    void onModuleDataChanged();
    void updateModuleConfig(const QString &key);

private:
    Dtk::Widget::DIconButton *m_backwardBtn; // 回退按钮
    Dtk::Core::DConfig *m_dconfig;           // 配置
    SearchWidget *m_searchWidget;            // 搜索框
    ModuleObject *m_rootModule;
    QList<ModuleObject *> m_currentModule;
    PluginManager *m_pluginManager;

    QSet<QString> m_hideModule;
    QSet<QString> m_disableModule;
};

} // namespace DCC_NAMESPACE
