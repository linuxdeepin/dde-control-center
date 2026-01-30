// SPDX-FileCopyrightText: 2024 - 2027 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later
#include "dccmanager.h"

#include "dccapp.h"
#include "dccimageprovider.h"
#include "dccobject_p.h"
#include "navigationmodel.h"
#include "pluginmanager.h"
#include "searchmodel.h"

#include <DGuiApplicationHelper>
#include <DIconTheme>

#include <QCoreApplication>
#include <QDBusConnection>
#include <QDBusPendingCall>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLoggingCategory>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QQuickWindow>
#include <QTimer>
#include <QTranslator>
#include <QWindow>

DCORE_USE_NAMESPACE

namespace dccV25 {
static Q_LOGGING_CATEGORY(dccLog, "dde.dcc.manager");

const QString WidthConfig = QStringLiteral("width");
const QString HeightConfig = QStringLiteral("height");
const QString HideConfig = QStringLiteral("hideModule");
const QString DisableConfig = QStringLiteral("disableModule");
const QString ControlCenterIcon = QStringLiteral("preferences-system");
const QString ControlCenterGroupName = "com.deepin.dde-grand-search.group.dde-control-center-setting";

DccManager::DccManager(QObject *parent)
    : DccApp(parent)
    , m_root(new DccObject())
    , m_activeObject(m_root)
    , m_hideObjects(new DccObject(this))
    , m_noAddObjects(new DccObject(this))
    , m_noParentObjects(new DccObject(this))
    , m_plugins(new PluginManager(this))
    , m_window(nullptr)
    , m_dconfig(DConfig::create("org.deepin.dde.control-center", "org.deepin.dde.control-center", QString(), this))
    , m_engine(nullptr)
    , m_navModel(new NavigationModel(this))
    , m_searchModel(new SearchModel(this))
    , m_imageProvider(nullptr)
    , m_sidebarWidth(180)
    , m_showTimer(nullptr)
{
    m_hideObjects->setName("_hide");
    m_noAddObjects->setName("_noAdd");
    m_noParentObjects->setName("_noParent");

    m_root->setName("root");
    m_root->setCanSearch(false);
    m_currentObjects.append(m_root);
    onObjectAdded(m_root);

    initConfig();
    connect(m_plugins, &PluginManager::addObject, this, &DccManager::addObject, Qt::QueuedConnection);
    waitShowPage("system", QDBusMessage());
}

DccManager::~DccManager()
{
    qCDebug(dccLog()) << "delete dccManger";
    clearData();
    delete m_plugins;
    qCDebug(dccLog()) << "delete dccManger end";
}

bool DccManager::installTranslator(const QString &name)
{
    const QStringList translateDirs = { TRANSLATE_READ_DIR,
                                        TRANSLATE_READ_DIR "/../v1.0", // 兼容旧版位置
                                        TRANSLATE_READ_DIR "/.." };
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
    m_window->installEventFilter(this);
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
    return w > 520 ? w : 780;
}

int DccManager::height() const
{
    auto h = m_dconfig->value(HeightConfig).toInt();
    return h > 400 ? h : 530;
}

int DccManager::sidebarWidth() const
{
    return m_sidebarWidth;
}

void DccManager::setSidebarWidth(int width)
{
    if (m_sidebarWidth != width) {
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
    QVector<DccObject *> objs;
    objs.append(obj);
    while (!objs.isEmpty()) {
        DccObject *o = objs.takeFirst();
        if (o->parentName().isEmpty()) {
            DccObject::Private::FromObject(m_noParentObjects)->addChild(o, false);
        } else {
            if (contains(m_hideModule, o)) {
                DccObject::Private::FromObject(o)->setFlagState(DCC_CONFIG_HIDDEN, true);
            }
            if (contains(m_disableModule, o)) {
                DccObject::Private::FromObject(o)->setFlagState(DCC_CONFIG_DISABLED, true);
            }
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
        auto message = this->message();
        setDelayedReply(true);
        QMetaObject::invokeMethod(this, &DccManager::waitShowPage, Qt::QueuedConnection, url, message);
        // show(); // 先查找再显示则注释掉此处
    } else {
        QMetaObject::invokeMethod(this, &DccManager::waitShowPage, Qt::QueuedConnection, url, QDBusMessage());
    }
}

void DccManager::showPage(DccObject *obj)
{
    QMetaObject::invokeMethod(this, "doShowPage", Qt::QueuedConnection, obj, QString());
}

void DccManager::showPage(DccObject *obj, const QString &cmd)
{
    QMetaObject::invokeMethod(this, "doShowPage", Qt::QueuedConnection, obj, cmd);
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
    QString helpTitle;
    if (1 < m_currentObjects.count())
        helpTitle = m_currentObjects.last()->name();
    if (helpTitle.isEmpty())
        helpTitle = "controlcenter";

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
                    &PluginManager::loadAllFinished,
                    this,
                    [this, json, message]() {
                        const auto &ret = this->search(json);
                        qDebug(dccLog) << "Searching finished, result size:" << ret.size();
                        QDBusConnection::sessionBus().send(message.createReply(ret));
                    },
                    Qt::SingleShotConnection);

            return {};
        }
    }
    return search(json);
}

bool DccManager::stop(const QString &)
{
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

    mainWindow()->show();
    mainWindow()->requestActivate();
    showPage(searchName);
    return true;
}

QString DccManager::GetAllModule()
{
    auto message = this->message();
    setDelayedReply(true);
    QMetaObject::invokeMethod(this, &DccManager::doGetAllModule, Qt::QueuedConnection, message);
    return QString();
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
    QWindow *w = DccManager::mainWindow();
    if (w->windowStates() == Qt::WindowMinimized || !w->isVisible())
        w->showNormal();
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
    m_sidebarWidth = m_dconfig->value("sidebarWidth", 180).toInt();
    connect(m_dconfig, &DConfig::valueChanged, this, &DccManager::updateModuleConfig);
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

// url需要在调用处保证非空
bool DccManager::isMatch(const QString &url, const DccObject *obj)
{
    Q_ASSERT(!url.isEmpty());

    QString objPath = "/" + obj->parentName() + "/" + obj->name();
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

bool DccManager::isEqual(const QString &url, const DccObject *obj)
{
    QString path = "/" + url;
    QString objPath = "/" + obj->parentName() + "/" + obj->name();
    for (auto it = path.rbegin(), itObj = objPath.rbegin(); it != path.rend() && itObj != objPath.rend(); ++it, ++itObj) {
        if (*it != *itObj) {
            return false;
        }
    }
    return true;
}

DccObject *DccManager::findObject(const QString &url, bool onlyRoot)
{
    if (!m_root || url.isEmpty()) {
        return nullptr;
    }
    QString path = url;
    if (path.startsWith("/")) {
        path = path.mid(1);
    }
    QVector<QVector<DccObject *>> objs;
    objs.append({ m_root });
    if (!onlyRoot) {
        objs.append(m_hideObjects->getChildren());
    }
    while (!objs.isEmpty()) {
        QVector<DccObject *> subObjs = objs.takeFirst();
        while (!subObjs.isEmpty()) {
            DccObject *obj = subObjs.takeFirst();
            if (isEqual(path, obj)) {
                return obj;
            }
            subObjs.append(obj->getChildren());
        }
    }
    return nullptr;
}

QVector<DccObject *> DccManager::findObjects(const QString &url, bool onlyRoot, bool one)
{
    if (!m_root || url.isEmpty()) {
        return {};
    }
    QString path = url;
    if (path.startsWith("/")) {
        path = path.mid(1);
    }
    QVector<DccObject *> rets;
    QVector<QVector<DccObject *>> objs;
    objs.append({ m_root });
    if (!onlyRoot) {
        objs.append(m_hideObjects->getChildren());
    }
    while (!objs.isEmpty()) {
        QVector<DccObject *> subObjs = objs.takeFirst();
        while (!subObjs.isEmpty()) {
            DccObject *obj = subObjs.takeFirst();
            if (isMatch(path, obj)) {
                rets.append(obj);
                if (one) {
                    return rets;
                }
            }
            subObjs.append(obj->getChildren());
        }
    }
    return rets;
}

const DccObject *DccManager::findParent(const DccObject *obj)
{
    const QString &path = obj->parentName();
    const DccObject *p = obj;
    const QObject *op = obj;
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
    if (m_dconfig->isValid()) {
        m_dconfig->setValue(WidthConfig, m_window->width());
        m_dconfig->setValue(HeightConfig, m_window->height());
    }
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
        DccManager::showPage(obj, QString());
    } else {
        int i = url.indexOf('?');
        cmd = i != -1 ? url.mid(i + 1) : QString();
        QString path = url.mid(0, i).split('/', Qt::SkipEmptyParts).join('/'); // 移除多余的/
        auto objs = findObjects(path, true, true);
        obj = objs.isEmpty() ? nullptr : objs.first();
        if (obj) {
            DccManager::showPage(obj, cmd);
        } else if (!m_plugins->loadFinished()) {
            m_showUrl = url;
            m_showMessage = message;
            if (!m_showTimer) {
                m_showTimer = new QTimer(this);
                connect(m_showTimer, &QTimer::timeout, this, &DccManager::tryShow);
                m_showTimer->start(10);
            }
            return;
        }
    }
    if (message.type() != QDBusMessage::InvalidMessage) {
        if (obj) {
            if (cmd.isEmpty() || isIndicatorShown(cmd)) {
                show();
            }
            QDBusConnection::sessionBus().send(message.createReply());
        } else {
            QDBusConnection::sessionBus().send(message.createErrorReply(QDBusError::InvalidArgs, QString("not found url:") + url));
        }
    }
}

void DccManager::clearShowParam()
{
    if (m_showTimer) {
        m_showTimer->stop();
        m_showTimer->deleteLater();
        m_showTimer = nullptr;
    }
    if (!m_showUrl.isEmpty()) {
        m_showUrl.clear();
        m_showMessage = QDBusMessage();
    }
}

void DccManager::tryShow()
{
    if (m_showUrl.isEmpty()) {
        clearShowParam();
        return;
    }
    int i = m_showUrl.indexOf('?');
    QString cmd = i != -1 ? m_showUrl.mid(i + 1) : QString();
    QString path = m_showUrl.mid(0, i).split('/', Qt::SkipEmptyParts).join('/'); // 移除多余的/
    DccObject *obj = findObject(path, true);
    if (obj) {
        showPage(obj, cmd);
        if (m_showMessage.type() != QDBusMessage::InvalidMessage) {
            if (cmd.isEmpty()) {
                show();
            }
            QDBusConnection::sessionBus().send(m_showMessage.createReply());
        }
        clearShowParam();
    } else if (m_plugins->loadFinished()) {
        if (m_showMessage.type() != QDBusMessage::InvalidMessage) {
            QDBusConnection::sessionBus().send(m_showMessage.createErrorReply(QDBusError::InvalidArgs, QString("not found url:") + m_showUrl));
        }
        clearShowParam();
    }
}

void DccManager::doShowPage(DccObject *obj, const QString &cmd)
{
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
    QList<DccObject *> modules;
    DccObject *triggeredObj = obj;
    if (triggeredObj->pageType() == DccObject::MenuEditor && !triggeredObj->getChildren().isEmpty()) {
        triggeredObj = triggeredObj->getChildren().first();
    }
    DccObject *tmpObj = triggeredObj;
    while (tmpObj && (tmpObj->pageType() != DccObject::Menu)) { // 页面中的控件，则激活项为父项
        tmpObj = DccObject::Private::FromObject(tmpObj)->getParent();
    }
    if (!tmpObj) {
        return;
    }
    modules.append(tmpObj);
    DccObject *p = DccObject::Private::FromObject(tmpObj)->getParent();
    while (p) {
        p->setCurrentObject(tmpObj);
        Q_EMIT p->active(QString());
        modules.prepend(p);
        tmpObj = p;
        p = DccObject::Private::FromObject(p)->getParent();
    }

    auto animationMode = DccApp::AnimationPush;

    // 处理旧对象
    for (auto *oldObj : std::as_const(m_currentObjects)) {
        if (!modules.contains(oldObj)) {
            oldObj->setCurrentObject(nullptr);
            animationMode = DccApp::AnimationPop;
        }
        if (oldObj != m_root && oldObj != modules.last()) {
            Q_EMIT oldObj->deactive();
        }
    }
    setAnimationMode(animationMode);

    // 触发新对象
    if (!cmd.isEmpty()) {
        Q_EMIT triggeredObj->active(cmd);
    }

    // 更新当前对象
    m_currentObjects = modules;
    if (auto *lastObj = m_currentObjects.last(); lastObj != m_activeObject) {
        m_activeObject = lastObj;
        Q_EMIT activeObjectChanged(m_activeObject);
    }

    // 更新导航模型和日志
    m_navModel->setNavigationObject(m_currentObjects);
    qCInfo(dccLog) << "trigger object:" << triggeredObj->name() << " active object:" << m_activeObject->name() << " parent:" << (void *)triggeredObj->parentItem();

    // 触发父项变更
    if (auto *parentItem = triggeredObj->parentItem(); !(triggeredObj->pageType() & DccObject::Menu) && parentItem) {
        Q_EMIT activeItemChanged(parentItem, indicatorShown);
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
            DccObject::Private::FromObject(obj)->setFlagState(type, true);
        }
    }
    for (auto &&url : removeModuleConfig) {
        QVector<DccObject *> objs = findObjects(url);
        for (auto &&obj : objs) {
            DccObject::Private::FromObject(obj)->setFlagState(type, false);
        }
    }
    if (newModuleConfig == &m_hideModule && (!addModuleConfig.isEmpty() || !removeModuleConfig.isEmpty())) {
        Q_EMIT hideModuleChanged(m_hideModule);
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
    DccObject *parentObj = m_root;
    for (auto &&o : m_currentObjects) {
        if (o == obj) {
            doShowPage(parentObj, QString());
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
    // doShowPage(m_root, QString());

#ifdef DCC_ENABLE_MEMORY_MANAGEMENT
    // TODO: delete m_engine会有概率崩溃
    if (m_window) {
        delete m_window;
    }
    qCDebug(dccLog()) << "delete root begin";
    DccObject *root = m_root;
    m_root = nullptr;
    Q_EMIT rootChanged(m_root);
    delete root;
    qCDebug(dccLog()) << "delete root end";

    qCDebug(dccLog()) << "delete clearData hide:" << m_hideObjects->getChildren().size() << "noAdd:" << m_noAddObjects->getChildren().size() << "noParent" << m_noParentObjects->getChildren().size();
    QVector<DccObject *> deleteObjects;
    deleteObjects.append(m_hideObjects);
    deleteObjects.append(m_noAddObjects);
    deleteObjects.append(m_noParentObjects);
    while (!deleteObjects.isEmpty()) {
        auto obj = deleteObjects.takeFirst();
        QVector<DccObject *> children = obj->getChildren();
        while (!children.isEmpty()) {
            delete children.first();
            children = obj->getChildren();
        }
        delete obj;
    }
    qCDebug(dccLog()) << "delete dccobject";
    qCDebug(dccLog()) << "delete QmlEngine";
    delete m_engine;
    qCDebug(dccLog()) << "clear QmlEngine";
    m_engine = nullptr;
#endif
}

void DccManager::waitLoadFinished() const
{
    if (!m_plugins->loadFinished()) {
        QEventLoop loop;
        QTimer timer;
        connect(&timer, &QTimer::timeout, &loop, &QEventLoop::quit);
        connect(m_plugins, &PluginManager::loadAllFinished, &loop, &QEventLoop::quit);
        timer.start(5000);
        loop.exec();
    }
}

void DccManager::doGetAllModule(const QDBusMessage message) const
{
    waitLoadFinished();
    DccObject *root = m_root;
    QList<QPair<DccObject *, QStringList>> modules;
    for (auto &&child : root->getChildren()) {
        modules.append({ child, { child->name(), child->displayName() } });
    }

    QJsonArray arr;
    while (!modules.isEmpty()) {
        const auto &urlInfo = modules.takeFirst();
        QJsonObject obj;
        obj.insert("url", urlInfo.second.at(0));
        obj.insert("displayName", urlInfo.second.at(1));
        obj.insert("weight", (int)(urlInfo.first->weight()));
        arr.append(obj);
        const QList<DccObject *> &children = urlInfo.first->getChildren();
        for (auto it = children.crbegin(); it != children.crend(); ++it)
            modules.prepend({ *it, { urlInfo.second.at(0) + "/" + (*it)->name(), urlInfo.second.at(1) + "/" + (*it)->displayName() } });
    }

    QJsonDocument doc;
    doc.setArray(arr);
    QString json = doc.toJson(QJsonDocument::Compact);
    QDBusConnection::sessionBus().send(message.createReply(json));
}

} // namespace dccV25
