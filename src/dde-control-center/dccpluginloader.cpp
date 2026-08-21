// SPDX-FileCopyrightText: 2026 UnionTech Software Technology Co., Ltd.
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "dccpluginloader.h"

#include "dccfactory.h"
#include "dccmanager.h"
#include "dccobject.h"
#include "dccobject_p.h"
#include "pluginmanager.h"

#include <DIconTheme>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QLoggingCategory>
#include <QPluginLoader>
#include <QQmlComponent>
#include <QQmlContext>
#include <QThread>

namespace dccV25 {

DccPluginLoader::DccPluginLoader(const QString &name, const QString &path, DccPluginManager *manager)
    : QObject(manager)
    , m_path(path)
    , m_status(PluginBegin)
    , m_type(T_Unknown)
    , m_factory(nullptr)
    , m_module(nullptr)
    , m_mainObj(nullptr)
    , m_soObj(nullptr)
    , m_data(nullptr)
    , m_pManager(manager)
{
    setObjectName(name);
}

DccPluginLoader::~DccPluginLoader()
{
    if (m_data) {
        qCDebug(dccLog) << "delete so data" << name();
        delete m_data;
        m_data = nullptr;
    }
}

QString DccPluginLoader::path() const
{
    return m_path;
}

DccPluginLoader::StatusFlags DccPluginLoader::status() const
{
    return m_status.load();
}

DccPluginLoader::TypeFlags DccPluginLoader::type() const
{
    return m_type;
}

DccObject *DccPluginLoader::module() const
{
    return m_module;
}

DccObject *DccPluginLoader::mainObj() const
{
    return m_mainObj;
}

DccObject *DccPluginLoader::soObj() const
{
    return m_soObj;
}

QObject *DccPluginLoader::data() const
{
    return m_data;
}

DccFactory *DccPluginLoader::factory() const
{
    return m_factory;
}

void DccPluginLoader::setLog(const QString &log)
{
    m_log.append(log);
}

bool DccPluginLoader::isFinished() const
{
    return m_status.load() & PluginEnd;
}

bool DccPluginLoader::hasError() const
{
    return m_status.load() & PluginErrMask;
}

bool DccPluginLoader::isVisibleToApp() const
{
    return !m_module || m_module->isVisibleToApp();
}

void DccPluginLoader::setType(TypeFlags type)
{
    m_type = type;
}

void DccPluginLoader::transitionStatus(StatusFlags status)
{
    StatusFlags oldStatus = m_status;
    m_status = m_status.load() | status;
    const static QMetaEnum statusFlagsMeta = QMetaEnum::fromType<StatusFlags>();
    QString statusName = statusFlagsMeta.valueToKeys(status);
    if (status & PluginErrMask) {
        qCWarning(dccLog) << name() << ": status" << QString::number(m_status.load(), 16) << statusName << m_log.join("\n");
    } else if (m_log.isEmpty()) {
        qCDebug(dccLog) << name() << ": status" << QString::number(m_status.load(), 16) << statusName;
    } else {
        qCDebug(dccLog) << name() << ": status" << QString::number(m_status.load(), 16) << statusName << m_log.join("\n");
    }
    m_log.clear();
    if (oldStatus != m_status) {
        Q_EMIT statusChanged(this, status);
    }
}

void DccPluginLoader::updateVisible(bool visibleToApp)
{
    if (!visibleToApp) {
        return;
    }
    if ((m_status.load() & PluginEnd) && (!(m_status.load() & (DataEnd | DataErr)))) {
        // 加载完成，没检查MetaData也没错误，不在hideModule中，则需要重新加载
        StatusFlags status = m_status.load() & ~PluginEnd;
        m_status = PluginBegin;
        transitionStatus(status);
    }
}

bool DccPluginLoader::updateType()
{
    QString qrcPath;
    switch (m_type & T_V_MASK) {
    case T_V1_1: {
        if (QFile::exists(m_path + "/qmldir")) {
            m_type |= T_HasModule | T_HasMain;
            qrcPath = ":/qt/qml/" + name();
            QQmlEngine *engine = m_pManager->engine();
            if (engine) {
                auto paths = engine->importPathList();
                QDir pluginDir(m_path);
                pluginDir.cdUp();
                QString qmlPlugin = pluginDir.absolutePath();
                if (!paths.contains(qmlPlugin)) {
                    engine->addImportPath(qmlPlugin);
                }
            }
        }
    } break;
    case T_V1_0: {
        QDir dir(m_path);
        if (dir.exists("main.qml")) {
            m_type |= T_ShortMain | T_HasMain;
        } else if (dir.exists(name() + "Main.qml")) {
            m_type |= T_HasMain;
        }
        if (dir.exists(name() + ".qml")) {
            m_type |= T_HasModule;
        }
        qrcPath = m_path;
    } break;
    default: {
        // Try to determine version
        m_type = T_V1_1;
        if (updateType()) {
            return true;
        }
        m_type = T_V1_0;
        if (updateType()) {
            return true;
        }
        m_type = T_Unknown;
        return false;
    } break;
    }

    if (m_type & T_DataMask) {
        QStringList paths = Dtk::Gui::DIconTheme::dciThemeSearchPaths();
        paths.append(qrcPath);
        Dtk::Gui::DIconTheme::setDciThemeSearchPaths(paths);
        return true;
    }
    return false;
}

bool DccPluginLoader::loadMetaData()
{
    if (!updateType()) {
        return false;
    }

    // Install translator for this plugin
    DccManager::installTranslator(name());

    // Skip hidden modules (this should be checked by the caller)
    if (m_pManager->hidden(name())) {
        setLog("module is hidden");
        return false;
    }
    return true;
}

bool DccPluginLoader::loadModule()
{
    if (!(m_type & T_HasModule)) {
        setLog("module qml not exists");
        return true;
    }

    QQmlComponent component(m_pManager->engine());
    switch (version()) {
    case T_V1_0: {
        const QString qmlPath = m_path + "/" + name() + ".qml";
        setLog("create module " + qmlPath);
        component.loadUrl(qmlPath);
    } break;
    case T_V1_1:
    default: {
        QString typeName = name();
        typeName[0] = typeName[0].toUpper();
        setLog("create module " + typeName);
        component.loadFromModule(name(), typeName);
    } break;
    }
    transitionStatus(ModuleCreate);
    switch (component.status()) {
    case QQmlComponent::Ready: {
        QObject *object = component.create();
        if (!object) {
            setLog("component create module object is null:" + component.errorString());
            return true;
        }
        object->setParent(m_pManager->rootModule());
        m_module = qobject_cast<DccObject *>(object);
        connect(m_module, &DccObject::visibleToAppChanged, this, &DccPluginLoader::updateVisible);
        if (m_module && !m_module->isVisibleToApp()) {
            setLog("create module finished, module is hidden");
            return false;
        }
    } break;
    case QQmlComponent::Error: {
        setLog("component create module object error:" + component.errorString());
        transitionStatus(ModuleErr);
    } break;
    default:
        break;
    }
    return true;
}

void DccPluginLoader::loadData()
{
    if (m_factory) {
        return;
    }

    const QString soPath = m_path + "/" + name() + ".so";
    if (!QFile::exists(soPath)) {
        return;
    }

    QPluginLoader loader(soPath);
    if (!loader.load()) {
        setLog("prepare factory load failed:" + loader.errorString());
        transitionStatus(DataErr);
        return;
    }

    const auto &meta = loader.metaData();
    const auto iid = meta["IID"].toString();
    if (iid.isEmpty() || iid != QString(qobject_interface_iid<DccFactory *>())) {
        setLog("prepare factory iid error:" + iid);
        transitionStatus(DataErr);
        loader.unload();
        return;
    }

    QObject *instance = loader.instance();
    if (!instance) {
        setLog("prepare factory instance failed:" + loader.errorString());
        transitionStatus(DataErr);
        loader.unload();
        return;
    }

    DccFactory *factory = qobject_cast<DccFactory *>(instance);
    if (!factory) {
        setLog("prepare factory cast failed.");
        transitionStatus(DataErr);
        loader.unload();
        return;
    }

    m_factory = factory;
}

void DccPluginLoader::createData()
{
    if (!m_factory) {
        setLog(": create data skipped");
        transitionStatus(DataErr);
        return;
    }
    m_data = m_factory->create();
}

void DccPluginLoader::createDccObject()
{
    if (m_factory) {
        m_soObj = m_factory->dccObject();
    }
}

void DccPluginLoader::moveThread()
{
    QThread *thread = this->thread();
    if (m_factory && m_factory->thread() != thread) {
        m_factory->moveToThread(thread);
    }
    if (m_data && m_data->thread() != thread) {
        m_data->moveToThread(thread);
    }
    if (m_soObj && m_soObj->thread() != thread) {
        m_soObj->moveToThread(thread);
    }
}

void DccPluginLoader::updateParent()
{
    if (m_data && m_data->parent() != this) {
        m_data->setParent(this);
    }
    if (m_soObj && m_soObj->parent() != this) {
        m_soObj->setParent(this);
    }
}

void DccPluginLoader::loadMain()
{
    if (!(m_type & T_HasMain)) {
        setLog("main qml not exists");
        transitionStatus(MainObjErr);
        return;
    }

    QQmlComponent component(m_pManager->engine());
    switch (version()) {
    case T_V1_0: {
        const QString qmlPath = m_path + "/" + ((m_type & T_ShortMain) ? "main.qml" : name() + "Main.qml");
        setLog("create Main " + qmlPath);
        component.loadUrl(qmlPath);
    } break;
    case T_V1_1:
    default: {
        QString typeName = name() + "Main";
        typeName[0] = typeName[0].toUpper();
        setLog("create Main " + typeName);
        component.loadFromModule(name(), typeName);
    } break;
    }
    transitionStatus(MainObjCreate);

    switch (component.status()) {
    case QQmlComponent::Ready: {
        QQmlContext *context = new QQmlContext(component.engine());
        context->setContextProperties({ { "dccData", QVariant::fromValue(m_data) }, { "dccModule", QVariant::fromValue(m_module) } });
        QObject *object = component.create(context);
        if (!object) {
            delete context;
            setLog(" component create main object is null:" + component.errorString());
            transitionStatus(MainObjErr);
            return;
        }
        context->setParent(object); // Context will be deleted when object is deleted
        object->setParent(m_module ? m_module : m_pManager->rootModule());
        m_mainObj = qobject_cast<DccObject *>(object);
    } break;
    case QQmlComponent::Error: {
        setLog(" component create main object error:" + component.errorString());
        transitionStatus(MainObjErr);
    } break;
    default:
        break;
    }
}

void DccPluginLoader::addMainObject()
{
    transitionStatus(MainObjAdd);

    if (!m_mainObj) {
        m_mainObj = m_soObj;
    }

    if (m_mainObj) {
        if (m_mainObj->name().isEmpty() || (m_module && m_mainObj->name() == m_module->name())) {
            // Plugin root item name is empty, associate with {name}.qml, don't add to tree
            if (m_module) {
                QQmlComponent *page = m_mainObj->page();
                if (page) {
                    m_module->setPage(page);
                }
                connect(m_mainObj, &DccObject::pageChanged, m_module, &DccObject::setPage);
                connect(m_mainObj, &DccObject::displayNameChanged, m_module, &DccObject::setDisplayName);
                connect(m_mainObj, &DccObject::descriptionChanged, m_module, &DccObject::setDescription);
                connect(m_mainObj, &DccObject::iconChanged, m_module, &DccObject::setIcon);
                connect(m_mainObj, &DccObject::badgeChanged, m_module, &DccObject::setBadge);
                connect(m_mainObj, &DccObject::visibleChanged, m_module, &DccObject::setVisible);
                connect(m_mainObj, &DccObject::active, m_module, &DccObject::active);
                connect(m_mainObj, &DccObject::deactive, m_module, &DccObject::deactive);
            }
        }
    } else {
        setLog("The plugin isn't main DccObject");
        transitionStatus(MainObjErr);
    }
}

void DccPluginLoader::cancel()
{
    setLog("cancelled");
    transitionStatus(PluginEnd);
}

void DccPluginLoader::reset()
{
    // Reset status to begin state
    m_status = PluginBegin;
    m_module = nullptr;
    m_mainObj = nullptr;
    m_soObj = nullptr;
    m_data = nullptr;
    // Don't reset factory - it can be reused
}

uint DccPluginLoader::version() const
{
    return m_type & T_V_MASK;
}

} // namespace dccV25
