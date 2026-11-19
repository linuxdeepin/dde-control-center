// SPDX-FileCopyrightText: 2024 - 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccmanager.h"

#include "dccapp.h"
#include "dccbenchmark.h"
#include "dccimageprovider.h"
#include "dccdatarequest.h"
#include "dccfactory.h"
#include "dccpluginloader.h"
#include "dccmoduleconfig.h"
#include "dccobject_p.h"
#include "navigationmodel.h"
#include "pluginmanager.h"
#include "searchmodel.h"

#include <DGuiApplicationHelper>
#include <DIconTheme>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QDBusConnectionInterface>
#include <QDBusReply>
#include <QDir>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QGuiApplication>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLocale>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QScreen>
#include <QScopedPointer>
#include <QTimer>
#include <QTranslator>
#include <QWindow>

#ifdef HAVE_DDE_API_EVENTLOGGER
#include <dde-api/eventlogger.hpp>

// Event ID for control center page stay (10-digit number)
constexpr qint64 EVENT_LOGGER_CONTROL_CENTER_STAY = 1000610008;
#endif

DCORE_USE_NAMESPACE

namespace dccV25 {

const QString kAppId = QStringLiteral("org.deepin.dde.control-center");

namespace {

const QString kModulePathsConfigName = QStringLiteral("org.deepin.dde.control-center.modulepaths");

QStringList collectModulePathSubpaths(const QStringList &dirs, const QStringList &plugins)
{
    // 空 subpath 只读取控制中心自身的清单，插件清单按插件名读取。
    QStringList subpaths { QString() };
    for (const QString &dir : dirs) {
        QDir pluginDir(dir);
        if (!pluginDir.exists())
            continue;
        QFileInfoList pluginList = pluginDir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
        if (!pluginDir.isEmpty(QDir::Files))
            pluginList.prepend(QFileInfo(pluginDir.absolutePath()));
        for (const QFileInfo &plugin : pluginList) {
            const QString name = plugin.baseName();
            if (!plugins.isEmpty() && !plugins.contains(name))
                continue;
            subpaths.append(QLatin1Char('/') + name.toLower());
        }
    }
    subpaths.removeDuplicates();
    return subpaths;
}

QSet<QString> readModulePaths(const QStringList &subpaths)
{
    QSet<QString> paths;
    for (const QString &subpath : subpaths) {
        QScopedPointer<DConfig> config(DConfig::create(kAppId, kModulePathsConfigName, subpath));
        if (!config || !config->isValid()) {
            qWarning() << "Failed to read module paths config:" << kModulePathsConfigName << subpath;
            continue;
        }

        const QVariant entries = config->value(QStringLiteral("modulePaths"));
        if (!entries.canConvert<QVariantList>()) {
            qWarning() << "Expected a modulePaths list in:" << kModulePathsConfigName << subpath;
            continue;
        }

        const QVariantList list = entries.toList();
        for (const QVariant &entry : list) {
            const QString path = entry.toString().trimmed();
            const QStringList parts = path.split(QLatin1Char('/'), Qt::SkipEmptyParts);
            if (entry.metaType().id() != QMetaType::QString || parts.size() < 2 || parts.first() != QLatin1String("root")
                || parts.contains(QStringLiteral(".")) || parts.contains(QStringLiteral(".."))
                || path.contains(QLatin1Char('*'))) {
                qWarning() << "Invalid module path in:" << kModulePathsConfigName << subpath << entry;
                continue;
            }
            paths.insert(parts.join(QLatin1Char('/')));
        }
    }
    return paths;
}

} // namespace

// 数据接口在工作线程执行：工厂已归属主线程，调用期间不做并发写入
// 用 QRunnable 投递，避免把 lambda 直接交给 QThreadPool
class Work final : public QRunnable
{
public:
    Work(DccManager *manager, quint64 requestId, QString module, DccFactory_v20 *factory,
         bool isGet, QVariantMap param, QString caller, QString callerPath)
        : m_manager(manager)
        , m_requestId(requestId)
        , m_module(std::move(module))
        , m_factory(factory)
        , m_isGet(isGet)
        , m_param(std::move(param))
        , m_caller(std::move(caller))
        , m_callerPath(std::move(callerPath))
    {
    }

    void run() override
    {
        if (!m_factory) {
            return;
        }
        const auto reply = m_isGet ? m_factory->get(m_param, m_caller, m_callerPath)
                                   : m_factory->set(m_param, m_caller, m_callerPath);
        // 完成回填不依赖 DataRequest 存活：请求可能已因超时销毁，
        // 但执行位必须按 id 释放，否则该模块后续请求永远无法执行。
        QMetaObject::invokeMethod(
                m_manager,
                [manager = m_manager, id = m_requestId, module = m_module, reply] {
                    if (manager) {
                        manager->completeWorkById(id, module, reply);
                    }
                },
                Qt::QueuedConnection);
    }

private:
    DccManager *m_manager;
    quint64 m_requestId;
    QString m_module;
    DccFactory_v20 *m_factory;
    bool m_isGet;
    QVariantMap m_param;
    QString m_caller;
    QString m_callerPath;
};

const QString WidthConfig = QStringLiteral("width");
const QString HeightConfig = QStringLiteral("height");
const QString SearchBarStateConfig = QStringLiteral("searchBarState");
const QString MaximizedConfig = QStringLiteral("maximized");
const QString HideConfig = QStringLiteral("hideModule");
const QString DisableConfig = QStringLiteral("disableModule");
const QString ControlCenterIcon = QStringLiteral("preferences-system");
const QString ControlCenterGroupName = "com.deepin.dde-grand-search.group.dde-control-center-setting";

DccManager::DccManager(QObject *parent)
    : DccApp(parent)
    , m_root(new DccObject())
    , m_activeObject(nullptr)
    , m_hideObjects(new DccObject(this))
    , m_noAddObjects(new DccObject(this))
    , m_noParentObjects(new DccObject(this))
    , m_plugins(new DccPluginManager(this))
    , m_window(nullptr)
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", "org.deepin.dde.control-center", QString(), this))
    , m_engine(nullptr)
    , m_navModel(new NavigationModel(this))
    , m_searchModel(new SearchModel(this))
    , m_imageProvider(nullptr)
    , m_sidebarWidth(-1)
    , m_showTimer(nullptr)
    , m_showFallbackTimer(nullptr)
    , m_showPagePending(false)
    , m_showLoadPage(!isTreeland())
    , m_needShow(false)
    , m_windowShown(false)
    , m_pageShown(false)
    , m_allPluginsLoaded(false)
    , m_autoExitTimer(new QTimer(this))
    , m_dataPolicy(new WhitelistAccessPolicy())
    , m_dataPool(new QThreadPool())
#ifdef HAVE_DDE_API_EVENTLOGGER
    , m_pageStayTimer(nullptr)
#endif
{
    m_hideObjects->setName("_hide");
    m_noAddObjects->setName("_noAdd");
    m_noParentObjects->setName("_noParent");

    m_root->setName("root");
    m_root->setCanSearch(false);
    m_currentObjects.append(m_root);
    onObjectAdded(m_root);
    m_objMap.insert(m_root->name(), { m_root });
    QJSEngine::setObjectOwnership(m_root, QQmlEngine::CppOwnership);
    QJSEngine::setObjectOwnership(m_hideObjects, QQmlEngine::CppOwnership);
    QJSEngine::setObjectOwnership(m_noAddObjects, QQmlEngine::CppOwnership);
    QJSEngine::setObjectOwnership(m_noParentObjects, QQmlEngine::CppOwnership);

#ifdef HAVE_DDE_API_EVENTLOGGER
    qCInfo(dccLog) << "EventLogger initialized";

    m_pageStayTimer = new QTimer(this);
    m_pageStayTimer->setSingleShot(true);
    m_pageStayTimer->setInterval(2000); // 2 seconds
    connect(m_pageStayTimer, &QTimer::timeout, this, &DccManager::onPageStayTimeout);
#endif

    initConfig();
    connect(m_plugins, &DccPluginManager::addObject, this, &DccManager::addObject);
    connect(m_plugins, &DccPluginManager::navigationReady, this, &DccManager::handleNavigationReady, Qt::QueuedConnection);
    connect(m_plugins, &DccPluginManager::loadAllFinished, this, &DccManager::handleLoadFinished, Qt::QueuedConnection);
    // 每个 loader 的后端终态通知，替代原先只有成功路径才发出的布尔通知
    connect(m_plugins, &DccPluginManager::backendStateChanged,
            this, &DccManager::onBackendStateChanged, Qt::QueuedConnection);
    // 发现阶段结束立即冲刷排队请求，不依赖 navigationReady 的排队时序
    connect(m_plugins, &DccPluginManager::discoveryFinishedChanged,
            this, &DccManager::completeUnknownModuleRequests, Qt::DirectConnection);
    connect(m_plugins, &DccPluginManager::modulePropertiesChanged, this, &DccManager::modulePropertiesChanged, Qt::QueuedConnection);
    m_showTimer = new QTimer(this);
    m_showTimer->setInterval(60);
    m_showTimer->setSingleShot(true);
    connect(m_showTimer, &QTimer::timeout, this, &DccManager::tryShow);
    m_showFallbackTimer = new QTimer(this);
    m_showFallbackTimer->setSingleShot(true);
    connect(m_showFallbackTimer, &QTimer::timeout, this, &DccManager::tryShowFallback);
    m_showFallbackTimer->start(5000); // 防止插件卡死不显示界面

    m_autoExitTimer->setInterval(10000);
    m_autoExitTimer->setSingleShot(true);
    connect(m_autoExitTimer, &QTimer::timeout, this, [this]() {
        // 当主界面show出来之后不再执行自动退出
        if (m_showPagePending || m_needShow || !mainWindow() || mainWindow()->isVisible())
            return;
        // 仍有数据请求未回复（排队或执行中）：此时退出会把它变成 Cancelled。
        // 这里重新计时，等请求自然收束后再判断，而不是取消调用者的请求。
        if (!m_dataRequests.isEmpty()) {
            m_autoExitTimer->start();
            return;
        }
        QCoreApplication::quit();
    });
    m_autoExitTimer->start();
}

DccManager::~DccManager()
{
    qCDebug(dccLog()) << "delete dccManger";
    // 关闭顺序：先拒绝新请求并结束回复责任，再等在途任务真实退出，
    // 之后才允许停用后端、释放页面与模块。顺序反过来会出现
    // "deactive() 时后端操作仍在执行"的未定义行为。
    failPendingRequests(QStringLiteral("control center is shutting down"));
    // 在途任务可能仍在写后端，必须在 deactive() 之前真实结束。
    // 这里不能设超时门槛：超时后继续 deactive() 会与后端操作并发，
    // 而线程池析构本来也会无期限等待，设上限只是把未定义行为提前。
    if (m_dataPool) {
        m_dataPool->clear();
        m_dataPool->waitForDone();
    }
    clearData();
    delete m_dataPool;
    m_dataPool = nullptr;
    delete m_dataPolicy;
    m_dataPolicy = nullptr;
    delete m_plugins;
    m_plugins = nullptr;
    qCDebug(dccLog()) << "delete dccManger end";
}

bool DccManager::installTranslator(const QString &name, bool optional)
{
    const QStringList translateDirs = { TRANSLATE_READ_DIR,
                                        TRANSLATE_READ_DIR "/../v1.0", // 兼容旧版位置
                                        TRANSLATE_READ_DIR "/.." };

    // 插件翻译可选：内置插件（system/device/accounts 等）通常无独立 .qm，复用主程序翻译，
    // 此时直接跳过，避免触发 dtkgui "can not find qm files" 告警；
    // 第三方/新增插件若提供了独立 .qm 仍会正常加载。
    // 预检目录须与 DGuiApplicationHelper::loadTranslator 实际搜索路径保持一致：
    // 除此处传入的三个目录外，loadTranslator 还会追加 applicationDirPath/translations
    // 与 currentPath/translations。
    if (optional) {
        QStringList searchDirs = translateDirs;
        searchDirs << QDir(qApp->applicationDirPath()).absoluteFilePath("translations");
        searchDirs << QDir::current().absoluteFilePath("translations");

        bool hasQm = false;
        for (const QString &dir : std::as_const(searchDirs)) {
            if (!QDir(dir).entryList({ name + "_*.qm" }, QDir::Files).isEmpty()) {
                hasQm = true;
                break;
            }
        }
        if (!hasQm)
            return false;
    }

    return Dtk::Gui::DGuiApplicationHelper::loadTranslator(name, translateDirs, { QLocale() });
}

void DccManager::init()
{
    if (m_engine)
        return;

    QQmlEngine::setObjectOwnership(dccV25::DccApp::instance(), QQmlEngine::CppOwnership);
    qmlRegisterSingletonInstance("org.deepin.dcc", 1, 0, "DccApp", dccV25::DccApp::instance());

    m_engine = new QQmlApplicationEngine(this);
    m_imageProvider = new DccImageProvider();
    m_engine->addImageProvider("DccImage", m_imageProvider);
}

QQmlApplicationEngine *DccManager::engine()
{
    return m_engine;
}

void DccManager::setMainWindow(QWindow *window)
{
    m_window = window;
    connect(m_window, &QWindow::widthChanged, this, &DccManager::saveSize);
    connect(m_window, &QWindow::heightChanged, this, &DccManager::saveSize);
    connect(m_window, &QWindow::windowStateChanged, this, &DccManager::saveSize);
    // Persist the maximized flag on the authoritative state-change event only,
    // not on the high-frequency widthChanged/heightChanged resize signals.
    connect(m_window, &QWindow::windowStateChanged, this, &DccManager::onWindowStateChanged);
    connect(qGuiApp, &QGuiApplication::screenAdded, this, &DccManager::handleScreenAdded);
    m_window->installEventFilter(this);
    DccObject *group = m_window->property("groupObjects").value<DccObject *>();
    if (group) {
        addObject(group);
    }
}

void DccManager::setPlugins(const QStringList &plugins)
{
    m_plugins->setPlugins(plugins);
}

void DccManager::setShowOnNavigationReady(bool enabled)
{
    m_plugins->setShowOnNavigationReady(enabled);
}

void DccManager::loadModules(bool async, const QStringList &dirs)
{
    // onAddModule(m_rootModule);
    m_plugins->loadModules(m_root, async, dirs, m_engine);
    // showModule(m_rootModule);
}

int DccManager::width() const
{
    auto w = m_dconfig->value(WidthConfig).toInt();
    return w > 630 ? w : 630;
}

int DccManager::height() const
{
    auto h = m_dconfig->value(HeightConfig).toInt();
    return h >= 400 ? h : 530;
}

int DccManager::sidebarWidth() const
{
    return m_sidebarWidth;
}

void DccManager::setSidebarWidth(int width)
{
    if (width > 0 && m_sidebarWidth != width) {
        m_sidebarWidth = width;
        m_dconfig->setValue("sidebarWidth", m_sidebarWidth);
        Q_EMIT sidebarWidthChanged(m_sidebarWidth);
    }
}

DccApp::UosEdition DccManager::uosEdition() const
{
    DSysInfo::UosEdition edition = DSysInfo::uosEditionType();
    return DccApp::UosEdition(edition);
}

Q_INVOKABLE Dtk::Core::DSysInfo::ProductType DccManager::productType() const
{
    return DSysInfo::productType();
}

bool DccManager::isTreeland() const
{
    return Dtk::Gui::DGuiApplicationHelper::testAttribute(Dtk::Gui::DGuiApplicationHelper::IsWaylandPlatform);
}

bool DccManager::isServerSystem() const
{
    return DSysInfo::uosType() == DSysInfo::UosServer;
}

bool DccManager::isCommunitySystem() const
{
    return DSysInfo::uosEditionType() == DSysInfo::UosCommunity;
}

bool DccManager::isDeepin() const
{
    return DSysInfo::isDeepin();
}

void DccManager::logTimeline(const QString &stage, const QString &detail) const
{
    DccAppTimeline::instance().log(stage, detail);
}

DccObject *DccManager::object(const QString &name)
{
    return findObject(name);
}

inline void noRepeatAdd(QVector<DccObject *> &list, DccObject *obj)
{
    if (!list.contains(obj)) {
        list.append(obj);
    }
}

void DccManager::addObject(DccObject *obj)
{
    if (!obj)
        return;
    m_searchModel->beginBatch();
    QVector<DccObject *> objs;
    objs.append(obj);
    while (!objs.isEmpty()) {
        DccObject *o = objs.takeFirst();
        if (!o->name().isEmpty()) {
            m_objMap[o->name()].append(o);
            connect(o, &DccObject::objectDestroyed, this, &DccManager::onDccObjectDestroyed, Qt::UniqueConnection);
        }
        connect(o, &DccObject::addObject, this, &DccManager::addObject);
        connect(o, &DccObject::removeObject, this, qOverload<DccObject *>(&DccManager::removeObject));
        if (o->parentName().isEmpty()) {
            DccObject::Private::FromObject(m_noParentObjects)->addChild(o, false);
        } else {
            updateConfigState(o, DCC_CONFIG_HIDDEN);
            updateConfigState(o, DCC_CONFIG_DISABLED);
            if (!o->isVisibleToApp()) {
                connect(o, &DccObject::visibleToAppChanged, this, &DccManager::onVisible, Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
                DccObject::Private::FromObject(m_hideObjects)->addChild(o, false);
            } else if (!addObjectToParent(o)) {
                DccObject::Private::FromObject(m_noAddObjects)->addChild(o, false);
            }
        }

        objs.append(DccObject::Private::FromObject(o)->getObjects());
    }
    // 处理m_noAddObject
    objs.append(m_noAddObjects->getChildren());
    while (!objs.isEmpty()) {
        DccObject *o = objs.takeFirst();
        if (const DccObject *parentObj = findParent(o)) {
            DccObject::Private::FromObject(m_noAddObjects)->removeChild(o);
            DccObject::Private::FromObject(parentObj)->addChild(o);
            objs = m_noAddObjects->getChildren();
        }
    }
    m_searchModel->endBatch();
}

void DccManager::removeObject(DccObject *obj)
{
    if (!obj)
        return;
    removeObjectFromParent(obj);
}

void DccManager::removeObject(const QString &name)
{
    removeObject(findObject(name));
}

void DccManager::showPage(const QString &url)
{
    if (this->calledFromDBus()) {
        // show(); // 先查找再显示则注释掉此处
        int i = url.indexOf('?');
        QString cmd = i != -1 ? url.mid(i + 1) : QString();
        if (cmd.isEmpty() || isIndicatorShown(cmd)) {
            show();
        }
        auto message = this->message();
        setDelayedReply(true);
        QMetaObject::invokeMethod(this, &DccManager::waitShowPage, Qt::QueuedConnection, url, message);
    } else {
        QMetaObject::invokeMethod(this, &DccManager::waitShowPage, Qt::QueuedConnection, url, QDBusMessage());
    }
}

void DccManager::showPage(DccObject *obj)
{
    qCInfo(dccLog) << "showPage:" << obj;
    m_showPagePending = true;
    QMetaObject::invokeMethod(this, "doShowPage", Qt::QueuedConnection, QPointer<DccObject>(obj), QString());
}

void DccManager::showPage(DccObject *obj, const QString &cmd)
{
    qCInfo(dccLog) << "showPage:" << obj << cmd;
    m_showPagePending = true;
    QMetaObject::invokeMethod(this, "doShowPage", Qt::QueuedConnection, QPointer<DccObject>(obj), cmd);
}

void DccManager::toBack()
{
    int row = m_navModel->rowCount() - 2;
    if (row < 0) {
        showPage(m_root);
    } else {
        QString url = m_navModel->data(m_navModel->index(row, 0), NavigationModel::NavUrlRole).toString();
        if (!url.isEmpty()) {
            showPage(url);
        }
    }
}

QWindow *DccManager::mainWindow() const
{
    return m_window;
}

void DccManager::showHelp()
{
    QStringList helpTitles;
    for (auto it = m_currentObjects.crbegin(); it != m_currentObjects.crend(); ++it) {
        DccObject *object = *it;
        if (!object || object == m_root)
            continue;

        if (!(object->pageType() & DccObject::PageType::Menu) || object->displayName().isEmpty())
            continue;

        helpTitles.append(object->name());
    }

    const QString helpTitle = helpTitles.isEmpty() ? QStringLiteral("controlcenter") : helpTitles.join(QChar(0x1f));

    const QString &dmanInterface = "com.deepin.Manual.Open";
    QDBusMessage message = QDBusMessage::createMethodCall(dmanInterface, "/com/deepin/Manual/Open", dmanInterface, "OpenTitle");
    message << "dde" << helpTitle;
    QDBusConnection::sessionBus().asyncCall(message);
}

QString DccManager::search(const QString &json) const
{
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toLocal8Bit().data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();

        // 处理搜索任务, 返回搜索结果
        QJsonArray items;
        m_searchModel->setFilterRegularExpression(jsonObject.value("cont").toString());
        qCDebug(dccLog()) << "search key:" << jsonObject.value("cont").toString();
        for (int i = 0; i < m_searchModel->rowCount(); ++i) {
            QJsonObject jsonObj;
            jsonObj.insert("item", m_searchModel->data(m_searchModel->index(i, 0), SearchModel::SearchUrlRole).toString());
            jsonObj.insert("name", m_searchModel->data(m_searchModel->index(i, 0), SearchModel::SearchPlainTextRole).toString());
            jsonObj.insert("icon", ControlCenterIcon);
            jsonObj.insert("type", "application/x-dde-control-center-xx");
            qCDebug(dccLog()) << "search results:" << jsonObj["name"].toString();
            items.insert(i, jsonObj);
        }

        QJsonObject objCont;
        objCont.insert("group", ControlCenterGroupName);
        objCont.insert("items", items);

        QJsonArray arrConts;
        arrConts.insert(0, objCont);

        QJsonObject jsonResults;
        jsonResults.insert("ver", jsonObject.value("ver"));
        jsonResults.insert("mID", jsonObject.value("mID"));
        jsonResults.insert("cont", arrConts);

        QJsonDocument document;
        document.setObject(jsonResults);

        return document.toJson(QJsonDocument::Compact);
    }

    return QString();
}

QString DccManager::searchProxy(const QString &json) const
{
    if (this->calledFromDBus()) {
        if (!m_plugins->loadFinished()) {
            qDebug(dccLog) << "Delay to get searching due to plugins unloaded.";
            auto message = this->message();
            setDelayedReply(true);
            QObject::connect(
                    m_plugins,
                    &DccPluginManager::loadAllFinished,
                    this,
                    [this, json, message]() {
                        const auto &ret = this->search(json);
                        qDebug(dccLog) << "Searching finished, result size:" << ret.size();
                        QDBusConnection::sessionBus().send(message.createReply(ret));
                    },
                    Qt::SingleShotConnection);

            m_autoExitTimer->start();
            return {};
        }
    }
    return search(json);
}

bool DccManager::stop(const QString &)
{
    m_autoExitTimer->start();
    return true;
}

bool DccManager::action(const QString &json)
{
    QString searchName;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(json.toLocal8Bit().data());
    if (!jsonDocument.isNull()) {
        QJsonObject jsonObject = jsonDocument.object();
        if (jsonObject.value("action") == "openitem") {
            // 打开item的操作
            searchName = jsonObject.value("item").toString();
        }
    }

    show();
    showPage(searchName);
    m_autoExitTimer->start();
    return true;
}

QString DccManager::GetAllModule()
{
    auto message = this->message();
    setDelayedReply(true);
    QMetaObject::invokeMethod(this, &DccManager::doGetAllModule, Qt::QueuedConnection, message);
    return QString();
}
// Get/Set 共用的准入检查：关闭状态、载荷、队列上限
namespace {

// 把 DBus 调用者的唯一总线名解析成其可执行文件路径。
// 取不到（游标已失效、权限不足、/proc 不可用）时返回空串：
// 调用者必须能区分"未知"与"确定没有路径"，不能伪造一个看似可信的值。
QString callerBinaryPath(const QDBusConnection &connection, const QString &caller)
{
    if (caller.isEmpty() || !connection.interface()) {
        return {};
    }
    const QDBusReply<uint> pidReply = connection.interface()->servicePid(caller);
    if (!pidReply.isValid() || pidReply.value() == 0) {
        return {};
    }
    // /proc/<pid>/exe 指向真实可执行文件；进程已退出时读取失败，按未知处理
    const QString link = QStringLiteral("/proc/%1/exe").arg(pidReply.value());
    return QFileInfo(link).symLinkTarget();
}

} // namespace
QVariantMap DccManager::beginDataRequest(const QString &module, const QVariantMap &param,
                                         DataOperation op)
{
    const auto message = this->message();
    const auto connection = this->connection();
    setDelayedReply(true);

    const auto reject = [&connection, &message](const QString &name, const QString &text) {
        connection.send(message.createErrorReply(name, text));
        return QVariantMap();
    };

    // 关闭中直接给出明确的取消错误，不能让客户端等到超时
    if (m_plugins->isDeleting()) {
        return reject(QStringLiteral("org.deepin.dde.ControlCenter1.Error.Cancelled"),
                      QStringLiteral("control center is shutting down"));
    }
    if (param.size() > DataRequest::kMaxParamKeys) {
        return reject(QStringLiteral("org.freedesktop.DBus.Error.InvalidArgs"),
                      QStringLiteral("request has too many keys"));
    }
    if (m_dataRequests.size() >= DataRequest::kMaxPendingTotal) {
        return reject(QStringLiteral("org.deepin.dde.ControlCenter1.Error.Busy"),
                      QStringLiteral("too many pending requests"));
    }
    // 运行中的请求也计入单模块未完成数，否则实际可挂到上限 +1
    const int outstanding = m_pendingByModule.value(module).size()
            + (m_runningByModule.contains(module) ? 1 : 0);
    if (outstanding >= DataRequest::kMaxPendingPerModule) {
        return reject(QStringLiteral("org.deepin.dde.ControlCenter1.Error.Busy"),
                      QStringLiteral("too many pending requests for module: ") + module);
    }

    // 调用者身份在 DBus 上下文失效前解析好：工作线程里不能再调用 message()
    auto *request = new DataRequest(m_nextRequestId++, module, op, param,
                                    message.service(),
                                    callerBinaryPath(connection, message.service()),
                                    connection, message, this);

    // 服务端期限：到期后按 Timeout 回复一次，不再等待慢后端
    const quint64 id = request->id();
    QTimer::singleShot(DataRequest::kTimeoutMs, this, [this, id] {
        auto it = m_dataRequests.find(id);
        if (it == m_dataRequests.end()) {
            return;
        }
        auto *expired = it.value();
        m_dataRequests.erase(it);
        const QString module = expired->module();
        auto pending = m_pendingByModule.find(module);
        if (pending != m_pendingByModule.end()) {
            pending->removeAll(expired);
            if (pending->isEmpty()) {
                m_pendingByModule.erase(pending);
            }
        }
        // 超时只结束回复责任：若它是该模块正在执行的那一个，
        // 执行位仍应保留到任务真实结束，这里不提前放开，避免并发进入后端。
        expired->finishWithError(QStringLiteral("org.deepin.dde.ControlCenter1.Error.Timeout"),
                                 QStringLiteral("request timed out"));
    });

    dispatchDataRequest(request);
    return {};
}

QVariantMap DccManager::get(const QString &module, const QVariantMap &param)
{
    if (!calledFromDBus()) {
        return {};
    }
    return beginDataRequest(module, param, DataOperation::Get);
}

QVariantMap DccManager::set(const QString &module, const QVariantMap &param)
{
    if (!calledFromDBus()) {
        return {};
    }
    return beginDataRequest(module, param, DataOperation::Set);
}

bool DccManager::checkDataAccess(const QString &module, DataOperation op,
                                 const QString &caller) const
{
    Q_UNUSED(module)
    if (!m_dataPolicy) {
        return false;
    }
    // 会话总线上的唯一名就是调用者身份，不接受请求体里自称的身份
    if (caller.isEmpty()) {
        return op == DataOperation::Get;
    }
    return true; // 暂时全允许
}

void DccManager::dispatchDataRequest(DataRequest *request)
{
    const QString module = request->module();

    // 禁用/隐藏不拦截数据通道：按范围裁定由插件自行决定，框架不加限制。

    DccPluginLoader *loader = m_plugins->findLoader(module);
    if (!loader) {
        // 发现阶段未结束时不能断定模块不存在
        if (!m_plugins->discoveryFinished()) {
            m_pendingByModule[module].append(request);
            m_dataRequests.insert(request->id(), request);
            // 发现结束会由 DccPluginManager::checkNavigationFinished() 触发
            // backendDiscoveryFinished()，此时统一冲刷这些请求。
            return;
        }
        // 模块不存在优先于白名单：否则未知模块的 Set 会被误报为 AccessDenied
        completeDataRequest(request, { dccV25::DataStatus::NotFound, {},
                                       QStringLiteral("unknown module: ") + module }, false);
        return;
    }

    if (!checkDataAccess(module, request->operation(), request->caller())) {
        completeDataRequest(request, { dccV25::DataStatus::AccessDenied, {},
                                       QStringLiteral("operation not allowed") }, false);
        return;
    }

    // 后端状态决定排队还是直接拒绝：未就绪的请求必须等，不能先报"不支持"
    using BackendState = DccPluginLoader::BackendState;
    switch (loader->backendState()) {
    case BackendState::NotDiscovered:
    case BackendState::Loading:
        // 后端尚未就绪：挂起，由 backendStateChanged 统一派发或失败
        m_pendingByModule[module].append(request);
        m_dataRequests.insert(request->id(), request);
        return;
    case BackendState::Failed:
        completeDataRequest(request, { dccV25::DataStatus::Failed, {},
                                       loader->backendError().isEmpty()
                                           ? QStringLiteral("backend failed to load")
                                           : loader->backendError() }, false);
        return;
    case BackendState::NotSupported:
        completeDataRequest(request, { dccV25::DataStatus::NotSupported, {}, {} }, false);
        return;
    case BackendState::Ready:
        break;
    }

    // Ready 之后再按请求的具体操作检查能力位：Get 与 Set 可以单独支持（§12）
    const DccCapability needed = request->operation() == DataOperation::Get
            ? DccCapability::Get
            : DccCapability::Set;
    if (!loader->supportsCapability(needed)) {
        completeDataRequest(request, { dccV25::DataStatus::NotSupported, {}, {} }, false);
        return;
    }
    // 同一后端一次只执行一个数据操作：已有任务在跑就排队，避免并发写同一后端
    if (m_runningByModule.contains(module)) {
        m_pendingByModule[module].append(request);
        m_dataRequests.insert(request->id(), request);
        return;
    }
    m_runningByModule.insert(module, request->id());
    // Ready 已保证工厂存在；这里不再单独判空
    m_dataRequests.insert(request->id(), request);
    const bool isGet = request->operation() == DataOperation::Get;
    const quint64 requestId = request->id();
    const QVariantMap param = request->param();
    const QString caller = request->caller();
    const QString callerPath = request->callerPath();

    // Ready 状态保证工厂存在
    m_dataPool->start(new Work(this, requestId, module, loader->factory(), isGet, param,
                               caller, callerPath));
}

void DccManager::completeWorkById(quint64 requestId, const QString &module,
                                  const dccV25::DataReply &reply)
{
    // 先释放该模块的执行位：无论请求是否已因超时销毁，后端任务都真实结束了，
    // 必须允许队列中的下一个请求开始执行。
    const bool wasRunning = m_runningByModule.value(module) == requestId;
    if (wasRunning) {
        m_runningByModule.remove(module);
    }

    DataRequest *request = m_dataRequests.value(requestId);
    if (request) {
        // 请求仍在回复表中（未超时）：执行位已在上面释放，
        // 这里只需回复并按 FIFO 推进队列。
        completeDataRequest(request, reply, wasRunning);
        return;
    }

    // 请求已超时销毁：没有回复可发，但仍须推进该模块的队列
    if (!wasRunning) {
        return;
    }
    auto queue = m_pendingByModule.find(module);
    if (queue != m_pendingByModule.end() && !queue->isEmpty()) {
        DataRequest *next = queue->takeFirst();
        if (queue->isEmpty()) {
            m_pendingByModule.erase(queue);
        }
        if (next) {
            dispatchDataRequest(next);
        }
    }
}

void DccManager::completeDataRequest(DataRequest *request, const dccV25::DataReply &reply,
                                     bool wasRunning)
{
    if (!request) {
        return;
    }
    const QString module = request->module();
    m_dataRequests.remove(request->id());
    auto pending = m_pendingByModule.find(module);
    if (pending != m_pendingByModule.end()) {
        pending->removeAll(request);
        if (pending->isEmpty()) {
            m_pendingByModule.erase(pending);
        }
    }

    // wasRunning 由调用方判定：completeWorkById 已经按 id 释放了执行位。
    // 超时销毁的请求不再走到这里；未占用执行位的完成（能力不支持/策略拒绝/
    // 模块不存在）不得改动队列顺序，否则队首会被取出后因模块仍在执行而追加
    // 回队尾，破坏 FIFO。
    request->finishFromReply(reply);

    if (!wasRunning) {
        return;
    }
    auto queue = m_pendingByModule.find(module);
    if (queue != m_pendingByModule.end() && !queue->isEmpty()) {
        DataRequest *next = queue->takeFirst();
        if (queue->isEmpty()) {
            m_pendingByModule.erase(queue);
        }
        if (next) {
            dispatchDataRequest(next);
        }
    }
}


void DccManager::onBackendStateChanged(DccPluginLoader *loader)
{
    if (!loader) {
        return;
    }
    // 后端进入终态：把等待该模块的请求按新状态重新派发。
    // dispatchDataRequest() 自身按状态分支，Ready 时执行、其余立即给出明确错误。
    const auto pending = m_pendingByModule.take(loader->name());
    for (auto *request : pending) {
        if (!request) {
            continue;
        }
        dispatchDataRequest(request);
    }
}

void DccManager::completeUnknownModuleRequests()
{
    // 发现阶段结束后仍挂在队列里的模块，说明没有对应 loader：
    // 必须立即收束，否则只能等到 20 秒请求期限。
    QList<QString> unknown;
    for (auto it = m_pendingByModule.cbegin(); it != m_pendingByModule.cend(); ++it) {
        if (!m_plugins->findLoader(it.key())) {
            unknown.append(it.key());
        }
    }
    for (const QString &module : unknown) {
        const auto pending = m_pendingByModule.take(module);
        for (auto *request : pending) {
            if (!request) {
                continue;
            }
            completeDataRequest(request, { dccV25::DataStatus::NotFound, {},
                                           QStringLiteral("unknown module: ") + module }, false);
        }
    }
}

void DccManager::failPendingRequests(const QString &reason)
{
    const auto requests = m_dataRequests;
    m_dataRequests.clear();
    m_pendingByModule.clear();
    for (auto *request : requests) {
        if (!request) {
            continue;
        }
        request->finishWithError(QStringLiteral("org.deepin.dde.ControlCenter1.Error.Cancelled"), reason);
    }
}

void DccManager::onDccObjectDestroyed(DccObject *obj)
{
    if (m_plugins->isDeleting()) {
        return;
    }
    const QString &name = obj->name();
    if (name.isEmpty()) {
        return;
    }
    auto it = m_objMap.find(name);
    if (it == m_objMap.end()) {
        return;
    }
    it->removeOne(obj);
    if (it->isEmpty()) {
        m_objMap.erase(it);
    }
}

QAbstractItemModel *DccManager::navModel() const
{
    return m_navModel;
}

QSortFilterProxyModel *DccManager::searchModel() const
{
    return m_searchModel;
}

void DccManager::cacheImage(const QString &id, const QSize &thumbnailSize)
{
    if (m_imageProvider) {
        m_imageProvider->cacheImage(id, thumbnailSize);
    }
}

void DccManager::show()
{
    m_needShow = true;
    QWindow *w = DccManager::mainWindow();
    if (!w) {
        return;
    }
    if (!m_showLoadPage && !m_activeObject) {
        return;
    }
    if (w->windowStates() == Qt::WindowMinimized || !w->isVisible()) {
        if (m_dconfig->value(MaximizedConfig, false).toBool())
            w->showMaximized();
        else
            w->showNormal();
    }
    w->requestActivate();
    m_needShow = false;

    if (!m_windowShown) {
        DccAppTimeline::instance().log(QStringLiteral("window-visible"));
        m_windowShown = true;
    }
}

void DccManager::toggle()
{
    QWindow *w = DccManager::mainWindow();
    if (!w) {
        return;
    }

    w->setVisible(!w->isVisible());
    if (w->isVisible())
        w->requestActivate();
}

void DccManager::initConfig()
{
    if (!m_dconfig->isValid()) {
        qCWarning(dccLog()) << QString("DConfig is invalide, name:[%1], subpath[%2].").arg(m_dconfig->name(), m_dconfig->subpath());
        return;
    }

    updateModuleConfig(HideConfig);
    updateModuleConfig(DisableConfig);
    updateSearchBarState();
    m_sidebarWidth = m_dconfig->value("sidebarWidth", -1).toInt();
    if (m_sidebarWidth <= 0) {
        // 英文环境做特殊处理，加宽侧边栏
        QLocale locale;
        m_sidebarWidth = locale.language() == QLocale::English ? 210 : 180;
    }
    connect(m_dconfig, &DConfig::valueChanged, this, &DccManager::updateModuleConfig);
    connect(m_dconfig, &DConfig::valueChanged, this, [this](const QString &key) {
        if (key == SearchBarStateConfig)
            updateSearchBarState();
    });
}

void DccManager::updateSearchBarState()
{
    // FileBackend cannot safely read keys missing from older metadata.
    const int value = m_dconfig->keyList().contains(SearchBarStateConfig)
        ? m_dconfig->value(SearchBarStateConfig, SearchBarVisible).toInt()
        : SearchBarVisible;
    const auto state = value >= SearchBarVisible && value <= SearchBarHidden
        ? static_cast<SearchBarState>(value) : SearchBarVisible;
    if (m_searchBarState == state)
        return;

    m_searchBarState = state;
    Q_EMIT searchBarStateChanged(state);
}

bool DccManager::contains(const QSet<QString> &urls, const DccObject *obj)
{
    for (auto &&url : urls) {
        if (url.contains("*")) {
            if (isMatch(url, obj)) {
                return true;
            }
        } else {
            if (isEqual(url, obj)) {
                return true;
            }
        }
    }
    return false;
}

QStringList DccManager::splitUrl(const QString &url, QString &targetName)
{
    QStringList paths = url.split("/", Qt::SkipEmptyParts);
    if (!paths.isEmpty()) {
        targetName = paths.takeLast();
    }
    return paths;
}

bool DccManager::isMatchByName(const QString &url, const QString &name)
{
    Q_ASSERT(!url.isEmpty());

    QString objPath = "/" + name;
    int urlPos = url.size() - 1;
    int objPos = objPath.size() - 1;
    bool inWildcard = false; //  包含*
    while (urlPos >= 0 && objPos >= 0) {
        if (url[urlPos] == objPath[objPos]) {
            urlPos--;
            objPos--;
            inWildcard = false;
        } else if (url[urlPos] == '*') {
            inWildcard = true;
            urlPos--;
        } else if (inWildcard && objPath[objPos] != '/') {
            objPos--;
        } else {
            return false;
        }
    }
    if (inWildcard) {
        return true;
    }
    if (urlPos >= 0) {
        return true; // 等价于 objPath[0] == '/' || url[urlPos] == '/';
    }
    if (objPos >= 0) {
        return objPath[objPos] == '/' || url[0] == '/';
    }
    return true;
}

// url需要在调用处保证非空
bool DccManager::isMatch(const QString &url, const DccObject *obj)
{
    return isMatchByName(url, obj->parentName() + "/" + obj->name());
}

bool DccManager::isEqualByName(const QString &url, const QString &name)
{
    for (auto it = url.crbegin(), itObj = name.crbegin();; ++it, ++itObj) {
        if (it == url.crend()) {
            return itObj == name.crend() || (*itObj) == '/';
        }
        if (itObj == name.crend()) {
            return (*it) == '/';
        }
        if (*it != *itObj) {
            return false;
        }
    }
    return true;
}

bool DccManager::isEqual(const QString &url, const DccObject *obj)
{
    return isEqualByName(url, obj->parentName() + "/" + obj->name());
}

bool DccManager::isObjectAttached(const DccObject *obj) const
{
    while (obj && obj != m_root) {
        obj = DccObject::Private::FromObject(obj)->getParent();
    }
    return obj == m_root;
}

DccObject *DccManager::findObject(const QString &url)
{
    if (!m_root || url.isEmpty()) {
        return nullptr;
    }
    QString targetName;
    QStringList paths = splitUrl(url, targetName);
    if (targetName.isEmpty()) {
        return nullptr;
    }
    auto it = m_objMap.find(targetName);
    if (it == m_objMap.end()) {
        return nullptr;
    }
    if (paths.isEmpty()) {
        return it.value().first();
    }
    QString parentPath = paths.join("/");
    for (const auto &obj : it.value()) {
        if (isEqualByName(parentPath, obj->parentName())) {
            return obj;
        }
    }
    return nullptr;
}

QVector<DccObject *> DccManager::findObjects(const QString &url, bool one)
{
    if (!m_root || url.isEmpty()) {
        return {};
    }
    QString targetName;
    QStringList paths = splitUrl(url, targetName);
    if (targetName.isEmpty()) {
        return {};
    }
    QVector<DccObject *> rets;
    QVector<DccObject *> objs;
    if (!targetName.contains("*")) {
        auto it = m_objMap.find(targetName);
        if (it == m_objMap.end()) {
            return {};
        }
        objs = it.value();
    } else {
        targetName = "/" + targetName;
        for (auto it = m_objMap.begin(); it != m_objMap.end(); it++) {
            if (isMatchByName(targetName, it.key())) {
                objs.append(it.value());
            }
        }
    }
    if (paths.isEmpty()) {
        return objs;
    }
    QString parentPath = "/" + paths.join("/");
    for (auto &&obj : objs) {
        if (isMatchByName(parentPath, obj->parentName())) {
            rets.append(obj);
            if (one) {
                break;
            }
        }
    }
    return rets;
}

const DccObject *DccManager::findParent(const DccObject *obj)
{
    const QString &path = obj->parentName();
    const DccObject *p = DccObject::Private::FromObject(obj)->getRecommendedParent();
    const QObject *op = obj;
    if (p && !p->name().isEmpty() && isEqual(path, p)) {
        return p;
    }
    while (op) {
        op = op->parent();
        p = qobject_cast<const DccObject *>(op);
        if (p && !p->name().isEmpty() && isEqual(path, p)) {
            return p;
        }
    }
    qCDebug(dccLog()) << obj->name() << "find parent:" << path << ".Parent-child position error, traverse all objects to find.";
    p = findObject(path);
    return p;
}

bool DccManager::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress && watched == m_window && m_window) {
        QMouseEvent *e = static_cast<QMouseEvent *>(event);
        if (e->buttons() == Qt::LeftButton) {
            QQuickWindow *w = static_cast<QQuickWindow *>(m_window.get());
            if (w) {
                QQuickItem *focusItem = w->activeFocusItem();
                if (focusItem) {
                    QObject *popup = focusItem->property("popup").value<QObject *>();
                    if (!popup || !popup->property("visible").toBool()) {
                        QPointF point = focusItem->mapFromGlobal(e->globalPosition());
                        QRectF rect(0, 0, focusItem->width(), focusItem->height());
                        if (!rect.contains(point)) {
                            QQuickItem *item = w->property("sidebarPage").value<QQuickItem *>();
                            if (item) {
                                item->forceActiveFocus();
                            }
                        }
                    }
                }
            }
        }
    }
    return DccApp::eventFilter(watched, event);
}

bool DccManager::isIndicatorShown(const QString &cmd) const
{
    return cmd == "indicator=true";
}

void DccManager::saveSize()
{
    if (!m_window)
        return;
    if (!m_dconfig->isValid())
        return;

    const auto states = m_window->windowStates();
    const bool isMaximized = states.testFlag(Qt::WindowMaximized) || states.testFlag(Qt::WindowFullScreen);
    const bool visible = m_window->isVisible();

    // Only save normal size when visible and not maximized.
    // During maximization, widthChanged/heightChanged may fire with screen
    // dimensions before windowStateChanged updates the state.
    // On some platforms (e.g. Wayland), hiding a window may reset its state,
    // so we must not save the reset dimensions either.
    if (!visible || isMaximized)
        return;

    m_dconfig->setValue(WidthConfig, m_window->width());
    m_dconfig->setValue(HeightConfig, m_window->height());
}

void DccManager::onWindowStateChanged()
{
    if (!m_window)
        return;
    if (!m_dconfig->isValid())
        return;

    // Persist the maximized/fullscreen flag only on the state-change event so
    // the last window state can be restored on the next launch. Writing it here
    // (rather than inside saveSize) avoids touching DConfig on every resize.
    m_dconfig->setValue(MaximizedConfig,
        m_window->windowStates().testFlag(Qt::WindowMaximized)
            || m_window->windowStates().testFlag(Qt::WindowFullScreen));
}

void DccManager::handleScreenAdded(QScreen *screen)
{
    Q_UNUSED(screen)
    if (!m_window)
        return;

    // Requirement: when HDMI re-connected, if the control center is maximized,
    // restore it back to the default (normal) window size.
    if (!m_window->windowStates().testFlag(Qt::WindowMaximized))
        return;

    m_window->showNormal();

    QScreen *targetScreen = m_window->screen() ? m_window->screen() : qGuiApp->primaryScreen();
    if (!targetScreen)
        return;

    QRect avail = targetScreen->availableGeometry();
    int w = width();
    int h = height();

    // Clamp into the available area (avoid going off-screen)
    // Keep consistent with `DccWindow.qml` minimum sizes.
    constexpr int kMinW = 520;
    constexpr int kMinH = 400;
    w = qMax(kMinW, qMin(w, avail.width()));
    h = qMax(kMinH, qMin(h, avail.height()));

    m_window->resize(QSize(w, h));
    const int x = avail.x() + (avail.width() - w) / 2;
    const int y = avail.y() + (avail.height() - h) / 2;
    m_window->setPosition(QPoint(x, y));
    m_window->requestActivate();
}

QString DccManager::parseShowPageUrl(const QString &url, QString &cmd) const
{
    const int i = url.indexOf('?');
    cmd = i != -1 ? url.mid(i + 1) : QString();
    return url.mid(0, i).split('/', Qt::SkipEmptyParts).join('/'); // 移除多余的/
}

void DccManager::replyShowPageRequest(const QString &url, const QDBusMessage &message, bool found) const
{
    if (message.type() == QDBusMessage::InvalidMessage) {
        return;
    }

    if (found) {
        QDBusConnection::sessionBus().send(message.createReply());
    } else {
        QDBusConnection::sessionBus().send(message.createErrorReply(QDBusError::InvalidArgs, QString("not found url:") + url));
    }
}

void DccManager::startPendingShow(const QString &url, const QDBusMessage &message)
{
    m_showUrl = url;
    m_showMessage = message;
    m_showTimer->start();
}

void DccManager::waitShowPage(const QString &url, const QDBusMessage message)
{
    qCInfo(dccLog()) << "show page:" << url;
    clearShowParam();

    if (m_plugins->isDeleting()) {
        return;
    }

    DccObject *obj = nullptr;
    QString cmd;
    if (url.isEmpty()) {
        obj = m_root;
        showPage(obj, QString());
    } else {
        const QString path = parseShowPageUrl(url, cmd);
        const auto objs = findObjects(path);
        const auto it = std::find_if(objs.cbegin(), objs.cend(), [this](const DccObject *candidate) {
            return isObjectAttached(candidate);
        });
        obj = it == objs.cend() ? nullptr : *it;
        if (obj) {
            showPage(obj, cmd);
        } else if (!m_plugins->loadFinished()) {
            startPendingShow(url, message);
            return;
        }
    }

    replyShowPageRequest(url, message, obj);
}

void DccManager::clearShowParam()
{
    m_showTimer->stop();
    if (!m_showUrl.isEmpty()) {
        m_showUrl.clear();
        m_showMessage = QDBusMessage();
    }
}

void DccManager::handleNavigationReady()
{
    // 发现阶段结束：此时仍无 loader 的排队请求可判定为"模块不存在"
    completeUnknownModuleRequests();
    if (m_plugins->showOnNavigationReady() && m_showUrl.isEmpty()) {
        handleShowReady();
    }
}

void DccManager::handleLoadFinished()
{
    m_allPluginsLoaded = true;
    handleShowReady();
    tryStopTimeline();
}

void DccManager::handleShowReady()
{
    qCInfo(dccLog) << "handleShowReady:" << m_showUrl;
    if (!m_showUrl.isEmpty()) {
        tryShow();
    } else if (m_showFallbackTimer->isActive() && !m_activeObject && !m_showPagePending) {
        tryShowFallback();
    }
}

void DccManager::tryStopTimeline()
{
    if (m_allPluginsLoaded && m_pageShown) {
        DccAppTimeline::instance().stop();
    }
}

void DccManager::tryShow()
{
    if (m_showUrl.isEmpty()) {
        qCDebug(dccLog) << "tryShow: url is empty, skip";
        return;
    }
    if (!m_plugins->loadFinished()) {
        if (!m_plugins->isDeleting()) {
            m_showTimer->start();
        }
        return;
    }

    qCDebug(dccLog) << "tryShow: url =" << m_showUrl
                       << "loadFinished =" << m_plugins->loadFinished()
                       << "isDeleting =" << m_plugins->isDeleting();

    QString cmd;
    const QString path = parseShowPageUrl(m_showUrl, cmd);
    DccObject *obj = findObject(path);
    qCDebug(dccLog) << "tryShow: parsed path =" << path << "cmd =" << cmd << "found obj =" << (void *)obj;
    if (obj && isObjectAttached(obj)) {
        const QString url = m_showUrl;
        const QDBusMessage message = m_showMessage;
        clearShowParam();
        showPage(obj, cmd);
        qCInfo(dccLog) << "tryShow: page shown successfully, url =" << url;
        replyShowPageRequest(url, message, true);
    } else if (m_plugins->loadFinished()) {
        qCWarning(dccLog) << "tryShow: plugins loaded but object not found, path =" << path << "url =" << m_showUrl;
        const QString url = m_showUrl;
        const QDBusMessage message = m_showMessage;
        clearShowParam();
        replyShowPageRequest(url, message, false);
        if (!m_activeObject) {
            showPage(m_root, QString());
        }
    } else if (!m_plugins->isDeleting()) {
        qCDebug(dccLog) << "tryShow: plugins not ready, retrying...";
        m_showTimer->start();
    } else {
        qCWarning(dccLog) << "tryShow: plugins deleting, abort show, url =" << m_showUrl;
    }
}

void DccManager::tryShowFallback()
{
    if (m_plugins->isDeleting() || !m_showUrl.isEmpty() || m_activeObject || m_showPagePending) {
        return;
    }
    qCInfo(dccLog) << "tryShowFallback: home";
    m_showFallbackTimer->stop();
    showPage(m_root, QString());
}

void DccManager::doShowPage(QPointer<DccObject> obj, const QString &cmd)
{
    struct PendingGuard
    {
        DccManager *manager;
        ~PendingGuard() { manager->m_showPagePending = false; }
    } pendingGuard{ this };

    if (m_plugins->isDeleting() || !obj) {
        return;
    }
    qCInfo(dccLog) << "ShowPage:" << obj << " have cmd:" << !cmd.isEmpty();
    // 禁用首页
    if (obj == m_root) {
        if (m_root->getChildren().isEmpty()) {
            return;
        }
        obj = m_root->getChildren().first();
    }
    if (m_activeObject == obj && cmd.isEmpty()) {
        return;
    }
    bool indicatorShown = isIndicatorShown(cmd);
    if (!cmd.isEmpty() && !indicatorShown) {
        Q_EMIT obj->active(cmd);
        return;
    }
    DccObject *parent = obj;
    while (parent && parent != m_root) {
        parent = DccObject::Private::FromObject(parent)->getParent();
    }
    if (parent != m_root) {
        return;
    }
    QVector<DccObject *> modules;
    QVector<DccObject *> triggeredObjs;
    DccObject *triggeredObj = obj;
    if (triggeredObj->pageType() == DccObject::MenuEditor && !triggeredObj->getChildren().isEmpty()) {
        triggeredObj = triggeredObj->getChildren().first();
    }
    DccObject *tmpObj = triggeredObj;
    tmpObj->setCurrentObject(nullptr);
    tmpObj->active(QString());
    while (tmpObj && (tmpObj->pageType() != DccObject::Menu)) { // 页面中的控件，则激活项为父项
        triggeredObjs.append(tmpObj);
        DccObject *tmpObjParent = DccObject::Private::FromObject(tmpObj)->getParent();
        if (tmpObjParent) {
            tmpObjParent->setCurrentObject(tmpObj);
            tmpObjParent->active(QString());
        }
        tmpObj = tmpObjParent;
    }
    if (!tmpObj) {
        return;
    }
    modules.append(tmpObj);
    DccObject *p = DccObject::Private::FromObject(tmpObj)->getParent();
    while (p) {
        p->setCurrentObject(tmpObj);
        Q_EMIT p->active(QString());
        modules.append(p);
        tmpObj = p;
        p = DccObject::Private::FromObject(p)->getParent();
    }
    triggeredObjs.append(modules);
    std::reverse(modules.begin(), modules.end());
    std::reverse(triggeredObjs.begin(), triggeredObjs.end());
    auto animationMode = DccApp::AnimationPush;

    // 处理旧对象
    for (auto *oldObj : std::as_const(m_triggeredObjects)) {
        if (!triggeredObjs.contains(oldObj)) {
            oldObj->setCurrentObject(nullptr);
            animationMode = DccApp::AnimationPop;
        }
        if (oldObj != m_root && oldObj != triggeredObjs.last()) {
            Q_EMIT oldObj->deactive();
        }
    }
    setAnimationMode(animationMode);

    // 更新当前对象
    const bool currentObjectsUpdated = (m_currentObjects != modules);
    const bool triggeredObjectsUpdated = (m_triggeredObjects != triggeredObjs);
    m_currentObjects = modules;
    m_triggeredObjects = triggeredObjs;
    if (auto *lastObj = m_currentObjects.last(); lastObj != m_activeObject) {
        m_activeObject = lastObj;
        Q_EMIT activeObjectChanged(m_activeObject);
    }

    // 更新导航模型和日志
    m_navModel->setNavigationObject(m_currentObjects);
    qCInfo(dccLog) << "trigger object:" << triggeredObj->name() << " active object:" << m_activeObject->name() << " parent:" << (void *)triggeredObj->parentItem();

#ifdef HAVE_DDE_API_EVENTLOGGER
    // Reset and start page stay timer when page changes
    if (m_pageStayTimer) {
        m_pageStayTimer->stop();
        // Build page tags directly from current objects
        m_lastPageTags.clear();
        for (auto *obj : m_currentObjects) {
            if (!obj->displayName().isEmpty()) {
                m_lastPageTags.append(obj->name());
            }
        }
        m_pageStayTimer->start();
    }
#endif

    // 触发父项变更
    if (auto *parentItem = triggeredObj->parentItem(); !(triggeredObj->pageType() & DccObject::Menu) && parentItem) {
        Q_EMIT activeItemChanged(parentItem, indicatorShown);
    }

    if (currentObjectsUpdated)
        Q_EMIT currentObjectsChanged(m_currentObjects);
    if (triggeredObjectsUpdated)
        Q_EMIT triggeredObjectsChanged(m_triggeredObjects);

    m_showFallbackTimer->stop();
    if (m_needShow) {
        show();
    }

    if (!m_pageShown) {
        DccAppTimeline::instance().log(QStringLiteral("first-page-visible"));
        m_pageShown = true;
        tryStopTimeline();
    }
}

QSet<QString> findAddItems(QSet<QString> *oldSet, QSet<QString> *newSet)
{
    QSet<QString> addSet;
    for (auto &&key : *newSet) {
        if (!oldSet->contains(key)) {
            addSet.insert(key);
        }
    }
    return addSet;
}

void DccManager::updateModuleConfig(const QString &key)
{
    QSet<QString> oldModuleConfig;
    QSet<QString> *newModuleConfig = nullptr;
    uint32_t type = DCC_CONFIG_HIDDEN;
    if (key == HideConfig) {
        type = DCC_CONFIG_HIDDEN;
        oldModuleConfig = m_hideModule;
        newModuleConfig = &m_hideModule;
    } else if (key == DisableConfig) {
        type = DCC_CONFIG_DISABLED;
        oldModuleConfig = m_disableModule;
        newModuleConfig = &m_disableModule;
    } else {
        return;
    }
    const auto &list = m_dconfig->value(key).toStringList();
    // 预处理，去掉首尾空格项，去多通配符项
    newModuleConfig->clear();
    for (auto &&config : list) {
        bool isValid = false;
        // 有效字符为：字母、数字、'/'、'*'
        for (auto &c : config) {
            isValid = c == '/' || c == '*' || c.isLetterOrNumber();
            if (!isValid) {
                break;
            }
        }
        if (isValid) {
            newModuleConfig->insert(config);
        }
    }
    QSet<QString> addModuleConfig = findAddItems(&oldModuleConfig, newModuleConfig);
    QSet<QString> removeModuleConfig = findAddItems(newModuleConfig, &oldModuleConfig);
    for (auto &&url : addModuleConfig) {
        QVector<DccObject *> objs = findObjects(url);
        for (auto &&obj : objs) {
            updateConfigState(obj, type);
        }
    }
    for (auto &&url : removeModuleConfig) {
        QVector<DccObject *> objs = findObjects(url);
        for (auto &&obj : objs) {
            updateConfigState(obj, type);
        }
    }
    if (newModuleConfig == &m_hideModule && (!addModuleConfig.isEmpty() || !removeModuleConfig.isEmpty())) {
        Q_EMIT hideModuleChanged(m_hideModule);
    }
}

void DccManager::loadModuleConfigs(const QStringList &dirs, const QStringList &plugins)
{
    const QStringList subpaths = collectModulePathSubpaths(dirs, plugins);
    const QSet<QString> modulePaths = readModulePaths(subpaths);
    loadModuleConfigValues(modulePaths);
}

void DccManager::loadModuleConfigValues(const QSet<QString> &modulePaths)
{
    m_moduleConfigPending = int(modulePaths.size());
    if (modulePaths.isEmpty()) {
        m_moduleConfigReady = true;
        refreshModuleConfigState();
        return;
    }

    // 与 QML 初始化、插件加载并行读取，结果全部返回后再统一生效。
    for (const QString &path : modulePaths) {
        auto *config = DccModuleConfig::create(kAppId, QLatin1Char('/') + path, this);
        if (!config) {
            qCWarning(dccLog) << "Failed to create module config for path:" << path;
            finishModuleConfigItem();
            continue;
        }
        // path 是配置路径，保留占位符用于读取 DConfig 和定位 override 目录；
        // target 是对象匹配路径，仅在这里把保留标记 __wildcard__ 还原为 *。
        // 例如 root/network/wired__wildcard__/page -> root/network/wired*/page。
        // 沿用 contains()/findObjects() 的规则：* 只匹配单个路径段内的名称。
        QString target = path;
        target.replace(QStringLiteral("__wildcard__"), QStringLiteral("*"));
        connect(config, &DccModuleConfig::configInitializeSucceed, this, [this, target, config](DConfig *) {
            // 读生成类缓存的属性（值已在配置线程写入 Data）：不要用 config->config()->value()，
            // DConfig 实例不在主线程，每次读值都是一次跨线程阻塞往返
            if (!config->visible())
                m_moduleHiddenPaths.insert(target);
            if (config->disable())
                m_moduleDisabledPaths.insert(target);
            config->deleteLater();
            finishModuleConfigItem();
        });
        // 初始化失败（如配置元数据未安装）：记录日志并释放实例，避免静默失效
        connect(config, &DccModuleConfig::configInitializeFailed, this, [this, path, config] {
            qCWarning(dccLog) << "Failed to init module config:" << path;
            config->deleteLater();
            finishModuleConfigItem();
        });
    }
}

void DccManager::finishModuleConfigItem()
{
    if (--m_moduleConfigPending > 0)
        return;

    // 所有配置都加载完了：此时才让单模块配置生效，并统一刷一次
    m_moduleConfigReady = true;

    refreshModuleConfigState();
}

void DccManager::refreshModuleConfigState()
{
    if (!m_moduleConfigReady)
        return;

    for (const QString &url : std::as_const(m_moduleHiddenPaths)) {
        const QVector<DccObject *> objs = findObjects(url);
        for (DccObject *obj : objs)
            DccObject::Private::FromObject(obj)->setFlagState(DCC_CONFIG_HIDDEN, true);
    }
    for (const QString &url : std::as_const(m_moduleDisabledPaths)) {
        const QVector<DccObject *> objs = findObjects(url);
        for (DccObject *obj : objs)
            DccObject::Private::FromObject(obj)->setFlagState(DCC_CONFIG_DISABLED, true);
    }
}

void DccManager::updateConfigState(DccObject *obj, uint32_t type)
{
    // 两个来源在这里按“任一要求即生效”合并：
    //   ① 主配置列表 hideModule / disableModule
    //   ② 单模块配置（各 subpath 的 visible / disable），全部读完前不参与，避免边读边生效
    switch (type) {
    case DCC_CONFIG_HIDDEN:
        DccObject::Private::FromObject(obj)->setFlagState(type,
            contains(m_hideModule, obj) || (m_moduleConfigReady && contains(m_moduleHiddenPaths, obj)));
        break;
    case DCC_CONFIG_DISABLED:
        DccObject::Private::FromObject(obj)->setFlagState(type,
            contains(m_disableModule, obj) || (m_moduleConfigReady && contains(m_moduleDisabledPaths, obj)));
        break;
    default:
        break;
    }
}

void DccManager::onVisible(bool visible)
{
    if (!m_root) {
        return;
    }
    DccObject *obj = qobject_cast<DccObject *>(sender());
    if (!obj) {
        return;
    }
    if (visible) {
        QVector<DccObject *> objs;
        objs.append(obj->getChildren());
        while (!objs.isEmpty()) {
            auto o = objs.takeFirst();
            if (o->isVisibleToApp()) {
                objs.append(o->getChildren());
            } else {
                connect(o, &DccObject::visibleToAppChanged, this, &DccManager::onVisible, Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
                removeObjectFromParent(o);
                DccObject::Private::FromObject(m_hideObjects)->addChild(o, false);
            }
        }
        DccObject::Private::FromObject(m_hideObjects)->removeChild(obj);
        if (!addObjectToParent(obj)) {
            DccObject::Private::FromObject(m_noAddObjects)->addChild(obj, false);
        }
    } else {
        removeObjectFromParent(obj);
        DccObject::Private::FromObject(m_hideObjects)->addChild(obj, false);
    }
}

void DccManager::onObjectAdded(DccObject *obj)
{
    if (!m_root) {
        return;
    }
    m_searchModel->addSearchData(obj, QString(), QString());
    QVector<DccObject *> objs;
    objs.append(obj);
    while (!objs.isEmpty()) {
        auto o = objs.takeFirst();
        connect(o, &DccObject::childAdded, this, &DccManager::onObjectAdded);
        connect(o, &DccObject::childRemoved, this, &DccManager::onObjectRemoved);
        connect(o, &DccObject::displayNameChanged, this, &DccManager::onObjectDisplayChanged);
        connect(o, &DccObject::visibleToAppChanged, this, &DccManager::onVisible, Qt::ConnectionType(Qt::QueuedConnection | Qt::UniqueConnection));
        objs.append(o->getChildren());
    }
}

void DccManager::onObjectRemoved(DccObject *obj)
{
    if (!m_root) {
        return;
    }
    QVector<DccObject *> objs;
    objs.append(obj);
    while (!objs.isEmpty()) {
        auto o = objs.takeFirst();
        disconnect(o, &DccObject::childAdded, this, nullptr);
        disconnect(o, &DccObject::childRemoved, this, nullptr);
        disconnect(o, &DccObject::displayNameChanged, this, nullptr);
        m_searchModel->removeSearchData(o, QString());
        objs.append(o->getChildren());
    }
    auto it = std::find(m_triggeredObjects.begin(), m_triggeredObjects.end(), obj);
    if (it != m_triggeredObjects.end()) {
        m_triggeredObjects.erase(it, m_triggeredObjects.end());
    }
    DccObject *parentObj = m_root;
    for (auto &&o : m_currentObjects) {
        if (o == obj) {
            doShowPage(QPointer<DccObject>(parentObj), QString());
            break;
        }
        parentObj = o;
    }
}

void DccManager::onObjectDisplayChanged()
{
    if (!m_root) {
        return;
    }
    DccObject *obj = qobject_cast<DccObject *>(sender());
    if (obj) {
        m_searchModel->removeSearchData(obj, QString());
        m_searchModel->addSearchData(obj, QString(), QString());
    }
}

bool DccManager::addObjectToParent(DccObject *obj)
{
    if (const DccObject *parentObj = findParent(obj)) {
        DccObject::Private::FromObject(parentObj)->addChild(obj);
        return true;
    }
    return false;
}

bool DccManager::removeObjectFromParent(DccObject *obj)
{
    DccObject *parentObj = DccObject::Private::FromObject(obj)->getParent();
    if (parentObj) {
        DccObject::Private::FromObject(parentObj)->removeChild(obj);
        return true;
    }
    return false;
}

void DccManager::clearData()
{
    if (m_plugins->isDeleting()) {
        return;
    }
    m_imageProvider = nullptr;
    m_plugins->beginDelete();
    clearShowParam();

    m_window->hide();
    m_window->close();
    m_objMap.clear();
    // doShowPage(m_root, QString());

#ifdef DCC_ENABLE_MEMORY_MANAGEMENT
    // TODO: delete m_engine会有概率崩溃
    m_window = nullptr;
    DccObject *root = m_root;
    m_root = nullptr;
    Q_EMIT rootChanged(m_root);

    qCDebug(dccLog()) << "delete clearData hide:" << m_hideObjects->getChildren().size() << "noAdd:" << m_noAddObjects->getChildren().size() << "noParent" << m_noParentObjects->getChildren().size();
    delete m_noParentObjects;
    delete m_noAddObjects;
    delete m_hideObjects;
    qCDebug(dccLog()) << "delete dccobject";
    qCDebug(dccLog()) << "delete QmlEngine";
    delete m_engine;
    qCDebug(dccLog()) << "clear QmlEngine";
    m_engine = nullptr;
    qCDebug(dccLog()) << "delete root begin";
    delete root;
    qCDebug(dccLog()) << "delete root end";
#endif
}

void DccManager::waitLoadFinished() const
{
    if (!m_plugins->loadFinished()) {
        QEventLoop loop;
        QTimer timer;
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        connect(m_plugins, &DccPluginManager::loadAllFinished, &loop, &QEventLoop::quit);
        timer.start(5000);
        loop.exec();
    }
}

bool DccManager::loadFinished() const
{
    return m_plugins->loadFinished();
}

QList<ModuleInfo> DccManager::moduleList() const
{
    waitLoadFinished();

    struct Entry
    {
        DccObject *obj;
        ModuleInfo info;
    };

    QList<Entry> pending;
    for (auto &&child : m_root->getChildren()) {
        pending.append({ child, { child->name(), child->displayName(), child->displayName(), (int)child->weight(), false } });
    }
    for (auto &&child : m_hideObjects->getChildren()) {
        pending.append({ child, { child->name(), child->displayName(), child->displayName(), (int)child->weight(), true } });
    }

    QList<ModuleInfo> modules;
    while (!pending.isEmpty()) {
        const Entry entry = pending.takeFirst();
        modules.append(entry.info);
        const QList<DccObject *> &children = entry.obj->getChildren();
        for (auto it = children.crbegin(); it != children.crend(); ++it) {
            ModuleInfo info;
            info.url = entry.info.url + "/" + (*it)->name();
            info.displayName = (*it)->displayName();
            info.pathDisplayName = entry.info.pathDisplayName + "/" + info.displayName;
            info.weight = (int)(*it)->weight();
            info.hidden = entry.info.hidden;
            pending.prepend({ *it, info });
        }
    }
    return modules;
}

void DccManager::doGetAllModule(const QDBusMessage message) const
{
    QJsonArray arr;
    for (const auto &module : moduleList()) {
        QJsonObject obj;
        obj.insert("url", module.url);
        obj.insert("displayName", module.pathDisplayName);
        obj.insert("weight", module.weight);
        arr.append(obj);
    }

    QJsonDocument doc;
    doc.setArray(arr);
    QString json = doc.toJson(QJsonDocument::Compact);
    QDBusConnection::sessionBus().send(message.createReply(json));
}

void DccManager::onPageStayTimeout()
{
#ifdef HAVE_DDE_API_EVENTLOGGER
    qCInfo(dccLog) << "onPageStayTimeout triggered, m_lastPageTags:" << m_lastPageTags;
    if (m_lastPageTags.isEmpty()) {
        qCWarning(dccLog) << "onPageStayTimeout: m_lastPageTags is empty, skipping log";
        return;
    }

    QJsonArray tagArray;
    for (const auto &tag : m_lastPageTags) {
        tagArray.append(tag);
    }

    DDE_EventLogger::EventLogger::instance().writeEventLog(
        DDE_EventLogger::EventLoggerData(EVENT_LOGGER_CONTROL_CENTER_STAY, "control_center_config", {
            {"control_center_tag", tagArray}
        }));

    qCInfo(dccLog) << "EventLogger: page stay - tags:" << QJsonDocument(tagArray).toJson(QJsonDocument::Compact);
#endif
}

} // namespace dccV25
