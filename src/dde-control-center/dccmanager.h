// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#ifndef DCCMANAGER_H
#define DCCMANAGER_H
#include "dccapp.h"
#include "dccdatarequest.h"
#include "dccobject.h"

#include <DConfig>
#include <DSysInfo>

#include <QDBusContext>
#include <QDBusMessage>
#include <QObject>
#include <QHash>

QT_BEGIN_NAMESPACE
class QWindow;
class QQmlApplicationEngine;
class QAbstractItemModel;
class QScreen;
QT_END_NAMESPACE

namespace dccV25 {
class NavigationModel;
class SearchModel;
class DccPluginLoader;
class DccPluginManager;
class DccImageProvider;

struct ModuleInfo
{
    QString url;
    QString displayName;
    QString pathDisplayName;
    int weight = 0;
    bool hidden = false;
};

class DccManager : public DccApp, protected QDBusContext
{
    Q_OBJECT
public:
    explicit DccManager(QObject *parent = nullptr);
    ~DccManager() override;

    static bool installTranslator(const QString &name, bool optional = false);
    void init();
    QQmlApplicationEngine *engine();
    void setMainWindow(QWindow *window);
    void setPlugins(const QStringList &plugins);
    void setShowOnNavigationReady(bool enabled);
    // 在 init() 前调用；异步读取单模块配置，不等待结果返回。
    void loadModuleConfigs(const QStringList &dirs, const QStringList &plugins);
    void loadModules(bool async, const QStringList &dirs);

    int width() const override;
    int height() const override;
    SearchBarState searchBarState() const override { return m_searchBarState; }
    int sidebarWidth() const override;
    void setSidebarWidth(int width) override;

    inline DccObject *root() const override { return m_root; }

    inline DccObject *activeObject() const override { return m_activeObject; }

    inline const QVector<DccObject *> &currentObjects() const override { return m_currentObjects; }

    inline const QVector<DccObject *> &triggeredObjects() const override { return m_triggeredObjects; }

    Q_INVOKABLE DccApp::UosEdition uosEdition() const;
    Q_INVOKABLE Dtk::Core::DSysInfo::ProductType productType() const;

    Q_INVOKABLE bool isTreeland() const;
    Q_INVOKABLE bool isServerSystem() const;
    Q_INVOKABLE bool isCommunitySystem() const;
    Q_INVOKABLE bool isDeepin() const;
    Q_INVOKABLE void logTimeline(const QString &stage, const QString &detail = QString()) const;

    inline const QSet<QString> &hideModule() const { return m_hideModule; }

public Q_SLOTS:
    DccObject *object(const QString &name) override;
    void addObject(DccObject *obj) override;
    void removeObject(DccObject *obj) override;
    void removeObject(const QString &name) override;
    void showPage(const QString &url) override;
    void showPage(DccObject *obj);
    void showPage(DccObject *obj, const QString &cmd) override;
    void toBack();
    QWindow *mainWindow() const override;
    QAbstractItemModel *navModel() const override;
    QSortFilterProxyModel *searchModel() const override;
    void cacheImage(const QString &id, const QSize &thumbnailSize = QSize());

    void show();
    void toggle();
    void showHelp();
    // DBus Search
    QString search(const QString &json) const;
    QString searchProxy(const QString &json) const;
    bool stop(const QString &json);
    bool action(const QString &json);
    QString GetAllModule();
    // DBus 数据通道：Get 返回请求键到值的映射；Set 逐项返回结果，空值表示已提交
    QVariantMap get(const QString &module, const QVariantMap &param);
    QVariantMap set(const QString &module, const QVariantMap &param);
    QList<ModuleInfo> moduleList() const;
    bool loadFinished() const;
    void onDccObjectDestroyed(DccObject *obj);

Q_SIGNALS:
    void activeItemChanged(QQuickItem *item, bool isIndicatorShown);
    void hideModuleChanged(const QSet<QString> &hideModule);
    // 模块数据变化，已带上插件 ID，供 adaptor 转成 DBus 信号
    void modulePropertiesChanged(const QString &module, const QVariantMap &properties);

private:
    void initConfig();
    void updateSearchBarState();
    void loadModuleConfigValues(const QSet<QString> &modulePaths);
    bool contains(const QSet<QString> &urls, const DccObject *obj);
    QStringList splitUrl(const QString &url, QString &targetName);
    bool isMatchByName(const QString &url, const QString &name);
    bool isMatch(const QString &url, const DccObject *obj);
    bool isEqualByName(const QString &url, const QString &name);
    bool isEqual(const QString &url, const DccObject *obj);
    bool isObjectAttached(const DccObject *obj) const;
    DccObject *findObject(const QString &url);
    QVector<DccObject *> findObjects(const QString &url, bool one = false);
    const DccObject *findParent(const DccObject *obj);
    bool eventFilter(QObject *watched, QEvent *event) override;
    bool isIndicatorShown(const QString &cmd) const;
    QString parseShowPageUrl(const QString &url, QString &cmd) const;
    void replyShowPageRequest(const QString &url, const QDBusMessage &message, bool found) const;
    void startPendingShow(const QString &url, const QDBusMessage &message);
    void tryStopTimeline();

private Q_SLOTS:
    void saveSize();
    void onWindowStateChanged();
    void handleScreenAdded(QScreen *screen);
    void waitShowPage(const QString &url, const QDBusMessage message);
    void clearShowParam();
    void handleNavigationReady();
    void handleLoadFinished();
    void handleShowReady();
    void tryShow();
    void tryShowFallback();
    void doShowPage(QPointer<DccObject> obj, const QString &cmd);
    void updateModuleConfig(const QString &key);
    void finishModuleConfigItem();
    void refreshModuleConfigState();
    void updateConfigState(DccObject *obj, uint32_t type);
    void onVisible(bool visible);
    void onObjectAdded(DccObject *obj);
    void onObjectRemoved(DccObject *obj);
    void onObjectDisplayChanged();
    bool addObjectToParent(DccObject *obj);
    bool removeObjectFromParent(DccObject *obj);
    void clearData();
    void waitLoadFinished() const;
    void doGetAllModule(const QDBusMessage message) const;
    void onPageStayTimeout();

    // 数据通道内部流程；DataRequest 定义在 dccdatarequest.h
public:
    void dispatchDataRequest(DataRequest *request);
    void completeDataRequest(DataRequest *request, const dccV25::DataReply &reply,
                             bool wasRunning);
    // 按请求 id 完成执行：释放该模块的执行位、回复并推进队列。
    // 不依赖请求对象存活：请求可能已因超时销毁，执行位仍须按 id 释放。
    void completeDataRequestById(quint64 requestId, const QString &module, const dccV25::DataReply &reply);
    // 执行单个数据请求（主线程、事件循环中调用）：完成后按请求 id 回填。
    // Q_INVOKABLE：由 invokeMethod 以字符串名排队调用，不引入匿名函数。
    Q_INVOKABLE void executeDataRequest(const QString &module, quint64 requestId,
                                        const QVariantMap &param, const QString &caller,
                                        bool isGet);
private:
    void failPendingRequests(const QString &reason);
    // 数据通道入口：准入检查（关闭、载荷、队列上限、调用者身份解析）、
    // 挂 20 秒服务端期限，然后按模块状态派发或排队。在 DBus 方法内调用，
    // 已 setDelayedReply(true)，实际回复由 DataRequest 完成。
    QVariantMap beginDataRequest(const QString &module, const QVariantMap &param, DataOperation op);
    // 后端进入终态时按新状态重新派发等待该模块的请求
    void onBackendStateChanged(DccPluginLoader *loader);
    // 发现阶段结束后，把队列中找不到 loader 的请求收束为 NotFound
    void completeUnknownModuleRequests();

private:
    DccObject *m_root;
    DccObject *m_activeObject;    // 当前定位的项
    DccObject *m_hideObjects;     // 隐藏的项
    DccObject *m_noAddObjects;    // 未找到父对象的
    DccObject *m_noParentObjects; // 没有父对象的

    QVector<DccObject *> m_currentObjects;   // 当前显示的页面路径，从根页面到当前页面
    QVector<DccObject *> m_triggeredObjects; // 用户交互触发的对象路径，从根菜单到当前子控件

    DccPluginManager *m_plugins;
    QPointer<QWindow> m_window;
    Dtk::Core::DConfig *m_dconfig;
    QSet<QString> m_hideModule;
    QSet<QString> m_disableModule;
    // 单模块配置：由各 subpath 的 visible=false / disable=true 派生的集合。
    QSet<QString> m_moduleHiddenPaths;
    QSet<QString> m_moduleDisabledPaths;
    bool m_moduleConfigReady = false; // 全部 subpath 返回后才置位：在此之前单模块配置不生效
    int m_moduleConfigPending = 0;    // 尚未返回的 subpath 数，归零即统一刷一次
    QQmlApplicationEngine *m_engine;
    NavigationModel *m_navModel;
    SearchModel *m_searchModel;
    DccImageProvider *m_imageProvider;
    int m_sidebarWidth;
    SearchBarState m_searchBarState = SearchBarVisible;
    // DBus 调用时对应项还没加载完成，此处保存跳转参数，加载完成后补跳。
    QTimer *m_showTimer;       // 已保存跳转参数时的显示触发
    QTimer *m_showFallbackTimer; // 插件卡死不显示界面的兜底
    QString m_showUrl;
    QDBusMessage m_showMessage;
    bool m_showPagePending;
    bool m_showLoadPage;
    bool m_needShow;
    bool m_windowShown;
    bool m_pageShown;
    bool m_allPluginsLoaded;
    QTimer *m_autoExitTimer;

    QHash<QString, QVector<DccObject *>> m_objMap; // 映射对象名称到对象指针列表，用于快速查找

    // DBus 数据通道：每个请求同一时刻最多一次回复
    QHash<quint64, DataRequest *> m_dataRequests;
    QHash<QString, QVector<DataRequest *>> m_pendingByModule;
    // 每个模块当前正在执行的数据请求（0 或 1）：保证同一后端串行处理。
    // 存请求 id 而非指针：请求可能因超时先销毁，执行位必须由完成回调按 id 释放。
    QHash<QString, quint64> m_runningByModule;
    quint64 m_nextRequestId = 1;

#ifdef HAVE_DDE_API_EVENTLOGGER
    QTimer *m_pageStayTimer;
    QStringList m_lastPageTags;
#endif
};
} // namespace dccV25
#endif // DCCMANAGER_H
